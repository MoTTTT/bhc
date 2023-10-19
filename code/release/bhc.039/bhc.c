/*			Copyright Q Solutions				*/
/*	File:		bhc.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		Main Controller Module				*/
/*									*/
/*			History						*/
/* 00:08 21/04/1997 	Written from scratch				*/
/* 11:29am 07-01-1997 	Source lines: 1262	(not incl library)	*/
/* 11:41am 07-13-1997  	Source lines: 1505	(library: 1464 lines)	*/
/*			First beta release: V0.01			*/
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
#ifndef	BYTETYPE
#define BYTETYPE
typedef	unsigned char byte;		/* Use unsigned char and int	*/
#endif
#include	<rtc.h>			/* Real time clock routines	*/
#include	<time.h>		/* Time related routines	*/
#include	<ce.h>			/* Control Engine routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/
#include	"ui.c"			/* Include User Interface Code	*/

sbit BHC_TEST= 0xC6;			/* UI Timing Test Point		*/

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	BHC_TEST=	0;		/* Clear UI Timing Test point	*/
	PCON|=		0x80;		/* Double Baud Rate		*/
	init_ce		( );		/* Initialise Control Engine	*/
	init_serial	( B192 );	/* Initialise Serial Port	*/
	init_iic	( BHC_ADR );	/* Initialise I2C Bus 		*/
	init_rtc	( );		/* Initialise Real Time Clock	*/
	init_rkpad	( );		/* Initialise I2C Keypad	*/
	init_i2clcd	( );		/* Initialise I2C Bus LCD	*/
	ce_sig		( S_PT );	/* Toggle Power LED		*/
	get_time	( &ce_t0 );	/* Store CE startup time	*/
}

void	proc_db	( void )		/* Database Log Processor	*/
{
	db_flag= 0;			/* Reset timer flag		*/
	out_stream=	IO_232;		/* Print to serial port		*/
	if	( get_time(&ce_tn) )	/* Store CE time		*/
	{
		if (!tim2lng(&ce_tn))	/* Check for midnight		*/
			ce_sig(S_RF);	/* Reset Total Flow		*/
		print_time ( &ce_tn );	/* Print the time		*/
		printf	( " " );	/* Print a delimiter		*/
	}
	else	printf	( " NO RTC! " );/* Print RTC error		*/
	ui_slog ( );			/* Log status to serial port	*/
}

void	main	( void )		/* Main Loop			*/
{
	initialise	( );		/* Set up drivers, sign on	*/
	while		( 1 )		/* Repeat Forever		*/
	{
		if	( test_rkey( ))	/* Remote keypress		*/
			proc_rkey ( );	/* Process input		*/
		if	( db_flag )	/* Data Logger Timer		*/
			proc_db	( );
		if	( ui_flag )	/* Display refresh period	*/
		{
			BHC_TEST= 1;	/* UI Timing test point start	*/
			wdog	( 0 );	/* Watchdog: (CE and UI running)*/
			ui_update ( );	/* Refresh info display		*/
			BHC_TEST= 0;	/* UI Timing test point end	*/
		}
	}
}