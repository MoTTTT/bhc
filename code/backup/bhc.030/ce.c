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
byte	ce_stat;			/* Control Engine state variable*/
byte	ce_dout;			/* Control Engine Dig Output(ro)*/
byte	ce_din;				/* Control Engine Dig Input	*/
byte	v_stat;				/* Valve control state		*/
byte	a_stat;				/* Alarm status			*/
byte	f_set;				/* Flow Setting			*/
byte	l_set;				/* Low Level Setting		*/
byte	ce_run;				/* Public RUN flag		*/
byte	ce_lll;				/* Low Level Lockout Flag	*/
byte	ce_man;				/* Manual Flag			*/
bit	ui_flag;			/* User interface timer flag	*/
bit	db_flag;			/* Data Logger timer flag	*/
/*		Local Includes, Parameters and Variables		*/
#include	<reg552.h>		/* Microcontroller registers	*/
#include	<adc.h>			/* ADC Routines			*/
/*		Control Engine Parameters and Variables			*/
#define	ADC_IMIN	64		/* ADC minimum current loop read*/
#define	I_MIN		64		/* ADC minimum current loop read*/
#define	T_RH		229		/* Timer reload value high	*/
#define	ADC_EN	((char*) 0x2F02FL )	/* ADC enable latch		*/
#define	DIG_OUT	((char*) 0x2F20FL )	/* Digital Output		*/
#define	D_VO		0x80		/* Digital Out: Valve Open	*/
#define	D_VC		0x40		/* Digital Out: Valve Close	*/
#define	D_V		0xC0		/* Digital Out: Valve (both)	*/
#define	D_RUN		0x20		/* Digital Out: Motor Run	*/
#define D_L		0x08		/* Digital Out: Low Lev Lockout	*/
#define	D_T		0x02		/* Digital Out: Test		*/
#define D_P		0x01		/* Digital Out: Power		*/
#define	D_I		0x00		/* Digital Out: Idle		*/
#define	DIG_IN	((char*) 0x2F20FL )	/* Digital Input		*/
#define	D_OV		0x80		/* Digital In: Overload		*/
#define	D_PF		0x40		/* Digital In: Phase Fail	*/
#define	CE_TMAX		0xFFFFFFFF	/* Long integer full scale	*/
#define	PRE_CE		10		/* Control Engine Prescaler 10ms*/
#define	PRE_UI		250		/* UI Flag Prescaler  250ms	*/
#define	PRE_DB		1000		/* Data Logger Prescaler 15s	*/
byte	pre_ce=		PRE_CE;		/* CE Prescale counter		*/
byte	pre_ui=		PRE_UI;		/* UI Prescale counter		*/
ulong	pre_db=		PRE_DB;		/* Data Logger Prescale counter	*/
ulong	ce_time=	0;		/* CE Time: No of ms since init	*/
sbit	CE_TEST=	0xC7;		/* DEBUG Port Pin (P4.7)	*/

byte	ce_d		( byte out );	/* Write to Output Port		*/
byte	ce_dset		( byte msk );	/* Set Output Port Pin/s	*/
byte	ce_dclr		( byte msk );	/* Clear Output Port Pin/s	*/
byte	ce_dtog		( byte msk );	/* Toggle Output Port Pin/s	*/

#include	"flow.c"		/* Flow Routines		*/
#include	"valve.c"		/* Valve Routines		*/
#include	"press.c"		/* Pressure Measurement 	*/
#include	"water.c"		/* Water Level Measurement 	*/

#define	CE_WARM		0xAA01		/* Initialisation check pattern	*/
uint	ce_boot;			/* Should be declared last	*/

byte	ce_d		( byte out )	/* Write to Output Port		*/
{
	return(*DIG_OUT= ce_dout= out);	/* Set Output Port Absolute	*/
}

byte	ce_dset		( byte msk )	/* Set Output Port Pin/s	*/
{
	return(*DIG_OUT=ce_dout|=msk);	/* Set bits in mask		*/
}

byte	ce_dclr		( byte msk )	/* Clear Output Port Pin/s	*/
{
	return(*DIG_OUT=ce_dout&=~msk);	/* Clear bits in mask	 	*/
}

byte	ce_dtog		( byte msk )	/* Toggle Output Port Pin/s	*/
{
	if	( ce_dout& msk )	/* If any of the bits are set	*/
		return( ce_dclr( msk ));/*  Clear them all, otherwise	*/
	else	return( ce_dset( msk ));/*  Set them all.		*/
}

void	start		( void )	/* Start Borhole		*/
{
	ce_stat=	CE_STA;		/* Start CE			*/
	v_stat=		V_O;		/* Adjust valve			*/
	ce_dclr		( D_V );	/* Clear Valve control		*/
	ce_dset		( D_VO+ D_RUN );/* Open Valve, Start motor	*/
}

void	stop		( void )	/* Stop Borehole		*/
{
	v_stat=		V_C;		/* Adjust valve			*/
	ce_dclr		( D_V+ D_RUN );	/* Clear Valve cont, Stop Motor	*/
	ce_dset		( D_VC );	/* Close Valve			*/
	ce_stat=	CE_STOP;	/* Change State			*/
}

void	shutdown	( void )	/* Shut down control engine	*/
{
	if	( a_stat& A_LL )	/* Low Level Lockout		*/
		ce_dset	( D_L );	/* Set LLL output		*/
	ce_stat=	CE_ERR;		/* Change to Error state	*/
	ce_run=		0;		/* Clear START signal		*/
	v_stat=		V_C;		/* Adjust valve			*/
	ce_dclr		( D_V+ D_RUN );	/* Clear Valve cont, Stop Motor	*/
	ce_dset		( D_VC );	/* Close Valve			*/
}

void	ce_reset	( void )	/* Reset Control Engine		*/
{
	ce_sllev( W_DLL );		/* Set Default Low Level	*/
	ce_sflow( F_DEF );		/* Set Default Flow		*/
	ce_run=		0;		/* Init Public RUN flag		*/
	ce_lll=		0;		/* Init Low Level Lockout Flag?	*/
	ce_man=		0;		/* Init Manual Flag		*/
	ce_boot= CE_WARM;		/* Set used flag		*/
	f_tot=		0;		/* Reset total flow		*/
}

bit	coldboot	( void )	/* Long downtime detection	*/
{
	if	( ce_boot!= CE_WARM )	/* RAM power lost		*/
	{
		ce_reset( );		/* Set system variables default	*/
		return	( 1 );		/* Return power lost confirm	*/
	}
	return	( 0 );			/* Return False			*/
}

void	init_ce		( void )	/* Initialise Control Engine	*/
{
	ce_d		( D_I );	/* Clear outputs		*/
	*ADC_EN=	0xFF;		/* Enable ADC inputs		*/
	init_flow	( );		/* Start flow pulse sampler	*/
	init_adc	( );		/* Initialise A2D converter	*/
	ET0=		1;		/* Enable timer 0 interrupt	*/
	TMOD&=		0xF0;		/* T0: Mode 0			*/
	TR0=		1;		/* Start Control Engine Timer	*/
	coldboot ( );			/* Check for long downtime	*/
	if		( ce_run )	/* Check current status		*/
		start	( );		/* Resume			*/
	else	stop	( );		/* Ensure system is shut down	*/
}

void	ce_sig		( byte in )	/* Userland signal		*/
{
	switch	( in )
	{
	case S_STA:			/* Recieved START Signal	*/
		ce_run=		1;	/* Set Control Engine RUN flag	*/
		break;
	case S_STO:			/* Recieved STOP Signal		*/
		ce_run=		0;	/* Reset Control Engine RUN flag*/
		break;
	case S_RLL:			/* Reset Low Level Lockout	*/
		ce_dclr	( D_L );	/* Reset LLL output		*/
		ce_reset( );		/* Set to system defaults	*/
		break;
	case S_DEB:			/* Debug test event		*/
		ce_dtog	( D_T );	/* Toggle Test Output		*/
		break;
	case S_PT:			/* Toggle Power Indicator	*/
		ce_dtog	( D_P );	/* Toggle Power Output		*/
		break;
	default: break;
	}
}

void	p_ce	( void )		/* Process Control Engine	*/
{
	if	( a_stat& A_SM )	/* Shutdown condition/s detected*/
	{
		if ( ce_stat!= CE_ERR )	/* Change of state		*/
			shutdown ( );	/* Shut down system		*/
		return;			/* Exit if (still) shut down	*/
	}
	switch	( ce_stat )
	{
	case CE_IDLE:			/* CE Idle			*/
		if	( ce_run )	/* CE Recieved RUN		*/
			start ( );	/* Start Controled System	*/
		break;
	case CE_STA:			/* CE started			*/
		if	( v_stat== V_S )/* Valve set to default		*/
			ce_stat= CE_RUN;/* CE stat: Running		*/
		if	( !ce_run )	/* CE received STOP		*/
			stop ( );	/* Shut down control engine	*/
		break;
	case CE_RUN:			/* CE Running			*/
		if	( !ce_run )	/* CE received STOP		*/
			stop ( );	/* Shut down control engine	*/
		break;
	case CE_STOP:			/* CE busy shutting down	*/
		if	( v_stat== V_I )/* Valve Closed			*/
			ce_stat=CE_IDLE;/* Return to IDLE		*/
		break;
	case CE_ERR:			/* CE was in Error state	*/
		ce_stat=CE_IDLE;	/* Change to Idle state		*/
		break;
	default:			/* Illegal State		*/
		ce_stat=CE_IDLE;	/* Change to Idle state		*/
		break;
	}
}

void	p_alrm	( void )		/* Process Alarms		*/
{
	ce_din=	*DIG_IN;		/* Read digital input lines	*/
	if	( ce_din& D_PF )	/* Phase Fail Condition Detected*/
		a_stat|= A_PF;		/* Set alarm			*/
	else	a_stat&= ~A_PF;		/* Reset alarm			*/
	if	( ce_din& D_OV )	/* Overload condition detected	*/
		a_stat|= A_OV;		/* Set alarm			*/
	else	a_stat&= ~A_OV;		/* Reset Alarm			*/
	p_water	( );			/* Check Water level		*/
	p_press	( );			/* Check Pressure		*/
}

void	t0_int	( void ) interrupt 1	/* Control Engine Timer		*/
{
	ce_time++;			/* Increment ms counter (Clock)	*/
	TH0=	T_RH;			/* Reload timer for T= 1 mS	*/
	flow_poll( );			/* Check for flow pulse		*/
	if	( !--pre_ui ) 		/* Screen Update		*/
	{				/* Occurs every UI_PRE ms	*/
		ui_flag= 1;		/* Set User Interface Flag	*/
		pre_ui= PRE_UI;		/* Reset UI prescaler		*/
	}
	if	( !--pre_db ) 		/* Data Logger			*/
	{				/* Occurs every PRE_DB ms	*/
		db_flag= 1;		/* Set User Interface Flag	*/
		pre_db= PRE_DB;		/* Reset UI prescaler		*/
	}
	if	( pre_ce== 1 )		/* Prepare for CE cycle in 1ms	*/
	{				/* Occurs every C_PRE ms	*/
		adc_start ( );		/* Start A2D conversions	*/
	}
	if	( !pre_ce )		/* Main Control Processor Run	*/
	{				/* Occurs every C_PRE ms	*/
		CE_TEST=1; /* DEBUG*/	/* Set Port Pin (P4.7)		*/
		if	( adc_read )	/* Should be finished, but check*/
		{
			return;		/* Try again next tick		*/
		}
		pre_ce=	PRE_CE;		/* Reset CE Prescaler		*/
		p_alrm	( );		/* Process Alarms		*/
		p_ce	( );		/* Process Control engine	*/
		if	( !--v_pre )	/* Valve Controller Run		*/
		{			/* Occurs every C_PRE X V_PRE ms*/
			p_valv ( );	/* Process Valve		*/
			v_pre= V_PRE;	/* Reset Valve prescaler	*/
		}
		CE_TEST=0;		/* DEBUG Port Pin (P4.7)	*/
	}
	pre_ce--;			/* Decrement CE mS counter	*/
}