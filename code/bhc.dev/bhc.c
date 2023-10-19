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
/* 00:39 01/05/1998 	MODBUS Driver Added				*/
/*									*/

/*			Compiler Directives				*/
#pragma		ROM (COMPACT)
#pragma		LARGE
/*			Standard Library Header Files			*/
#include	<stdio.h>
#include	<reg552.h>
/*			uNode Library Header Files			*/
#include	<types.h>		/* byte, uint, and ulong defines*/
#include	<rtc.h>			/* Real time clock routines	*/
#include	<time.h>		/* Time related routines	*/
#include	<ce.h>			/* Control Engine routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/
#include	"db.c"			/* Database logging routines	*/
#include	"ui.c"			/* Include User Interface Code	*/
#include	"scada.c"		/* Include SCADA Interface Code	*/

sbit BHC_TEST= 0xC6;			/* UI Timing Test Point		*/

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	BHC_TEST=	0;		/* Clear UI Timing Test point	*/
	init_ce		( );		/* Initialise Control Engine	*/
	init_serial	( B9600 );	/* Initialise Serial Port	*/
	init_modb	( MADDR );	/* Initialise Modbus database	*/
	init_iic	( BHC_ADR );	/* Initialise I2C Bus 		*/
	init_rtc	( );		/* Initialise Real Time Clock	*/
	init_rkpad	( );		/* Initialise I2C Keypad	*/
	init_i2clcd	( );		/* Initialise I2C Bus LCD	*/
	ce_sig		( S_PT );	/* Toggle Power LED		*/
	get_time	( &ce_t0 );	/* Store CE startup time	*/
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
		if	( modb_new( ) )	/* Modbus frame received	*/
			proc_mb	( );
		if	( ui_flag )	/* Display refresh period	*/
		{
			BHC_TEST= 1;	/* UI Timing test point start	*/
			wdog	( 0 );	/* Watchdog: (CE and UI running)*/
			ui_update ( );	/* Refresh info display		*/
			BHC_TEST= 0;	/* UI Timing test point end	*/
		}
	}
}