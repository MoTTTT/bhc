/*			Copyright Q Solutions				*/
/*	File:		ui.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		User Interface Module				*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	Extracted from main bhc.c 			*/
/* 01:45am 09-13-1997 	Change Data Base Record structure		*/
/*									*/

#include	<iic.h>			/* I2C Bus Wrapper routines	*/
#include	<rlcdpad.h>		/* Remote LCD & keypad routines	*/
#include	<serial.h>		/* Serial Port routines		*/
#include	<adc.h>			/* Analog routines		*/

/* 			UI States					*/
#define	READY		0x02		/* UI State: Taking Commands	*/
#define DEBUG		0xFF		/* UI State: Debug LCD, Keypad	*/
#define	IDLE		0		/* State Idle			*/
#define	U_IDLE		0		/* User interface Idle		*/
/*			Platform dependant constants			*/
#define	BHC_ADR		0x68		/* Main Controller Slave Addr	*/
#define	IO_RLCD		0x01		/* io_stream value for LCD data */
#define	IO_232		0x02		/* io_stream value for RS232	*/
#define	B192		0xFD		/* Serial Port BAUD rate	*/
#define	LCD_W		0x0100		/* Wait period for remote LCD	*/
#define	MSB		0x80		/* Most significant bit mask	*/

//time	t_start;
byte	i2ccount=	0;		/* IIC input frame counter	*/
byte	out_stream=	IO_RLCD;	/* Output port			*/
byte	ui_state=	U_IDLE;		/* User Interface state		*/
bit	dirty=		1;		/* Lcd refresh flag		*/
bit	lcd_ack;			/* Lcd write acknowledge flag	*/

#include	"mesg.c"		/* User Interface Messages	*/
#include	"menu.c"		/* User Interface Menu		*/
#include	"info.c"		/* User Interface Info		*/
#include	"db.c" 			/* Database Struct & Functions	*/

void	ui_slog		( void )	/* Log status to serial Port	*/
{
	out_stream=	IO_232;		/* Send to serial Port		*/
	if	( show_time( ) )	/* Print time			*/
		printf	( " " );	/* Print a delimiter		*/
	else	printf	( " NO RTC! " );/* Print RTC error		*/
	db_print( db_idx- 1 );		/* Print last database record	*/
	out_stream=	IO_RLCD;	/* Send to Remote LCD as default*/
}

void	paint	( char *l1, char *l2 )	/* Print two lines to LCD	*/
{
	if	( dirty& lcd_ack )
	{
		paint_rlcd ( l1, l2 );	/* Paint to remote LCD		*/
		dirty=	0;
	}
}

void	init_i2clcd	( void )	/* Initialise I2C Bus LCD Module*/
{
uint	wait= LCD_W;			/* Remote process wait counter	*/
	while	( --wait );		/* Wait for remote unit	to sync	*/
	if ( init_rlcd ( ) )		/* Initialise LcdPad module	*/
	init_rlcd ( );			/* Initialise LcdPad module	*/
	{
		lcd_ack= 1;		/* Remote display acknowledge	*/
		dirty= 1;		/* Refresh LCD			*/
		paint	( rso1, rso2 );	/* Remote Sign On splash screen	*/
		ui_state= IDLE;		/* Set User Interface to IDLE	*/
	}
}

char	putchar		( char out )	/* Putchar Output Wrapper	*/
{
	if	( out_stream== IO_RLCD )/* Destination: Remote LCD	*/
		return rputchar	( out );/* Send data to remote LCD	*/
	if	( out_stream== IO_232 )	/* Destination: Serial Port	*/
		return sputchar	( out );/* Send character to serial port*/
}

void	proc_com	( char in )	/* Process commands from keypad	*/
{
	switch	( in )
	{
	case 0x7e: if ( m_state< M_MAX )/* -> : Cycle Menu Foreward	*/
			m_state++;	/* Select next menu item	*/
		else	m_state= 1;
		if	( !ce_lll& (m_state==M_LLLR) )
			m_state++;	/* Invalid Menu: No LLL		*/
		m_reset	( );		/* Reset Menu			*/
		put_menu( );		/* Print Menu Item		*/
		break;
	case 0x7f: if ( m_state> 1 )	/* <- : Cycle Menu Backward	*/
			m_state--;	/* Select last menu item	*/
		else	m_state= M_MAX;
		if	( !ce_lll& (m_state==M_LLLR) )
			m_state--;	/* Invalid Menu: No LLL		*/
		m_reset	( );		/* Reset Menu			*/
		put_menu( );		/* Print Menu Item		*/
		break;
	case 'D': if ( i_state> 1 )	/* Down Arrow: Cycle Info Back	*/
			i_state--;	/* Select last info item	*/
		else	i_state= I_MAX;
		put_info( );		/* Print Info Item		*/
		break;
	case 'U': if ( i_state< I_MAX )	/* Up Arrow: Cycle Info		*/
			i_state++;	/* Select next info item	*/
		else	i_state= 1;
		put_info( );		/* Print Info Item		*/
		break;
	case 'R': proc_run ( );		/* Run/Stop Pressed		*/
		m_reset	( );		/* Reset Menu			*/
		m_state= M_IDLE;	/* Menu back to Idle		*/
		break;
	case 'O': break;		/* Not used			*/
	case 'C': break;		/* Not used			*/
	case '*': break;		/* Not used			*/
	case '.': break;		/* Not used			*/
	default : 			/* Print Instructions		*/
		if ( m_state== M_IDLE )	/* First find the arrow keys	*/
		{
			set_rlcd(RL_L1);/* Goto Line 1			*/
			printf(s_menu);	/* Print menu navigation help	*/
			break;
		}
		if (!cursor& in!='E')	/* Enter to select Menu Item	*/
		{
			set_rlcd(RL_L1);/* Goto Line 1			*/
			printf(s_enter);/* Print menu selection help	*/
			m_state= M_IDLE;/* Set menu back to idle	*/
			break;
		}
		proc_in ( in );		/* Process 0..9, Enter		*/
		break;
	}
}

void	proc_rkey	( )		/* Process Remote Keypad	*/
{
char	in;
	i2ccount++;			/* Increment keypress counter	*/
	in=	rgetkey ( );		/* Read keypad			*/
	if	( in== PING )		/* Remote unit trying to sync	*/
	{
		init_i2clcd	( );	/* Init I2C Bus LCD		*/
		m_state=	M_IDLE;	/* Reset Menu State		*/
		i_state=	I_STAT;	/* Reset Info State		*/
		return;			/* Don't process it		*/
	}
	switch	( ui_state )
	{
	case IDLE:			/* Sign on printed on LCD	*/
		dirty=	1;		/* Repaint LCD			*/
		paint (s_menu, s_stat);	/* Print LCD screen		*/
		ui_state= READY;	/* Change State			*/
		break;
	case READY:			/* Processing Commands		*/
		proc_com ( in );	/* Process			*/
		break;
	default:ui_state= IDLE;		/* Unknown State: Reset		*/
		break;
	}
}
