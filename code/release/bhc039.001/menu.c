/*			Copyright Q Solutions				*/
/*	File:		menu.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		User Interface Menus				*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	Extracted from main bhc.c (ui.c)		*/
/* 10:38am 07-01-1997 	Extracted from ui.c (menu.c)			*/
/*									*/

#define	M_IDLE		0		/* Menu Idle			*/
#define	M_TIME		1		/* Menu: Set Time		*/
#define	M_FLOW		2		/* Menu: Set flow		*/
#define	M_LLLR		3		/* Menu: Reset LLL		*/
#define	M_LLEV		4		/* Menu: Set Low Level		*/
#define	M_ST		5		/* Menu: Start/Stop		*/
#define M_MAX		5		/* Menu: Total items		*/

byte	m_state=	M_IDLE;		/* Menu state			*/
byte	f_new=		0;		/* New Flow Setting		*/
byte	l_new=		0;		/* New Low Level Setting	*/
byte	cursor=		0;		/* Entry Cursor position	*/
time	t_in;				/* New Time			*/

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
			t_in.h= t* 16;	/* Store tens of hours (BCD)	*/
			cursor++;
		}
		break;
	case 8:	t+= t_in.h;		/* Calculate hh 		*/
		if	( t< 0x25 )	/* Check range			*/
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
			t_in.m= t* 16;	/* Store tens of minutes	*/
			cursor++;
		}
		break;
	case 11:t+= t_in.m;		/* Calculate mm 		*/
		if	( t< 0x60 )	/* Check range			*/
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
			t_in.s= t* 16;	/* Store tens of seconds	*/
			cursor++;
		}
		break;
	case 14:t+= t_in.s;		/* Calculate ss 		*/
		if	( t< 0x60 )	/* Check range			*/
		{
			t_in.s= t;	/* Store hours 			*/
			set_rlcd(RL_L1);/* Goto Line 1			*/
			printf (s_tset);/* Print confirmation		*/
			set_time(&t_in);/* Set the RTC			*/
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
		{
			printf(s_nrng);	/* Print Error Message		*/
			return;
		}
		if	( ce_lll )	/* Low Level Lockout		*/
		{
			printf(s_nrll);	/* Print Error Message		*/
			return;
		}
		if ( ce_mode== CE_MAN )/* BHC in Manual mode		*/
		{
			printf(s_nman);	/* Print error message		*/
			return;
		}
		if ( ce_mode== CE_OFF )/* BHC in Off mode		*/
		{
			printf(s_noff);	/* Print error message		*/
			return;
		}
		printf	( s_rning );	/* Print Confirmation: Starting	*/
		ce_sig	( S_STA );	/* Signal Control Engine: Start	*/
	}
	else
	{
		if ( ce_mode== CE_MAN )/* BHC in Manual mode		*/
		{
			printf(s_nman);	/* Print error message		*/
			return;
		}
		if ( ce_mode== CE_OFF )/* BHC in Off mode		*/
		{
			printf(s_noff);	/* Print error message		*/
			return;
		}
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