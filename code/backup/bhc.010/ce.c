/*			Copyright Q Solutions				*/
/*	File:		ce.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine					*/
/*									*/
/*			History						*/
/* 16:01 27/04/1997  	Written from scratch				*/
/*									*/

/*			Standard Library Header Files			*/
#include	<reg552.h>
/*			Control Engine Parameters			*/
#define	T_RH		0x00		/* Timer reload value high,	*/
#define	T_RL		0x00		/* 	and Low Byte		*/
#define	CE_PRE		100		/* Control Engine Prescaler 	*/
#define	ALRM_PRE	100		/* Alarm Sensor Prescaler 	*/
#define	VALV_PRE	100		/* Valve Control Prescaler 	*/
#define	IDLE		0		/* CE State: IDLE		*/
#define	STOP		1		/* CE State: Stopped		*/
#define	START		2		/* CE State: Started		*/
#define	ERR		3		/* CE State: Error		*/
/*			Includes, Defines and Variables			*/
typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/
typedef	byte xdata xbyte;		/* External data byte		*/
typedef	uint xdata xint;		/* External data uint		*/
#include	<adc.h>			/* ADC Routines			*/
xbyte	ce_pre=		CE_PRE;		/* CE Prescale counter		*/
xbyte	alrm_pre=	ALRM_PRE;	/* ALARM Prescale counter	*/
xbyte	valv_pre=	VALV_PRE;	/* VALVE Prescale counter	*/
xbyte	ce_stat=	IDLE;		/* Control Engine state variable*/
bit	cos_pub=	1;		/* Public Change of state flag	*/

void	init_ce	( void )		/* Initialise Control Engine	*/
{
	init_adc	( );		/* Initialise Analog Converter	*/
	ET0=	1;			/* Enable timer 1 interrupt	*/
	TCON=	0x20;			/* T0: Mode 0			*/
	TH0=	T_RH;			/* Set timer reload value high,	*/
	TL0=	T_RL;			/* 	and Low Byte		*/
	TR0=	1;			/* Start timer			*/
	adc_start	( );		/* Start an A2D converstion	*/
}

void	ce_stop		( void )	/* Stop Control Engine		*/
{
	ce_stat= STOP;			/* Control Engine state variable*/
	cos_pub= 1;
}

void	ce_start	( void )	/* Start Control Engine		*/
{
	ce_stat= START;			/* Control Engine state variable*/
	cos_pub= 1;
}

bit	ce_cos		( void )	/* Public change of state test	*/
{
	if	( cos_pub )		/* If true			*/
	{
		cos_pub= 0;		/* Reset			*/
		return	( 1 );		/* Return true			*/
	}
	else	return	( 0 );		/* Return False			*/
}

void	proc_valv	( void )	/* Process Valve		*/
{
}

void	proc_alrm	( void )	/* Process Alarms		*/
{
}

void	t0_int	( void ) interrupt 1	/* Control Engine Timer		*/
{
	if	( !ce_pre-- )		/* Prescale Control Engine	*/
	{
		ce_pre=	CE_PRE;		/* Reset Control Engine Presc.	*/
		if	( !alrm_pre-- )	/* Prescale Alarm detector	*/
		{
			alrm_pre= ALRM_PRE;	/* Reset Prescaler	*/
			proc_alrm ( );	/* Process Alarms		*/
		}
		if	( !valv_pre-- )	/* Prescale Valve Controller	*/
		{
			valv_pre= VALV_PRE;	/* Reset prescaler	*/
			proc_valv ( );	/* Process Valve		*/
		}
	}
}