/*			Copyright Q Solutions				*/
/*	File:		ce.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine					*/
/*									*/
/*			History						*/
/* 16:01 27/04/1997  	Written from scratch				*/
/*									*/
#pragma		LARGE
typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef unsigned long ulong;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/
/*		Public Includes, Defines and Variables			*/
#include	<ce.h>
byte	ce_stat=	CE_IDLE;	/* Control Engine state variable*/
byte	v_stat=		V_I;		/* Valve control state		*/
byte	a_stat=		0;		/* Alarm status			*/
byte	f_flo=		0;		/* Current Flow value		*/
byte	f_set=		0;		/* Flow Setting			*/
byte	l_set=		0;		/* Low Level Setting		*/
bit	ce_run=		0;		/* Public RUN flag		*/
bit	overload=	0;		/* Overload Flag		*/
bit	phase_fail=	0;		/* Phase Fail flag		*/
bit	ui_flag=	0;		/* User interface period flag	*/
/*		Local Includes, Defines and Variables			*/
#include	<reg552.h>		/* Microcontroller registers	*/
#include	<adc.h>			/* ADC Routines			*/
#define	V		0		/* ADC channel: Valve		*/
#define	V_PRE		10		/* Valve Control Prescaler 	*/
#define V_SF		256/100		/* Scale factor Byte - %	*/
#define	V_CLO		5*V_SF		/* Valve position: Closed	*/
#define	V_DEF		75*V_SF		/* Valve Position: Default	*/
#define	V_OPN		95*V_SF		/* Valve position: Full open	*/
#define	V_PLEN		10		/* Valve pulse length		*/
#define	P		1		/* ADC channel: Pressure	*/
#define	P_CNT		10		/* Number of pressure samples	*/
#define	P_TH		200		/* Pressure alarm threshold hi	*/
#define	P_TL		190		/* Pressure threshold lo	*/
#define	P_FS		10		/* Pressure: Full Scale Factor	*/
#define	P_SHH		0.7*P_CNT	/* Pres: sample high hi theshold*/
#define	P_SHL		0.3*P_CNT	/* Pres: sample high lo theshold*/
#define	P_SLH		0.7*P_CNT	/* Pres: sample low hi theshold	*/
#define	P_SLL		0.3*P_CNT	/* Pres: sample low lo theshold	*/
#define	W		2		/* ADC channel: Water level	*/
#define	W_FS		25		/* Water level: Full scale	*/
#define	W_CNT		10		/* Number of water samples	*/
#define	W_TL		70		/* Low Level threshold		*/
#define	W_SL		0.3*W_CNT	/* Water Low threshold off	*/
#define	W_SH		0.7*W_CNT	/* Water Low threshold on	*/
#define	FI		0		/* Flow interrupt vector	*/
#define	F_EI		EX0		/* Flow interrupt enable	*/
#define	F_TL		5		/* Flow threshold low		*/
#define	F_TH		10		/* Flow threshold high		*/
#define	CE_PRE		10		/* Control Engine Prescaler 	*/
#define	ALRM_PRE	10		/* Alarm Sensor Prescaler 	*/
#define	UI_PRE		250		/* UI Flag Prescaler 		*/
#define	ADC_IMIN	64		/* ADC minimum current loop read*/
#define	T_RH		229		/* Timer reload value high	*/
sbit	TEST0=		0x90;		/* DEBUG Port Pin (P1.0)	*/
byte	ce_pre=		CE_PRE;		/* CE Prescale counter		*/
byte	alrm_pre=	ALRM_PRE;	/* ALARM Prescale counter	*/
byte	ui_pre=		UI_PRE;		/* UI Prescale counter		*/
byte	v_pre=		V_PRE;		/* VALVE Prescale counter	*/
sbit	v_open=		0x91;		/* Valve open control line	*/
sbit	v_close=	0x92;		/* Valve close control line	*/
byte	v_pulse=	0;		/* Valve pulse timer		*/
byte	v_cur;				/* Current Valve position	*/
uint	p_htot=		0;		/* Pressure high sum		*/
uint	p_ltot=		0;		/* Pressure low sum		*/
byte	p_cnt=		0;		/* Pressure sample count	*/
byte	p_ave=		0;		/* Pressure 			*/
float	pressure=	0;		/* Pressure			*/
uint	w_tot=		0;		/* Water sample sum		*/
byte	w_cnt=		0;		/* Water sample count		*/
byte	w_ave=		0;		/* Water sample average		*/
float	water_level=	0;		/* Water level			*/
byte	f_ave=		0;		/* Flow average			*/
ulong	f_tot=		0;		/* Flow total			*/
byte	f_cnt=		0;		/* Flow interrupt count		*/

void	init_flow	( void )	/* Initialise Flow Measurement	*/
{
	f_cnt=		0;		/* Reset Flow interrupt count	*/
	f_flo=		0;		/* Reset Current Flow value	*/
	F_EI=		1;		/* Enable External Interrupt 	*/
}

void	init_ce		( void )	/* Initialise Control Engine	*/
{
	ET0=	1;			/* Enable timer 1 interrupt	*/
	TCON=	0x20;			/* T0: Mode 0			*/
	TR0=	1;			/* Start timer			*/
	init_adc	( );		/* Initialise Analog Converter	*/
	init_flow	( );		/* Initialise Flow Meter	*/
}

bit	ce_sflow	( byte in )	/* Set Desired Flow		*/
{
	if	(( in> 0 )& ( in< 40 ))	/* New Flow in range		*/
	{
		f_set=	in;		/* Set new value		*/
		return	( 1 );
	}
	return	( 0 );			/* Error, not setting		*/
}

bit	ce_sllev	( byte in )	/* Set Low Level		*/
{
	if	(( in> 1 )& ( in< 20 ))	/* New Level in range		*/
	{
		l_set=	in;		/* Set new value		*/
		return	( 1 );
	}
	return	( 0 );			/* Error, not setting		*/
}

void	ce_sig		( byte in )	/* Userland signal		*/
{
	switch	( in )
	{
	case S_STA:			/* Recieved START Signal	*/
		ce_run=		1;	/* Reset Control Engine RUN flag*/
		break;
	case S_STO:			/* Recieved STOP Signal		*/
		ce_run=		0;	/* Reset Control Engine RUN flag*/
		break;
	case S_RLL:			/* Reset Low Level Lockout	*/
		break;
	}
}

void	p_valv	( void )		/* Process Valve		*/
{
	switch	( v_stat )
	{
	case V_I:			/* Valve control Idle		*/
		break;
	case V_O:			/* Valve Opening		*/
		if((adc[V]>V_OPN)|!ce_run)/* Default Position, or STOP	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_C:			/* Valve Closing		*/
		if((adc[V]<V_CLO)|ce_run)/* Closed, or START received	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_I;	/* Change state			*/
		}
		break;
	case V_P:			/* Adjusting valve position	*/
		if	( !(v_pulse--) )/* Check for end of pulse	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_S:			/* Valve position set		*/
		if	( f_flo< F_TL )	/* Flow below threshold		*/
		{
			v_open=	1;	/* Start Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_P;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		if	( f_flo> F_TH )	/* Flow above threshold		*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 1;	/* Start Actuator Close		*/
			v_stat= V_P;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		break;
	case V_E:			/* Valve control error		*/
		break;
	default:			/* Invalid state		*/
		v_stat= V_I;		/* Set to default		*/
		break;
	}
}

void	p_press		( void )	/* Check pressure condition	*/
{
	p_ave=	adc[P];			/* Get ADC Value		*/
	if	( p_ave < ADC_IMIN )	/* Sensor error			*/
		pressure= 0;		/* Set sane value		*/
	else
	{
		pressure= p_ave- ADC_IMIN;	/* Subtract offset	*/
		pressure/= 255-ADC_IMIN;	/* Scale to unit	*/
		pressure*= P_FS;		/* Scale to measurement	*/
	}
	if	( adc[P]> P_TH )	/* High Pressure ?		*/
		p_htot++;		/* Notch one up			*/
	if	( adc[P]< P_TL )	/* Low Pressure ?		*/
		p_ltot++;		/* Notch one up			*/
	if	( ++p_cnt== P_CNT )	/* Full set of samples		*/
	{
		if	( p_htot> P_SHH)/* High pressure count high	*/
			a_stat|= A_PH;	/* Switch on warning flag	*/
		if	( p_htot< P_SHL)/* High pressure count low	*/
			a_stat&= ~A_PH;	/* Reset warning		*/
		if	( p_ltot> P_SLH)/* Low pressure count high	*/
			a_stat|= A_PL;	/* Switch on warning flag	*/
		if	( p_htot< P_SLL)/* High pressure count low	*/
			a_stat&= ~A_PL;	/* Reset warning		*/
		p_cnt=p_htot=p_ltot=0;	/* Reset count, sum		*/
	}
}

void	p_water		( void )	/* Check Water condition	*/
{
	w_ave=	adc[W];			/* Get ADC value		*/
	if	( w_ave < ADC_IMIN )	/* Sensor error			*/
		water_level= 0;		/* Set sane value		*/
	else
	{
		water_level= w_ave- ADC_IMIN;	/* Subtract offset	*/
		water_level/= 255-ADC_IMIN;	/* Scale to unit	*/
		water_level*= W_FS;		/* Scale to measurement	*/
	}
	if	( adc[W]< W_TL )	/* Is level below threshold ?	*/
		w_tot++;		/* Notch one up			*/
	if	( ++w_cnt== W_CNT )	/* Full set of samples		*/
	{
		if	( w_tot> W_SH )	/* Low level count high		*/
			a_stat|= A_WL;	/* Switch on alarm flag		*/
		if	( w_tot< W_SL )	/* Low level count low		*/
			a_stat&= ~A_WL;	/* Reset alarm			*/
		w_cnt= w_tot=	0;	/* Reset count, sum		*/
	}
}

void	p_alrm	( void )		/* Process Alarms		*/
{
	if	( phase_fail )		/* Phase Fail Condition Detected*/
		a_stat|= A_PF;		/* Set alarm			*/
	else	a_stat&= ~A_PF;		/* Reset alarm			*/
	if	( overload )		/* Overload condition detected	*/
		a_stat|= A_OV;		/* Set alarm			*/
	else	a_stat&= ~A_OV;		/* Reset Alarm			*/
	p_water	( );			/* Check Water level		*/
	p_press	( );			/* Check Pressure		*/
}

void	startup	( )			/* Start Control Engine		*/
{
	ce_stat=	CE_STA;		/* Start CE			*/
	v_stat=		V_O;		/* Adjust valve			*/
	v_close=	0;		/* No Actuator Close		*/
	v_open=		1;		/* Start Actuator Open		*/
}

void	shutdown	( void )	/* Shut down control engine	*/
{
	v_stat=		V_C;		/* Adjust valve			*/
	v_open=		0;		/* No Actuator Open		*/
	v_close=	1;		/* Start Actuator Close		*/
}

void	p_ce	( void )		/* Process Control Engine	*/
{
	if	( a_stat )		/* Alarm condition/s detected	*/
	{
		ce_stat= CE_ERR;	/* Change to Error state	*/
		ce_run=	0;		/* Clear START signal		*/
		shutdown ( );		/* Shut down system		*/
	}
	switch	( ce_stat )
	{
	case CE_IDLE:			/* CE Idle			*/
		if	( ce_run )	/* CE Recieved RUN		*/
			startup ( );	/* Start Control engine		*/
		break;
	case CE_STA:			/* CE started			*/
		if	( v_stat== V_S )/* Valve set to default		*/
			ce_stat= CE_RUN;/* CE stat: Running		*/
		if	( !ce_run )	/* CE received STOP		*/
		{
			shutdown ( );	/* Shut down control engine	*/
			ce_stat=CE_STOP;/* Change State			*/
		}
		break;
	case CE_RUN:			/* CE Running			*/
		if	( !ce_run )	/* CE received STOP		*/
		{
			shutdown ( );	/* Shut down control engine	*/
			ce_stat=CE_STOP;/* Change State			*/
		}
		break;
	case CE_STOP:			/* CE busy shutting down	*/
		if	( v_stat== V_I )/* Valve Closed			*/
			ce_stat=CE_IDLE;/* Return to IDLE		*/
		break;
	case CE_ERR:			/* CE in Error, Reset		*/
		if	( !a_stat )	/* No Alarm condition/s detected*/
		{
			ce_stat=CE_IDLE;/* Change to Idle state		*/
			break;
		}
		break;
	default:			/* Illegal State		*/
		ce_stat=CE_IDLE;	/* Change to Idle state		*/
		break;
	}
}

void	check_v		( void )	/* Check valve control		*/
{
}

byte	calc_flow	( void )	/* Calculate flow		*/
{
	return ( 0 );
}

void	flow_int( void ) interrupt FI	/* 1 Litre Flow interrupt	*/
{
	f_flo=	calc_flow ( );		/* Calculate flow		*/
	if	( ce_stat== CE_RUN )	/* Adjusting for flow		*/
	{
		if	( v_stat== V_S )/* Previous control complete	*/
		{
			if	( f_flo< F_TL )	/* Flow below threshold	*/
				v_stat= V_O;	/* Pulse open valve	*/
			if	( f_flo> F_TH )	/* Flow above threshold	*/
				v_stat= V_C;	/* Pulse close valve	*/
		}
		else	check_v ( );	/* Check for valve control error*/
	}
}

void	t0_int	( void ) interrupt 1	/* Control Engine Timer		*/
{
	TEST0=	1;			/* DEBUG: Control Cycle Timing	*/
	TH0=	T_RH;			/* Reload timer for T= 1 mS	*/
	if	( ce_pre== 1 )		/* Prepare for CE cycle in 1ms	*/
	{				/* Occurs every C_PRE ms	*/
		adc_start ( );		/* Start A2D conversions	*/
	}
	if	( !ce_pre )		/* Main Control Processor Run	*/
	{				/* Occurs every C_PRE ms	*/
		if	( adc_read )	/* Should be finished, but check*/
			return;		/* Try again next tick		*/
		p_alrm	( );		/* Process Alarms		*/
		p_ce	( );		/* Process Control engine	*/
		if	( !v_pre )	/* Valve Controller Run		*/
		{			/* Occurs every C_PRE X V_PRE ms*/
			p_valv ( );	/* Process Valve		*/
			v_pre= V_PRE;	/* Reset Valve prescaler	*/
		}
		v_pre--;		/* Decrement prescaler		*/
		ce_pre=	CE_PRE;		/* Reset CE Prescaler		*/
	}
	ce_pre--;			/* Decrement prescaler		*/
	if	( !ui_pre )		/* Screen Update		*/
	{				/* Occurs every UI_PRE ms	*/
		ui_flag= 1;		/* Set User Interface Flag	*/
		ui_pre= UI_PRE;		/* Reset UI prescaler		*/
	}
	ui_pre--;			/* Decrement UI prescaler	*/
	TEST0=	0;			/* DEBUG			*/
}