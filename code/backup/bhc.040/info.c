/*			Copyright Q Solutions				*/
/*	File:		info.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		User Interface: Info Display			*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	Extracted from main bhc.c (ui.c)		*/
/* 10:48am 07-01-1997	Extracted from ui.c (info.c)			*/
/*									*/

#define	I_FLOW		1		/* Info: Flow			*/
#define	I_PRES		2		/* Info: Pressure		*/
#define	I_WAT		3		/* Info: Water Level		*/
#define	I_TIME		4		/* Info: Time			*/
#define	I_STAT		5		/* Info: Status			*/
#define	I_MAX		5		/* Info: Total Items		*/

byte	i_state=	I_STAT;		/* Info display state		*/
byte	old_flow=	0;		/* Previous flow reading	*/
bit	ui_refr=	0;		/* Info refresh flag		*/
time	t_start;			/* System boot-up time		*/
time	t_now;				/* Current time			*/

void	put_info	( void )	/* Print info string		*/
{
	set_rlcd( RL_L2 );		/* Goto Line 1			*/
	switch	( i_state )
	{
	default:i_state= I_STAT;	/* Info: Default State		*/
	case I_STAT: printf( s_stat );	/* Info: Status			*/
		break;
	case I_FLOW: printf( s_flow );	/* Info: Flow			*/
		old_flow= 0;
		break;
	case I_PRES: printf( s_pres );	/* Info: Pressure		*/
		break;
	case I_WAT: printf( s_wl );	/* Info: Water Level		*/
		break;
	case I_TIME: printf( s_time );	/* Info: Time			*/
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
	if	( ce_mode== CE_MAN )	/* Manual Mode			*/
		putchar	( 'M' );	/* Print "M"			*/
	else if	( ce_mode== CE_OFF )	/* Off Mode			*/
		putchar	( 'O' );	/* Print "O"			*/
	else	putchar	( 'A' );	/* Auto Mode; Print "A"		*/
	put_flag( ce_run, 'R' );	/* Running			*/
	put_flag( a_stat& A_PH, 'P' );	/* Pressure High		*/
	put_flag( a_stat& A_WL, 'W' );	/* Water Low			*/
	put_flag( ce_lll, 'L' );	/* Low Level Lockout		*/
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
	case V_PO:putchar ( 'o' );	/* Valve Pulsing (Open)		*/
		break;
	case V_PC:putchar ( 'c' );	/* Valve Pulsing (Close)	*/
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
//	if(!(lcd_ack&(ui_state==READY)))/* Abort if LCD not OK		*/
//		return;
	if	( ui_state!=READY)	/* Abort if LCD not READY	*/
		return;
	if	( cursor )		/* Cursor in use		*/
		set_rlcd ( RL_COF );	/* Switch Blink Off		*/
	switch	( i_state )
	{
	case I_STAT:			/* Info: Status			*/
		put_status( );		/* Print the status line	*/
		break;
	case I_FLOW:			/* Info: Flow			*/
		if (flow!= old_flow)
		{
			old_flow= flow;	/* Save new flow		*/
			set_rlcd(RL_L2);/* Cursor to out field		*/
			printf("%02bu l/s",flow);/* Print the new flow	*/
			ce_cos=	1;	/* Set Change Of State flag	*/
		}
		else
		{
			set_rlcd(RL_L2+7);/* Set cursor to output field	*/
			printf("%07lu l",f_tot);/* Print the total flow	*/
		}
		break;
	case I_PRES:			/* Info: Pressure		*/
		calc_press( );		/* Refresh Pressure Variable	*/
		set_rlcd( RL_L2+ 10 );	/* Set cursor to output field	*/
		if	( !pressure )	/* Sensor Error			*/
			printf("Error!");/* Show error message		*/
		else	printf("%01.1fBar", (float) pressure/ 10);
		break;			 /* Print pressure		*/
	case I_WAT:			/* Info: Water Level		*/
		calc_level( );		/* Refresh Water Level Variable	*/
		set_rlcd( RL_L2+ 13 );	/* Set cursor to output field	*/
		if	( !water_level )/* Sensor Error			*/
			printf ("Err!");/* Show Error message		*/
		else	printf ("%02.0fm", (float) water_level/ 10);
		break;			/* Print level			*/
	case I_TIME:			/* Info: Time			*/
		set_rlcd( RL_L2+ 7 );	/* Set cursor to output field	*/
		show_time( );		/* Print the time		*/
		break;
	default:i_state= I_STAT;	/* Illegal state, set to Idle	*/
	}
	if	( cursor )		/* Cursor in use		*/
	{
		set_rlcd (RL_L1+cursor);/* Restore it			*/
		set_rlcd ( RL_CON );	/* Switch Blink back on		*/
	}
}
