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
#pragma		LARGE
/*			Standard Library Header Files			*/
#include	<stdio.h>
#include	<reg552.h>
/*			uNode Library Header Files			*/
typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned long ulong;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/
#include	<ce.h>			/* Control Engine routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/
#include	<rtc.h>			/* Real time clock routines	*/
bit	bhc_man=	0;		/* Borehole Manual Flag		*/
#include	"ui.c"			/* Include User Interface Code	*/

sbit BHC_TEST= 0xC6;

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	init_ce		( );		/* Initialise Control Engine	*/
	init_iic	( BHC_ADR );	/* Initialise I2C Bus 		*/
	init_rtc	( );		/* Initialise Real Time Clock	*/
	init_rkpad	( );		/* Initialise I2C Keypad	*/
	init_i2clcd	( );		/* Initialise I2C Bus LCD	*/
}

void	main	( void )		/* Main Loop			*/
{
	initialise	( );		/* Set up drivers, sign on	*/
	while		( 1 )		/* Repeat Forever		*/
	{
		BHC_TEST= 0;
		counter++;		/* Increment loop counter	*/
		wdog	( 100 );	/* Refresh watchdog		*/
		if	( test_rkey( ))	/* Remote keypress		*/
			proc_rkey ( );	/* Process input		*/
		if	( ui_flag )	/* Display refresh period	*/
			ui_update ( );	/* Refresh info display		*/
		BHC_TEST= 1;
	}
}