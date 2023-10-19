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
/*		Public Includes, Parameters and Variables		*/
#include	<ce.h>
byte	ce_stat=	CE_IDLE;	/* Control Engine state variable*/
byte	ce_dout=	0;		/* Control Engine Dig Output(ro)*/
byte	v_stat=		V_I;		/* Valve control state		*/
byte	a_stat=		0;		/* Alarm status			*/
byte	f_flo=		0;		/* Current Flow value		*/
byte	f_set=		0;		/* Flow Setting			*/
byte	l_set=		0;		/* Low Level Setting		*/
bit	ce_run=		0;		/* Public RUN flag		*/
bit	ce_lll=		0;		/* Low Level Lockout Flag	*/
bit	ce_man=		0;		/* Manual Flag			*/
bit	overload=	0;		/* Overload Flag		*/
bit	phase_fail=	0;		/* Phase Fail flag		*/
bit	ui_flag=	0;		/* User interface period flag	*/
/*		Local Includes, Parameters and Variables		*/
#include	<reg552.h>		/* Microcontroller registers	*/
#include	<adc.h>			/* ADC Routines			*/
/*		Control Engine Parameters and Variables			*/
#define	PRE_CE		10		/* Control Engine Prescaler 	*/
#define	PRE_ALRM	10		/* Alarm Sensor Prescaler 	*/
#define	PRE_UI		125		/* UI Flag Prescaler  125mS	*/
#define	ADC_IMIN	64		/* ADC minimum current loop read*/
#define	I_MIN		64		/* ADC minimum current loop read*/
#define	T_RH		229		/* Timer reload value high	*/
#define	ADC_EN	((char*) 0x2F02FL )	/* ADC enable latch		*/
#define	DIG_OUT	((char*) 0x2F20FL )	/* Digital Output		*/
sbit	CE_TEST=	0xC7;		/* DEBUG Port Pin (P4.7)	*/
byte	pre_ui=		PRE_UI;		/* UI Prescale counter		*/
byte	pre_ce=		PRE_CE;		/* CE Prescale counter		*/
byte	pre_alrm=	PRE_ALRM;	/* ALARM Prescale counter	*/
ulong	ce_time=	0;		/* CE Time: No of ms since init	*/
#include	"flow.c"		/* Flow Routines		*/
#include	"valve.c"		/* Valve Routines		*/
#include	"press.c"		/* Pressure Measurement 	*/
#include	"water.c"		/* Water Level Measurement 	*/

void	init_ce		( void )	/* Initialise Control Engine	*/
{
	*ADC_EN=	0xFF;		/* Enable ADC inputs		*/
	init_adc	( );		/* Initialise Analog Converter	*/
	ce_out	( 0x00 );		/* Reset Digital Output		*/
//	init_flow	( );		/* Initialise Flow Meter	*/
	ce_sllev	( W_DLL );	/* Set Default Low Level	*/
	ce_sflow	( F_DEF );	/* Set Default Flow		*/
	ET0=	1;			/* Enable timer 0 interrupt	*/
	TCON=	0x20;			/* T0: Mode 0			*/
	TR0=	1;			/* Start Control Engine Timer	*/
}

byte	ce_out		( byte out )	/* Write to Output Port		*/
{
	return(*DIG_OUT= ce_dout= out);	/* Set public variable too	*/
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
	case S_DEB:			/* Debug test event		*/
		ce_out ( ce_dout? 0: 0xFF );/* Set Digital Output	*/
		break;
	default: break;
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
	if	( a_stat& A_SM )	/* Shutdown condition/s detected*/
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
		if (!(a_stat& A_SM ))	/* No Shutdown condition	*/
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

void	t0_int	( void ) interrupt 1	/* Control Engine Timer		*/
{
	ce_time++;			/* Increment ms counter (Clock)	*/
	TH0=	T_RH;			/* Reload timer for T= 1 mS	*/
	if	( !--pre_ui ) /*heh*/	/* Screen Update		*/
	{				/* Occurs every UI_PRE ms	*/
		ui_flag= 1;		/* Set User Interface Flag	*/
		pre_ui= PRE_UI;		/* Reset UI prescaler		*/
	}
	if	( pre_ce== 1 )		/* Prepare for CE cycle in 1ms	*/
	{				/* Occurs every C_PRE ms	*/
		CE_TEST=1; /* DEBUG*/	/* Set Port Pin (P4.7)		*/
		adc_start ( );		/* Start A2D conversions	*/
		CE_TEST=0;		/* DEBUG Port Pin (P4.7)	*/
	}
	if	( !--pre_ce )		/* Main Control Processor Run	*/
	{				/* Occurs every C_PRE ms	*/
		pre_ce=	PRE_CE;		/* Reset CE Prescaler		*/
		if	( adc_read )	/* Should be finished, but check*/
			return;		/* Try again next tick		*/
		p_alrm	( );		/* Process Alarms		*/
		p_ce	( );		/* Process Control engine	*/
		if	( !--v_pre )	/* Valve Controller Run		*/
		{			/* Occurs every C_PRE X V_PRE ms*/
			p_valv ( );	/* Process Valve		*/
			v_pre= V_PRE;	/* Reset Valve prescaler	*/
		}
	}
}