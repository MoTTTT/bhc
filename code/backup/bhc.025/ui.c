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

/* 			UI States					*/
#define	READY		0x02		/* UI State: Taking Commands	*/
#define DEBUG		0xFF		/* UI State: Debug LCD, Keypad	*/
#define	IDLE		0		/* State Idle			*/
#define	U_IDLE		0		/* User interface Idle		*/
#define	M_IDLE		0		/* Menu Idle			*/
#define	M_TIME		1		/* Menu: Set Time		*/
#define	M_FLOW		2		/* Menu: Set flow		*/
#define	M_LLLR		3		/* Menu: Reset LLL		*/
#define	M_LLEV		4		/* Menu: Set Low Level		*/
#define	M_ST		5		/* Menu: Start/Stop		*/
#define	M_AU		6		/* Menu: Manual/Auto		*/
#define M_MAX		6		/* Menu: Total items		*/
#define	I_IDLE		0		/* Info: Idle			*/
#define	I_FLOW		1		/* Info: Flow			*/
#define	I_PRES		2		/* Info: Pressure		*/
#define	I_WAT		3		/* Info: Water Level		*/
#define	I_TIME		4		/* Info: Time			*/
#define	I_STAT		5		/* Info: Status			*/
#define	I_MAX		5		/* Info: Total Items		*/
/*			Refresh and Sign on strings			*/
const char *clear=   "                ";/* Blank Line			*/
const char *rso1=    "Bore Hole Master";/* Line 1 of remote sign on 	*/
const char *rso2=    "   Controller   ";/* Line 2 of remote sign on	*/
/*			Menu States and strings				*/
const char *s_menu=  "\x7e and \x7f for Menu";	/* Menu: Prompt		*/
const char *s_enter= "ENTER to Select ";/* Mene: Menu Help		*/
const char *s_stime= "    Set Time    ";/* Menu: Set Time		*/
const char *s_ptime= " Time: hh:mm:ss ";/* Time framework		*/
const char *s_tset=  "    Time Set    ";/* Menu: Set Time		*/
const char *s_sflow= "    Set Flow    ";/* Menu: Set Flow		*/
const char *s_pflow= " Flow:     l/s  ";/* Menu: Set Flow Prompt	*/
const char *s_fset=  "    Flow Set    ";/* Menu: Flow Set		*/
const char *s_rlll=  "   Reset LLL    ";/* Menu: Reset Low Level Lockout*/
const char *s_rstng= " Resetting LLL  ";/* Menu: Reset Low Level Lockout*/
const char *s_nrst=  " Level Still Low";/* Menu: Reset Low Level Lockout*/
const char *s_sll=   " Set Low Level  ";/* Menu: Set Low Level 		*/
const char *s_pll=   " Low Level:    m";/* Water Level framework	*/
const char *s_lset=  " Low Level Set  ";/* Menu: Low Level Set		*/
const char *s_stop=  " Stop Borehole  ";/* Menu: Start/Stop Prompt:Stop	*/
const char *s_stpng= "    Stopping    ";/* Status: Stop pressed		*/
const char *s_start= " Start Borehole ";/* Menu: Start/Stop Prompt:Start*/
const char *s_rning= "    Starting    ";/* Status: Start pressed	*/
const char *s_nrng=  "System Shut Down";/* Status: Start pressed, errors*/
const char *s_man=   "     Manual     ";/* Menu: Man/Auto Prompt: Man	*/
const char *s_sman=  " Set to Manual  ";/* Menu: Man/Auto Prompt: Man	*/
const char *s_auto=  "   Automatic    ";/* Menu: Man/Auto Prompt: Auto	*/
const char *s_saut=  "Set to Automatic";/* Menu: Man/Auto Prompt: Auto	*/
const char *s_oor=   "  Out Of Range  ";/* Menu: Input out of range	*/
/*			Info Strings					*/
const char *s_flow=  "Flow:           ";/* Flow framework		*/
const char *s_pres=  "Pressure:    Bar";/* Pressure framework		*/
const char *s_wl=    "Water Level:   m";/* Water Level framework	*/
const char *s_time=  "Time:           ";/* Time framework		*/
const char *s_stat=  "Status:         ";/* Status framework		*/

#define	BHC_ADR		0x68		/* Main Controller Slave Addr	*/
#define	IO_RLCD		0x01		/* io_stream value for LCD data */
#define	IO_232		0x02		/* io_stream value for RS232	*/
#define	LCD_W		0x1000		/* Wait for remote LCD		*/

typedef struct	{ byte h; byte m; byte s; } time; /* Time Structure	*/

uint	counter;			/* Routine cycle counter	*/
byte	i2ccount=	0;		/* IIC input frame counter	*/
byte	out_stream=	IO_RLCD;	/* Output port			*/
byte	ui_state=	U_IDLE;		/* User Interface state		*/
byte	m_state=	M_IDLE;		/* Menu state			*/
byte	i_state=	I_STAT;		/* Info display state		*/
byte	f_new=		0;		/* New Flow Setting		*/
byte	l_new=		0;		/* New Low Level Setting	*/
byte	cursor=		0;		/* Entry Cursor position	*/
time	t_in;				/* New Time			*/
bit	dirty=		1;		/* Lcd refresh flag		*/
bit	ui_refr=	0;		/* Info refresh flag		*/
bit	lcd_ack;			/* Lcd write acknowledge flag	*/

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
	{
		lcd_ack= 1;		/* Remote display acknowledge	*/
		dirty= 1;		/* Refresh LCD			*/
		paint	( rso1, rso2 );	/* Remote Sign On splash screen	*/
		ui_state= IDLE;		/* Set User Interface to IDLE	*/
	}
}

char	putchar		( char out )	/* Putchar Output Wrapper	*/
{
	return rputchar	( out );	/* Data to remote LCD		*/
}

void	put_info	( void )	/* Print info string		*/
{
	set_rlcd( RL_L2 );		/* Goto Line 1			*/
	switch	( i_state )
	{
	default:i_state= I_IDLE;	/* Info: Idle			*/
	case I_IDLE:
	case I_STAT: printf( s_stat );	/* Info: Status			*/
		break;
	case I_FLOW: printf( s_flow );	/* Info: Flow			*/
		break;
	case I_PRES: printf( s_pres );	/* Info: Pressure		*/
		break;
	case I_WAT: printf( s_wl );	/* Info: Water Level		*/
		break;
	case I_TIME: printf( s_time );	/* Info: Time			*/
		break;
	}
}

void	put_menu	( void )	/* Print a menu item		*/
{
	set_rlcd( RL_L1 );		/* Goto Line 2			*/
	switch	( m_state )
	{
	default:m_state= M_IDLE;
	case M_IDLE: printf( s_menu );	/* Menu Idle			*/
		break;
	case M_TIME: printf( s_stime );	/* Menu: Set Time		*/
		break;
	case M_FLOW: printf( s_sflow );	/* Menu: Set flow		*/
		break;
	case M_LLLR: printf( s_rlll );	/* Menu: Reset LLL		*/
		break;
	case M_LLEV: printf( s_sll );	/* Menu: Set Level 		*/
		break;
	case M_ST: if	( !ce_run )	/* Menu: Start/Stop		*/
			printf ( s_start );
		else	printf ( s_stop );
		break;
	case M_AU: if	( !bhc_man )	/* Menu: Manual/Auto		*/
		{
			printf	( s_man );
		}
		else
		{
			printf	( s_auto );
		}
		break;
	}
}

void	put_flag	( bit on, char out )
{
	if	( on )			/* Flag active			*/
		putchar	( out );	/* Print the flag		*/
	else	putchar	( ' ' );
}

void	put_status	( void )	/* Print the status flags	*/
{
	set_rlcd( RL_L2+ 8 );		/* Set cursor to output field	*/
	put_flag( bhc_man, 'M' );	/* Manual			*/
	put_flag( ce_run, 'R' );	/* Running			*/
	put_flag( a_stat& A_PH, 'P' );	/* Pressure High		*/
	put_flag( a_stat& A_WL, 'W' );	/* Water Low			*/
	put_flag( ce_lll, 'L' );	/* Low Level Lockout		*/
//	printf	( "%02bX", a_stat );	/* Alarm Debug			*/
	put_flag( a_stat& A_PF, 'F' );	/* Phase Fail			*/
	put_flag( a_stat& A_OV, 'O' );	/* Overlaod			*/
	switch	( v_stat )		/* Print Valve State Indicator	*/
	{
	case V_I: putchar ( 'I' );	/* Valve Idle			*/
		break;
	case V_O: putchar ( 'O' );	/* Valve Opening		*/
		break;
	case V_C: putchar ( 'C' );	/* Valve Closing		*/
		break;
	case V_P: putchar ( 'P' );	/* Valve Pulsing (Open or Close)*/
		break;
	case V_S: putchar ( 'S' );	/* Valve set: Flow within limits*/
		break;
	default:  putchar ( ' ' );	/* Unknown state		*/
		break;
	}
}

void	ui_update	( )		/* Refresh info readout		*/
{
	ui_flag= 0;			/* Reset refresh flag		*/
	if(!(lcd_ack&(ui_state==READY)))/* Abort if LCD not OK		*/
		return;
	switch	( i_state )
	{
	case I_STAT:			/* Info: Status			*/
		put_status( );		/* Print the status line	*/
		break;
	case I_FLOW:			/* Info: Flow			*/
		set_rlcd( RL_L2+ 5 );	/* Set cursor to output field	*/
		printf	( "%02bu %8lu  ", f_ave, f_tot );
		break;
	case I_PRES:			/* Info: Pressure		*/
		calc_press( );		/* Refresh Pressure Variable	*/
		set_rlcd( RL_L2+ 10 );	/* Set cursor to output field	*/
//		printf("%01.1f", pressure); /* Print pressure		*/
		printf("%03bu", p_ave); /* Print ADC pressure		*/
		break;
	case I_WAT:			/* Info: Water Level		*/
		calc_level( );		/* Refresh Water Level Variable	*/
		set_rlcd( RL_L2+ 12 );	/* Set cursor to output field	*/
		printf("%02.0f", water_level);/* Print water level	*/
		break;
	case I_TIME:			/* Info: Time			*/
		set_rlcd( RL_L2+ 7 );	/* Set cursor to output field	*/
		show_time( );		/* Print the time		*/
		break;
	default:i_state= I_IDLE;	/* Illegal state, set to Idle	*/
	}
	if	( cursor )		/* Cursor in use		*/
	{
		set_rlcd (RL_L1+cursor);/* Restore it			*/
	}
}

void	m_reset		( void )	/* Reset Menu			*/
{
	set_rlcd( RL_COF );		/* Reset LCD Cursor		*/
	cursor=	0;			/* Reset input cursor		*/
}

void	set_flow	( void )	/* Set Flow			*/
{
	set_rlcd( RL_L1 );		/* Goto Line 1			*/
	m_reset	( );			/* Reset menu			*/
	if	( ce_sflow( f_new ) )	/* New Flow in range		*/
		printf	( s_fset );	/* Acknowledge new flow		*/
	else	printf	( s_oor );	/* Print message: out of range	*/
}

void	set_llev	( void )	/* Set Low Level		*/
{
	set_rlcd( RL_L1 );		/* Goto Line 1			*/
	m_reset	( );			/* Reset menu			*/
	if	( ce_sllev( l_new ) )	/* New Level in range		*/
		printf	( s_lset );	/* Acknowledge new level	*/
	else	printf	( s_oor );	/* Print message: out of range	*/
}

void	p_itime		( char in )	/* Process Input: Time		*/
{
byte	t;
	t= in- '0';			/* Convert input to decimal	*/
	switch	( cursor )
	{
	case 0:	printf(s_ptime);	/* Show the prompt		*/
		cursor=	7;		/* Set the cursor		*/
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		set_rlcd( RL_CON );	/* Set Blinking Cursor		*/
		break;
	case 7:	if	( t< 3 )	/* Check range			*/
		{
			set_rlcd(RL_L1+cursor);	/* Goto input field	*/
			putchar	( in );	/* Print input			*/
			t_in.h= t* 10;	/* Store tens of hours		*/
			cursor++;
		}
		break;
	case 8:	t+= t_in.h;		/* Calculate hh 		*/
		if	( t< 25 )	/* Check range			*/
		{
			set_rlcd(RL_L1+cursor);	/* Goto input field	*/
			putchar	( in );	/* Print input			*/
			t_in.h= t;	/* Store hours 			*/
			cursor+= 2;	/* Move on to minutes		*/
		}
		break;
	case 10:if	( t< 6 )	/* Check range			*/
		{
			set_rlcd(RL_L1+cursor);	/* Goto input field	*/
			putchar	( in );	/* Print input			*/
			t_in.m= t* 10;	/* Store tens of minutes	*/
			cursor++;
		}
		break;
	case 11:t+= t_in.m;		/* Calculate mm 		*/
		if	( t< 60 )	/* Check range			*/
		{
			set_rlcd(RL_L1+cursor);	/* Goto input field	*/
			putchar	( in );	/* Print input			*/
			t_in.m= t;	/* Store minutes		*/
			cursor+= 2;	/* Move on to seconds		*/
		}
		break;
	case 13:if	( t< 6 )	/* Check range			*/
		{
			set_rlcd(RL_L1+cursor);	/* Goto input field	*/
			putchar	( in );	/* Print input			*/
			t_in.s= t* 10;	/* Store tens of seconds	*/
			cursor++;
		}
		break;
	case 14:t+= t_in.s;		/* Calculate ss 		*/
		if	( t< 60 )	/* Check range			*/
		{
			set_rlcd(RL_L1);/* Goto Line 1			*/
			printf (s_tset);/* Print input			*/
			t_in.s= t;	/* Store hours 			*/
			m_reset	( );	/* Reset menu			*/
		}
		break;
	default:m_reset	( );		/* Reset menu			*/
	}
}

void	p_iflow		( char in )	/* Process Input: Flow		*/
{
	switch	( cursor )
	{
	case 0:	cursor=	8;		/* Set the cursor position	*/
		printf	( s_pflow );	/* Show the prompt		*/
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		printf ("%02bu", f_set);/* Print current Setting	*/
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		set_rlcd( RL_CON );	/* Set Blinking Cursor		*/
		break;			/* Finished			*/
	case 8:				/* Default value printed	*/
		if	( in== 'E' )	/* On enter, accept default	*/
		{
			f_new= f_set;	
			set_flow( );	/* Set Flow			*/
			break;
		}
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		putchar	( in );		/* Print input			*/
		f_new=	in- '0';	/* Store input to new flow var	*/
		cursor++;		/* Advance cursor		*/
		break;
	case 9:				/* First value accepted		*/
		if	( in== 'E' )	/* On enter, accept last input	*/
		{
			set_flow( );	/* Set flow			*/
			break;
		}
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		putchar	( in );		/* Print input			*/
		f_new*=	10;		/* Last digit was tens		*/
		f_new+=	in- '0';	/* Add input to new flow var	*/
		set_flow( );		/* Set flow			*/
		break;
	default:m_reset	( );		/* Reset menu			*/
	}
}

void	p_ilevel	( char in )	/* Process Input: Level		*/
{
	switch	( cursor )
	{
	case 0:	cursor=	11;		/* Set the cursor position	*/
		printf	( s_pll );	/* Show the prompt		*/
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		printf ("%02bu", l_set);/* Print current Setting	*/
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		set_rlcd( RL_CON );	/* Set Blinking Cursor		*/
		break;			/* Finished			*/
	case 11:			/* Default value printed	*/
		if	( in== 'E' )	/* On enter, accept default	*/
		{
			l_new= l_set;	
			set_llev( );	/* Set Low Level		*/
			break;
		}
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		putchar	( in );		/* Print input			*/
		l_new=	in- '0';	/* Store input to new flow var	*/
		cursor++;		/* Advance cursor		*/
		break;
	case 12:			/* First value accepted		*/
		if	( in== 'E' )	/* On enter, accept last input	*/
		{
			set_llev( );	/* Set Low Level		*/
			break;
		}
		set_rlcd(RL_L1+cursor);	/* Goto input field		*/
		putchar	( in );		/* Print input			*/
		l_new*=	10;		/* Last digit was tens		*/
		l_new+=	in- '0';	/* Add input to new flow var	*/
		set_llev( );		/* Set flow			*/
		break;
	default:m_reset	( );		/* Reset menu			*/
	}
}

void	proc_run	( void )	/* Process Run Command		*/
{
	set_rlcd( RL_L1 );		/* Goto Line 1			*/
	if	( !ce_run )
	{
		if	( a_stat& A_SM )/* Borehole Shut Down		*/
			printf(s_nrng);	/* Print Error Message		*/
		else			/* Ready to Run			*/
		{
			if ( ce_lll )	/* Low Level Lockout		*/
				ce_lll=0;/* Reset LLL			*/
			printf(s_rning);/* Print Confirmation: Starting	*/
			ce_sig (S_STA);	/* Signal Control Engine: Start	*/
		}
	}
	else
	{
		printf(s_stpng);	/* Print Confirmation: Stopping	*/
		ce_sig (S_STO);		/* Signal Control Engine: Stop	*/
	}
}

void	proc_in		( char in )	/* Process 0..9 keys		*/
{
	set_rlcd( RL_L1 );		/* Goto Line 1			*/
	switch	( m_state )
	{
		break;
	case M_ST: proc_run ( );	/* Menu: Start/Stop		*/
		break;
	case M_AU:			/* Menu: Manual/Auto		*/
		bhc_man	= bhc_man? 0: 1;/* Toggle flag			*/
		ce_sig	( S_DEB );	/* Signal Control Engine: Debug	*/
		if	( bhc_man )
			printf ( s_sman );
		else	printf ( s_saut );
		break;
	case M_LLLR:			/* Reset Low Level Lockout	*/
		set_rlcd(RL_L1);	/* Goto Line 2			*/
		if ( a_stat& A_LL )	/* Check Alarm Status		*/
			printf(s_nrst);	/* Level Still Low		*/
		else
		{
			printf(s_rstng);/* Print confirmation: Reset	*/
			ce_lll= 0;	/* Reset flag			*/
			ce_sig (S_RLL);	/* Signal Control Engine	*/
		}
		break;
	case M_TIME: p_itime ( in );	/* Menu: Set Time		*/
		break;
	case M_FLOW: p_iflow ( in );	/* Menu: Set flow		*/
		break;
	case M_LLEV: p_ilevel ( in );	/* Menu: Set Low Level		*/
		break;
	default: m_state= M_IDLE;
		break;
	}
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