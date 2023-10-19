/*			Copyright Q Solutions				*/
/*	File:		LcdPad.c					*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		IIC Bus Keypad & Screen Module			*/
/*									*/
/*			History						*/
/* 17:29 30/03/1997	Written from scratch, using uNode Library	*/
/*			Initialisation and main loop.			*/
/*									*/

#pragma		ROM (COMPACT)
#pragma		SMALL

/* Target Specific parameters. Change these to generate final ROM image	*/
#define		KEYCTRL		/* Keypad Control Port			*/
#define 	KEYMON		/* Keypad Monitor Port 			*/
#define		SCREEN		/* LCD Control Port			*/
#define 	IICADR		/* IIC Bus Address			*/

/* Library files							*/
#include	<stdio.h>
#include	<reg552.h>

typedef	unsigned int uint;
typedef	unsigned char byte;
#include	<cbkey.h>
/*#include	<portlcd.h>*/
#include	<sio1.h>
#include	<n3inc.h>	/* uNode library			*/

void	init_uNode	( void )
{
//	EX1	= 1;				/* ENABLE XINT1		*/
//	IT1	= 1;				/* XINT1 -> edge int	*/
//	EA	= 1;				/* ENABLE GLOBAL INT	*/
//	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
//	init_sio0( );
//	lcd_addr=	0x82;
	init_lcd( );
	sio1_init( 0x31 );
	init_RTC ( );
	set_lcd	( LINE_1 );
	printf	( "\xe4Node3" );
	set_lcd	( LINE_2 );
	printf	( "LcdPad" );
}

void	init_lcd1	( )	/* Initialise LCD module	*/
{
}

void	initialise	( )	/* Initialise Hardware		*/
{
	init_uNode	( );	/* DEV	code			*/
	init_cbkey	( );	/* Initialise cross bar switch	*/
	init_lcd1	( );	/* Initialise LCD on port SCREEN*/
}

void	service_keypad	( )	/* Read and process keypad	*/
{

}

void	paint_screen	( )	/* Draw or update the screen	*/
{

}

void	main	( void )
{
uint	counter;
	initialise	( );
	while	( 1 )
	{
		counter++;
		service_keypad	( );	/* Process input	*/
		paint_screen	( );    /* Update LCD		*/
		show_time( LINE_2+ 8 );
		show_date( LINE_1+ 11);
	}
}
