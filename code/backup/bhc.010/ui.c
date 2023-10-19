/*			Copyright Q Solutions				*/
/*	File:		ui.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		User Interface Module				*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	Extracted from main bhc.c 			*/
/*									*/

#include	<iic.h>			/* I2C Bus Wrapper routines	*/
#include	<rlcdpad.h>		/* Remote LCD & keypad routines	*/
#define	BHC_ADR		0x68		/* Main Controller Slave Addr	*/
#define	IO_RLCD		0x01		/* io_stream value for LCD data */
#define	IO_232		0x02		/* io_stream value for RS232	*/
#define	LCD_W		0x0300		/* Wait for remote LCD		*/
#define	IDLE		0x00		/* UI State: Waiting for any key*/
#define	READY		0x02		/* UI State: Taking Commands	*/
#define DEBUG		0xFF		/* UI State: Debug LCD, Keypad	*/
const char *rso1=    "Bore Hole Master";/* Line 1 of remote sign on 	*/
const char *rso2=    "   Controller   ";/* Line 2 of remote sign on	*/
const char *debug=   "LCD, Keypad Test";/* Debug Heading		*/
const char *clear=   "                ";/* Blank Line			*/
const char *status=  "Status :        ";/* Status prompt		*/
const char *S_S=	     "Stopped ";/* Status: Stopped		*/
const char *S_R=	     "Running ";/* Status: Running		*/
const char *prompt=  "Command:        ";/* Command prompt		*/
const char *C_R=	     "Run     ";/* Command: Run			*/
const char *C_S=	     "Stop    ";/* Command: Stop		*/

/*			Aplication Compile Time Parameters		*/
xuint	counter;			/* Routine cycle counter	*/
xbyte	i2ccount=	0;		/* IIC input frame counter	*/
xbyte	out_stream=	IO_RLCD;	/* Output port			*/
xbyte	ui_state=	IDLE;		/* User Interface state		*/
bit	dirty=		1;		/* Lcd refresh flag		*/
bit	lcd_ack;			/* Lcd write acknowledge flag	*/

void	paint	( char *l1, char *l2 )	/* Print two lines to LCD	*/
{
	if	( dirty& lcd_ack )
	{
		paint_rlcd ( l1, l2 );	/* Paint to remote LCD		*/
		dirty=	0;
	}
}

void	init_i2clcd	( )
{
uint	wait= LCD_W;			/* Remote process wait counter	*/
	while	( --wait );		/* Wait for remote unit	to proc	*/
	if ( init_rlcd ( ) )		/* Initialise LcdPad module	*/
	{
		lcd_ack= 1;		/* Remote display acknowledge	*/
		dirty= 1;		/* Refresh LCD			*/
		paint	( rso1, rso2 );	/* Remote Sign On splash screen	*/
		ui_state= IDLE;		/* Set User Interface to IDLE	*/
	}
}

char	putchar		( char out )
{
	return rputchar	( out );	/* Data to remote LCD		*/
}

void	proc_rkey	( )		/* Process Remote Keypad	*/
{
char	in;
	i2ccount++;			/* Increment keypress counter	*/
	in=	rgetkey ( );		/* Read keypad			*/
	if	( in== PING )		/* Remote unit trying to sync	*/
	{
		init_i2clcd	( );	/* Init I2C Bus LCD		*/
		return;			/* Dont process it		*/
	}
	switch	( ui_state )
	{
	case IDLE:			/* Sign on printed on LCD	*/
		dirty=	1;		/* Repaint LCD			*/
		paint (status, prompt);	/* Print LCD screen		*/
		ui_state= READY;	/* Change State			*/
		break;
	case READY:			/* Processing Commands		*/
		proc_com ( in );	/* Process			*/
		break;
	case DEBUG:			/* Keypad, LCD Test		*/
		paint ( debug, clear );	/* Repaint LCD			*/
		set_rlcd( RL_L2 );	/* Set cursor to line 2		*/
		printf("  #%3bu  Key: %c  ",	/* Print keycount, key	*/
			 i2ccount, in );
		break;
	default:			/* Unknown state		*/
		ui_state= IDLE;		/* Reset state			*/
		break;
	}
}

void	proc_status	( )		/* Refresh status readout	*/
{
	if (lcd_ack& (ui_state== READY))/* Only if LCD Present		*/
	{
		set_rlcd( RL_L1+ 8 );	/* Set cursor to status field	*/
		if	( bhc_run )	/* If "Running"			*/
			printf	( S_R );/* Print so			*/
		else	printf	( S_S );/* Print stopped		*/
	}
}