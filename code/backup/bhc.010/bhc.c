/*			Copyright Q Solutions				*/
/*	File:		bhc.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		Main Controller Module				*/
/*									*/
/*			History						*/
/* 00:08 21/04/1997 	Written from scratch				*/
/*									*/

/*			Compiler Directives				*/
#pragma		ROM (COMPACT)
#pragma		SMALL
/*			Standard Library Header Files			*/
#include	<stdio.h>
#include	<reg552.h>
/*			uNode Library Header Files			*/
typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/
typedef	uint xdata xuint;		/* External Data byte		*/
typedef	byte xdata xbyte;		/* External Data uint		*/
#include	<ce.h>			/* Control Engine routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/
//#include	<rtc.h>			/* Real time clock routines	*/

void	proc_com ( char command );	/* Process commands from keypad	*/
bit	bhc_run=	0;		/* Borehole running flag	*/
#include	"ui.c"			/* Include User Interface Code	*/

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	init_ce		( );		/* Initialise Control Engine	*/
	init_iic	( BHC_ADR );	/* Initialise I2C Bus 		*/
//	init_rtc	( );		/* Initialise Real Time Clock	*/
	init_rkpad	( );		/* Initialise I2C Keypad	*/
	init_i2clcd	( );		/* Initialise I2C Bus LCD	*/
}

void	proc_com	( char command )/* Process commands from keypad	*/
{
	set_rlcd( RL_L2+ 8 );		/* Set cursor to Command field	*/
	switch	( command )
	{
	case 'R':			/* Run/Stop Pressed		*/
		if	( !bhc_run )
		{
			printf( C_R );	/* Print command name		*/
			bhc_run= 1;	/* Change state			*/
			ce_start ( );	/* Start Engine			*/
		}
		else
		{
			printf( C_S );	/* Print command name		*/
			bhc_run= 0;	/* Change state			*/
			ce_stop ( );	/* Stop Engine			*/
		}
		break;
	default:
		printf	( "%c       ", command );
		break;
	}
}

void	main	( void )		/* Main Loop			*/
{
	initialise	( );		/* Set up drivers, sign on	*/
	while	( 1 )
	{
		counter++;		/* Increment loop counter	*/
		wdog	( 10 );		/* Refresh watchdog		*/
		if	( test_rkey( ))	/* Remote keypress		*/
			proc_rkey ( );	/* Process input		*/
		if	( ce_cos ( ) )	/* Change of system status	*/
			proc_status( );	/* Update Status Field		*/
	}
}
