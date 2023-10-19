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
#include	<iic.h>			/* IIC Bus routines		*/
#include	<wdog.h>		/* Watchdog refresh		*/

/* Local defines							*/
#define		S_IDLE	0x10		/* Idle State			*/
#define		S_000	0x00		/* Testing for peripherals	*/
#define		S_0K0	0x02		/* Keypad ACK			*/
#define		S_00L	0x04		/* LCD Control			*/
#define		S_0KL	0x06		/* BHC present, No RTC		*/
#define		S_ERR	0x08		/* Keypad not acknowledged	*/
#define		P_WAIT	5000		/* Wait period for peripherals	*/
#define		PING	100		/* Send dummy keycode		*/
#define		TRY_MAX	5		/* Number of Keypad retries	*/
const char *signon1= " I2C Bus Remote ";/* Line 1 of sign on screen	*/
const char *signon2= " LcdPad Module  ";/* Line 2 of signon screen	*/
const char *modname= "  LcdPad Module ";/* Header			*/
const char *mes_off= "Off Line        ";/* Off line message		*/
const char *mes_con= "  Keypad Online ";/* Print Confirmation		*/
const char *err1=    "   Write Error  ";/* Print Status			*/
const char *prompt=  "  Press any Key ";/* Prompt			*/
const char *clr=     "                ";/* Blank line			*/

const char key_map[20]=	{ 'C', 'E', '.', '0', '*',	/* Keycode to	*/
			  'D', 'U', '9', '8', '7',	/* character	*/
			  126, 127, '6', '5', '4',	/* mapping	*/
			  'O', 'R', '3', '2', '1' };

bit		dirty= 1;		/* Sreen repaint flag		*/
char idata	iic_in[2];		/* IIC Bus input buffer		*/
char idata	iic_out[2];		/* IIC Bus output buffer	*/

void	initialise	( )		/* Initialise Hardware		*/
{
uint	w= P_WAIT;
	EA= 		1;		/* ENABLE GLOBAL INT		*/
	iic_init	( LP_ADR );	/* Initialise IIC Bus		*/
	iic_sset	( iic_in, 2 );	/* Set up slave input buffer	*/
	init_cbkey	( );		/* Initialise cross bar switch	*/
	while		( w-- )		/* Wait for periphs to stabilise*/
		wdog	( 100 );	/* Refresh the Watchdog timer	*/
	init_plcd	( );		/* Initialise LCD on port SCREEN*/
}

char	putchar		( char out )
{
	return	( pputchar ( out ) );
}

void	proc_iic	( void )	/* Process I2C In (LCD data)	*/
{
char	in= iic_in[1];
	switch	( iic_in[0] )		/* Command			*/
	{
	case 0:	paint_plcd (clr, clr);	/* Clear LCD			*/
		set_plcd ( LCD_L1 );	/* Set cursor to line 1 	*/
		break;
	case 1:	set_plcd( in );		/* Write LCD control		*/
		break;
	case 2: pputchar( in );		/* Write LCD data		*/
		break;
	case 3: break;			/* Read LCD control		*/
	case 4: break;			/* Read LCD data		*/
	case PING: break;		/* ACKnowledged by hardware	*/
	default:
		break;
	}
	iic_sready=	0;		/* Clear slave reception flag	*/
}

bit	proc_key	( )		/* Read and process keypad	*/
{
byte	retry=	TRY_MAX;		/* Set retry timout		*/
	iic_out[0]= 	read_cbkey( );	/* Read key from keypad		*/
	while	( retry-- )
	{
		iic_mstart (BHC_ADR,1,iic_out);	/* Send key to BHC	*/
		if	( iic_wait( ) )	/* Was transmission acknowledged*/
			return	( 0 );	/* Return Success		*/
	}
	return	( 1 );			/* No reply			*/
}

void	paint	( char *l1, char *l2 )	/* Draw Screen			*/
{
	if	( dirty )		/* Only if necessary		*/
	{
		paint_plcd( l1, l2 );	/* Print strings		*/
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
byte	state= S_IDLE;			/* LcdPad state variable	*/
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
		printf("%4u",counter>>8);/* Show some activity		*/
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (test_key())	/* Check for Keypad acknowledge	*/
			{
				state= S_0K0;	/* Change state		*/
				dirty= 1;	/* Repaint screen	*/
				break;
			}
		}
		if	( test_cbkey())	/* Check for keypress		*/
			while	( 1 );	/* Reset Unit 			*/
		break;
	case S_0K0:			/* Keypad ACK			*/
		if	( iic_sready )	/* Check for LCD Control	*/
		{
			state= S_0KL;	/* Change state			*/
			break;
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
	case S_0KL:			/* Key Ack + LCD Control	*/
		if	( iic_sready )	/* Incoming LCD Command		*/
			proc_iic ( );	/* Process it			*/
		if	( test_cbkey())	/* Keypress			*/
		{
			if (proc_key())	/* Process, check for error	*/
			{
			   state= S_ERR;/* Change state			*/
			   dirty= 1;	/* Repaint screen		*/
			}
		}
		break;
	case S_ERR:			/* LCD Control			*/
		paint	( err1,prompt );/* Print Status			*/
		if	( !skip )	/* Only test every 256 cycles	*/
		{
			if (test_key())	/* Check for Keypad acknowledge	*/
			{
				state= S_0KL;	/* Change State		*/
				break;
			}
		}
		if	( test_cbkey())	/* Check for keypress		*/
			while	( 1 );	/* Stall unit : Reset I2C bus	*/
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