/*			Copyright Q Solutions				*/
/*	File:		LcdPad.c					*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		IIC Bus Keypad & Screen Module			*/
/*									*/
/*			History						*/
/* 17:29 30/03/1997	Written from scratch, using uNode Library	*/
/*			Initialisation and main loop.			*/
/* 11:35 12/04/1997 	uNode Library removed, code cleanup.		*/
/* 23:37 13/04/1997 	IIC Bus support added,				*/
/*			State machine implimented.			*/

#pragma		ROM (COMPACT)
#pragma		SMALL

/* Library files							*/
#include	<stdio.h>
#include	<reg552.h>

/* Target Specific parameters. Change these to generate final ROM image	*/
#define 	LP_ADR	0x60		/* IIC Slave Address		*/
#define 	BHC_ADR	0x68		/* Main Controller Address	*/
#define		COLUMNS	16		/* Number of LCD Columns	*/

typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/

#include	<cbkey.h>		/* Keypad Driver routines	*/
#include	<portlcd.h>		/* LCD routines			*/
#include	<iicdriv.h>		/* IIC Bus routines		*/
#include	<rtc.h>			/* Real time clock routines	*/
#include	<wdog.h>		/* Watchdog refresh		*/

/* Local defines							*/
#define		S_IDLE	0x10		/* Idle State			*/
#define		S_000	0		/* Testing for peripherals	*/
#define		S_R00	1		/* RTC present			*/
#define		S_0K0	2		/* Keypad ACK			*/
#define		S_RK0	3		/* RTC, Keypad response		*/
#define		S_00L	4		/* LCD Control			*/
#define		S_R0L	5		/* RTC, LCD Control		*/
#define		S_0KL	6		/* BHC present, No RTC		*/
#define		S_RKL	7		/* Full Connection		*/
#define		S_SET	0x20		/* Changing settings, time etc	*/
#define		PING	100		/* Send dummy keycode		*/
const char *signon1= "    Borehole    ";/* Line 1 of sign on screen	*/
const char *signon2= "   Controller   ";/* Line 2 of signon screen	*/
const char *modname= "  LcdPad Module ";/* Header			*/
const char *mes_off= "Off Line        ";/* Off line message		*/
const char *mes_rtc= "Connect         ";/* Print Confirmation		*/
const char *mes_snd= "Sending:        ";/* Print Status			*/
const char *err_wr=  "   Write Error  ";/* Print Status			*/

const char key_map[20]=	{ 'C', 'E', '.', '0', '*',	/* Keycode to	*/
			  'D', 'U', '9', '8', '7',	/* character	*/
			  126, 127, '6', '5', '4',	/* mapping	*/
			  'O', 'R', '3', '2', '1' };

bit		dirty= 1;		/* Sreen repaint flag		*/
char idata	iic_in[2];		/* IIC Bus input buffer		*/
char idata	iic_out[2];		/* IIC Bus output buffer	*/
byte		state= S_IDLE;		/* LcdPad state variable	*/

void	initialise	( )		/* Initialise Hardware		*/
{
char	i= 1;
	EA	= 1;			/* ENABLE GLOBAL INT		*/
	init_plcd	( );		/* Initialise LCD on port SCREEN*/
	while		( i++ );	/* Wait for LCD to reset	*/
	init_cbkey	( );		/* Initialise cross bar switch	*/
	iic_init	( LP_ADR );	/* Initialise IIC Bus		*/
	iic_sset	( iic_in, 2 );	/* Set up slave input buffer	*/
}

char	putchar		( char out )
{
	return	( pputchar ( out ) );
}

void	proc_iic	( )
{
	switch	( iic_in[0] )		/* Command			*/
	{
	case 0:	init_plcd( );		/* Set LCD to defaults		*/
		break;
	case 1:	set_plcd( iic_in[1] );	/* Write LCD control		*/
		break;
	case 2: pputchar( iic_in[1] );	/* Write LCD data		*/
		break;
	case 3: break;			/* Read LCD control		*/
	case 4: break;			/* Read LCD data		*/
	default:
		break;
	}
	iic_sready=	0;		/* Clear slave reception flag	*/
}

bit	proc_key	( )		/* Read and process keypad	*/
{
	iic_out[0]= 	read_cbkey( );	/* Read key from keypad		*/
	iic_mstart (BHC_ADR,1,iic_out);	/* Send it to BHC		*/
	if	( !iic_wait( ) )	/* No reply			*/
		return	( 1 );		/* Return error			*/
	return	( 0 );
}

void	paint	( char *l1, char *l2 )	/* Draw Screen			*/
{
	if	( dirty )		/* Only if necessary		*/
	{
		set_plcd( LCD_L1 );	/* Goto Line 1			*/
		printf	( l1 );		/* Print first string		*/
		set_plcd( LCD_L2 );	/* Goto Line 2			*/
		printf	( l2 );		/* Print second string		*/
		dirty= 0;		/* Done				*/
	}
}

char	test_key	( )		/* Check for keypad acknowledge	*/
{
	iic_out[0]= 	PING;		/* Send dummy key		*/
	iic_mstart	( BHC_ADR, 1, iic_out );
	return		( iic_wait( ));	/* If no reply: Return Error	*/
}

void	main	( void )		/* Main Loop			*/
{
uint	counter= 0;			/* Routine cycle counter	*/
byte	skip= 0;			/* Test periph every 256 loops	*/
   initialise	( );			/* Set up drivers		*/
   while	( 1 )			/* Do the following forever	*/
   {
	wdog	( 100 );		/* Refresh the Watchdog timer	*/
	switch	( state )		/* Act appropriate to state	*/
	{
	case S_IDLE:			/* Display Product Name		*/
		paint(signon1,signon2);	/* Print intro 			*/
		if	( newkey )	/* Check for keypress		*/
		{
			read_cbkey ( );	/* Flush buffer			*/
			state= S_000;	/* Test for system status	*/
			dirty= 1;	/* Repaint screen		*/
		}
		break;
	case S_000:			/* Off Line; Test system status	*/
		paint(modname,mes_off);	/* Print Status			*/
		set_plcd( LCD_L2+ 9 );	/* Goto line 2			*/
		printf	("%4u",counter);/* Show some activity		*/
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (init_rtc())	/* Check for Real Time Clock	*/
			{
				state= S_R00;	/* Change state		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
			if (test_key())	/* Check for Keypad acknowledge	*/
			{
				state= S_0K0;	/* Change state		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
		}
		if	( iic_sready )	/* Check for LCD Control	*/
		{
			state= S_00L;
			break;
		}
		if	( test_cbkey())	/* Check for keypress		*/
		{
			read_cbkey ( );	/* Flush buffer			*/
			state= S_IDLE;	/* Print "About"		*/
			dirty= 1;	/* Repaint screen		*/
		}
		break;
	case S_R00:			/* RTC present			*/
		paint(modname, mes_rtc);/* Print Confirmation		*/
		set_plcd( LCD_L2+ 8 );	/* Go to line 2			*/
		if	(! show_time())	/* Print the time		*/
		{
			state= S_000;	/* Change state			*/
			dirty= 1;	/* Repaint screen		*/
			break;
		}
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (test_key())	/* Check for Keypad acknowledge	*/
			{
				state= S_RK0;	/* Change State		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
		}
		if	( iic_sready )	/* Check for LCD Control	*/
		{
			state= S_R0L;
			dirty= 1;
			break;
		}
		if	( newkey )	/* Check for keypress		*/
		{
			read_cbkey ( );	/* Flush buffer			*/
			state= S_IDLE;	/* Print "About"		*/
			dirty= 1;	/* Repaint screen		*/
			break;
		}
		break;
	case S_0K0:			/* Keypad ACK			*/
		paint(modname, mes_snd);/* Print Confirmation		*/
		if	( iic_sready )	/* Check for LCD Control	*/
		{
			state= S_0KL;	/* Change state			*/
			break;
		}
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (init_rtc())	/* Check for Real Time Clock	*/
			{
				state= S_RK0;	/* Change state		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
		}
		if	(test_cbkey())	/* Check for keypress		*/
		{
			if (proc_key())	/* Process, check for error	*/
			{
			   state= S_000;/* No acknowledge		*/
			   dirty= 1;	/* Refresh screen		*/
			}
		}
		break;
	case S_RK0:			/* RTC, Keypad response		*/
		paint(modname, mes_snd);/* Print Confirmation		*/
		if	( iic_sready )	/* Check for LCD Control	*/
		{
			state= S_RKL;
			break;
		}
		if	( test_cbkey())	/* Check for keypress		*/
		{
			if (proc_key())	/* Process, check for error	*/
			{
			   state= S_R00;/* Change state			*/
			   dirty= 1;	/* Refresh screen		*/
			}
		}
		break;
	case S_0KL:			/* BHC present, No RTC		*/
	case S_RKL:			/* Full Connection		*/
		if	( iic_sready )	/* Incoming LCD Command		*/
			proc_iic ( );	/* Process it			*/
		if	( test_cbkey())	/* Keypress			*/
		{
			if (proc_key())	/* Process, check for error	*/
			{
			   state= S_R0L;/* Change state			*/
			   dirty= 1;	/* Repaint screen		*/
			}
		}
		break;
	case S_R0L:			/* RTC, LCD Control		*/
		paint(modname, err_wr);	/* Print Confirmation		*/
		if	( iic_sready )	/* Incoming LCD Command		*/
			proc_iic ( );	/* Process it			*/
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if( test_key( ))/* Check for Keypad acknowledge	*/
			{
				state= S_RKL;	/* Change state		*/
				break;
			}
		}
		if	( test_cbkey())	/* Check for keypress		*/
		{
			read_cbkey ( );	/* Flush buffer			*/
			dirty= 1;	/* Repaint screen		*/
		}
		break;
	case S_00L:			/* LCD Control			*/
		paint(modname, err_wr);	/* Print Confirmation		*/
		if	( iic_sready )	/* Incoming LCD Command		*/
			proc_iic ( );	/* Process it			*/
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (init_rtc())	/* Check for Real Time Clock	*/
			{
				state= S_R0L;	/* Change State		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
			if (test_key())	/* Check for Keypad acknowledge	*/
			{
				state= S_0KL;	/* Change State		*/
				break;
			}
		}
		if	( test_cbkey())	/* Check for keypress		*/
		{
			read_cbkey ( );	/* Flush buffer			*/
			dirty= 1;	/* Repaint screen		*/
		}
		break;
	default:
		state= S_IDLE;
		break;
	}
	skip++;				/* Increment test skip		*/
	if	( ++counter >9999 )	/* Increment loop counter	*/
	{
		counter= 0;
	}
    }
}
