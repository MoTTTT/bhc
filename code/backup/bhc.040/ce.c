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
#include	<reg552.h>		/* Microcontroller registers	*/
#include	<rtc.h>			/* RTC related routines		*/
#include	<time.h>		/* Time related routines	*/
#include	<adc.h>			/* ADC Routines			*/
#include	<ce.h>
/*		Control Engine Parameters and Variables			*/
//#define T_RH		229		/* Timer reload value high 1ms	*/
#define	T_RH		252		/* Timer reload value high	*/
#define	CE_TMAX		0xFFFFFFFF	/* Long integer full scale	*/
#define	DT_MAX		60* 60		/* Max downtime before recovery	*/
#define	PRE_1MS		7		/* Frequency of interrupt (Hz)	*/
//#define	PRE_1MS	1		/* Frequency of interrupt (Hz)	*/
#define	PRE_CE		10		/* Control Engine Prescaler 10ms*/
#define	PRE_UI		330		/* UI Flag Prescaler  (ms)	*/
#define	PRE_DB		500		/* Data Logger Prescaler in ms 	*/
#define	CE_WARM		0xAA01		/* Initialisation check pattern	*/
byte	pre_1ms=	PRE_1MS;	/* 1 ms  Prescale counter	*/
byte	pre_ce=		PRE_CE;		/* CE Prescale counter		*/
byte	pre_ui=		PRE_UI;		/* UI Prescale counter		*/
ulong	pre_db=		PRE_DB;		/* Data Logger Prescale counter	*/
ulong	ce_time=	0;		/* CE Time: No of ms since init	*/
byte	ce_stat;			/* Control Engine state variable*/
byte	ce_mode;			/* Control Engine mode variable	*/
byte	ce_cos;				/* Control Engine ChangeOf State*/
byte	v_stat;				/* Valve control state		*/
byte	a_stat=		0;		/* Alarm status			*/
time	ce_t0;				/* CE Start Time: RTC at init	*/
time	ce_tn;				/* CE Start Time: latest RTC 	*/
sbit	CE_TEST=	0xC7;		/* DEBUG Port Pin (P4.7)	*/
uint	ce_boot;			/* Should be declared last	*/
#include	"io.c"			/* General I/O Routines		*/
#include	"flow.c"		/* Flow Routines		*/
#include	"valve.c"		/* Valve Routines		*/
#include	"press.c"		/* Pressure Measurement 	*/
#include	"water.c"		/* Water Level Measurement 	*/

void	ce_reset	( void )	/* Reset Control Engine		*/
{
	ce_sllev( W_DLL );		/* Set Default Low Level	*/
	ce_sflow( F_DEF );		/* Set Default Flow		*/
	ce_run=		0;		/* Init Public RUN flag		*/
	ce_lll=		0;		/* Init Low Level Lockout Flag?	*/
	ce_man=		0;		/* Init Manual Flag		*/
	ce_sin=		0;		/* Control Engine Signal Input	*/
	ce_dout=	0;		/* Control Engine Digital Output*/
	f_tot=		0;		/* Reset total flow		*/
}

void	start		( void )	/* Start Borehole		*/
{
	ce_stat=	CE_STA;		/* Start CE			*/
	v_stat=		V_O;		/* Adjust valve			*/
	ce_dclr		( D_V );	/* Clear Valve control		*/
	ce_dset		( D_VO );	/* Open Valve			*/
	if	( ce_mode!= CE_MAN )	/* Not in manual mode		*/
		ce_dset	( D_RUN );	/* Start motor			*/
}

void	stop		( byte next )	/* Stop Borehole		*/
{
	ce_run=		0;		/* Clear START signal		*/
	v_stat=		V_C;		/* Adjust valve			*/
	ce_dclr		( D_V );	/* Clear Valve cont		*/
	if	( ce_mode!= CE_MAN )	/* Not in manual mode		*/
		ce_dclr	( D_RUN );	/* Stop motor			*/
	ce_dset		( D_VC );	/* Close Valve			*/
	ce_stat=	next;		/* Change to next State		*/
	ce_cos++;			/* Set COS flag			*/
}

bit	coldboot	( void )	/* Long downtime detection	*/
{
	if	( ce_boot!= CE_WARM )	/* RAM power lost		*/
	{
		ce_boot= CE_WARM;	/* Set used flag in RAM		*/
		ce_reset( );		/* Set system variables default	*/
		return	( 1 );		/* Return power lost confirm	*/
	}
	if(dif_time(&ce_tn,&ce_t0)>DT_MAX)/* Check downtime length	*/
	{
		ce_reset( );		/* Set system variables default	*/
		return	( 1 );		/* Return downtime confirm	*/
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
	coldboot	( );		/* Check for long downtime	*/
	if		( ce_run )	/* Check current status		*/
		start	( );		/* Resume			*/
	else	stop	( CE_STOP );	/* Ensure system is shut down	*/
}

bit	p_mode	( void )		/* Update CE Mode		*/
{
	if	( a_stat& A_SM )	/* Shutdown condition/s detected*/
	{
		if ( ce_stat!= CE_ERR )	/* Change of state		*/
		{
			stop ( CE_ERR );/* Shut down system		*/
			if(a_stat& A_LL)/* Low Level Lockout		*/
				ce_dset(D_L);/* Set LLL output		*/
		}
		return ( 0 );		/* Exit if (still) Shut Down	*/
	}
	if(!((ce_din&D_MAN)|(ce_din&D_AUTO)))	/* Mode: Off		*/
	{
		if ( ce_mode!= CE_OFF )	/* Change of state		*/
		{
			ce_mode=CE_OFF;	/* Change mode state		*/
			stop (CE_STOP);	/* Shut down the system		*/
		}
		return ( 0 );		/* Exit if still OFF		*/
	}
	if	( ce_din& D_MAN )	/* Manual Mode			*/
	{
		if ( ce_mode!= CE_MAN )	/* Change of state		*/
		{
			if ( ce_mode!= CE_OFF )	/* Error COS		*/
				stop (CE_STOP);	/* Shut down system	*/
			ce_mode= CE_MAN;	/* Change mode		*/
			ce_cos++;		/* Flag COS		*/
		}
		if ( ce_din& D_R )	/* Motor Running		*/
		{
			if ( !ce_run )	/* Change of state		*/
			{
				ce_run=1;/* Start Flow control		*/
				ce_cos++;/* Set COS flag		*/
			}
		}
		else			/* Motor off			*/
		{
			if ( ce_run )	/* Change of state		*/
			{
				ce_run=0;/* Stop Flow control		*/
				ce_cos++;/* Set COS flag		*/
			}
		}
		return	( 1 );		/* Continue CE processing	*/
	}
	if	( ce_mode!= CE_AUTO )	/* Change of Mode		*/
	{
		ce_mode= CE_AUTO;	/* Set Mode			*/
		ce_cos++;		/* Flag COS			*/
	}
	return	( 1 );			/* Continue CE processing	*/
}

void	p_alrm	( void )		/* Process Alarms		*/
{
		CE_TEST=1;/* DEBUG*/	/* Set Port Pin (P4.7)		*/
	ce_din=	*DIG_IN;		/* Read digital input lines	*/
	p_overl	( );			/* Check Overload Condition	*/
	p_phase	( );			/* Check Phase Fail Condition	*/
	p_water	( );			/* Check Water level		*/
	p_press	( );			/* Check Pressure		*/
		CE_TEST=0;/* DEBUG */	/* Clear Port Pin (P4.7)	*/
}

void	p_ce	( void )		/* Process Control Engine	*/
{
	p_alrm	( );			/* Process Alarms		*/
	if	( !p_mode( ))		/* Update CE Mode		*/
		return;			/* Exit if CE Shut Down		*/
	switch	( ce_stat )		/* Select control engine status	*/
	{
	case CE_IDLE:			/* CE Idle			*/
		if	( ce_run )	/* CE Recieved RUN		*/
			start ( );	/* Start Controled System	*/
		break;
	case CE_STA:			/* CE started			*/
		if	( v_stat== V_S )/* Valve set to default		*/
			ce_stat= CE_RUN;/* CE stat: Running		*/
		if	( !ce_run )	/* CE received STOP		*/
			stop ( CE_STOP );/* Shut down control engine	*/
		break;
	case CE_RUN:			/* CE Running			*/
		if	( !ce_run )	/* CE received STOP		*/
			stop ( CE_STOP );/* Shut down control engine	*/
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
	}
}

void	p_1ms	( void )		/* >1 ms period processes	*/
{
	ce_time++;			/* Increment ms counter (Clock)	*/
	if	( ce_sin )		/* Check for Userland signal	*/
		p_sig ( );		/* Process it			*/
	if	( !--pre_ui ) 		/* Screen Update		*/
	{				/* Occurs every PRE_UI ms	*/
		ui_flag= 1;		/* Set User Interface Flag	*/
		pre_ui= PRE_UI;		/* Reset UI prescaler		*/
	}
	if	( !--pre_db ) 		/* Data Logger			*/
	{				/* Occurs every PRE_DB ms	*/
		db_flag= 1;		/* Set Logger Flag		*/
		pre_db= PRE_DB;		/* Reset DB prescaler		*/
	}
	if	( pre_ce== 1 )		/* Prepare for CE cycle in 1ms	*/
		adc_start ( );		/* Start A2D conversions	*/
	if	( pre_ce== 0 )		/* Main Control Processor Run	*/
	{				/* Occurs every C_PRE ms	*/
		if	( adc_read )	/* Should be finished, but check*/
			return;		/* Try again next tick		*/
		p_ce	( );		/* Process Control engine	*/
		if	( !--v_pre )	/* Valve Controller Run		*/
		{			/* Occurs every C_PRE X V_PRE ms*/
			p_valv ( );	/* Process Valve		*/
			v_pre= V_PRE;	/* Reset Valve prescaler	*/
		}
		pre_ce=	PRE_CE;		/* Reset CE Prescaler		*/
	}
	pre_ce--;			/* Decrement CE mS counter	*/
}

void	t0_int	( void ) interrupt 1	/* Control Engine Timer		*/
{
	TH0=	T_RH;			/* Reload timer for F ~= 9 Hz	*/
	flow_poll( );			/* Check for flow pulse		*/
	if	( !--pre_1ms ) 		/* 1 ms resolution tasks	*/
	{				/* Occurs every 1 ms		*/
		p_1ms	( );		/* >1 ms period processes	*/
		pre_1ms= PRE_1MS;	/* Reset 1ms prescaler		*/
	}
}