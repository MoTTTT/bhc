/*			Copyright Q Solutions				*/
/*	File:		scada.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		SCADA Interface Module				*/
/*									*/
/*			History						*/
/* 00:24 01/12/1997	Written from scratch				*/
/*									*/

#include	<modbus.h>		/* MODBUS protocol handler	*/

#define	W_H	0			/* Tag Map Write: Hours		*/
#define	W_M	2			/* Tag Map Write: Minutes	*/
#define	W_S	4			/* Tag Map Write: Seconds	*/
#define	W_CON	22			/* Tag Map Write: Control	*/
#define	W_ACT	32			/* TAG Map Write: Activate	*/
#define	W_RES	34			/* Tag Map Write: Reset		*/
#define	W_SLL	36			/* Tag Map Write: Set Low Level	*/
#define	W_FLO	38			/* Tag Map Write: Set Flow	*/
#define	R_H	0			/* Tag Map: Hours		*/
#define	R_M	2			/* Tag Map: Minutes		*/
#define	R_S	4			/* Tag Map: Seconds		*/
#define	R_A	6			/* Tag Map: Analog Channel 0	*/
#define	R_I	24			/* Tag Map: Digital Input	*/
#define	R_O	26			/* Tag Map: Digital Output	*/
#define	R_F	28			/* Tag Map: Flow		*/
#define	R_V	30			/* Tag Map: Volume (Total Flow)	*/
#define	R_C	32			/* Tag Map: Command Stat	*/
#define	R_L	36			/* Tag Map: Low Level		*/
#define	R_D	38			/* Tag Map: Desired Flow	*/
#define	W_CON	22			/* Tag Map: Control		*/
#define	W_ACT	32			/* TAG Map: Activate		*/
#define	W_RES	34			/* Tag Map: Reset		*/
#define	W_SLL	36			/* Tag Map: Set Low Level	*/
#define	W_FLO	38			/* Tag Map: Set Flow		*/

#define	MB_SIZE	40			/* MODBUS Database size (bytes)	*/
#define	F_SIZE	60			/* MODBUS Tx Frame size (bytes)	*/
byte	d_rd[DB_SIZE];			/* Input database buffer	*/
byte	d_wr[DB_SIZE];			/* Output database buffer	*/
byte	mb_tx[F_SIZE];			/* MODBUS Transmition frame	*/

void	mw_time	( void )		/* Process TIME write		*/
{
time	t;
	if	( d_wr[W_H]| d_wr[W_M]| d_wr[W_S] )
	{
		t.h=	d_wr[W_H];	/* Save hours in tempory struct	*/
		t.m=	d_wr[W_M];	/* Save minutes in temp struct	*/
		t.s=	d_wr[W_S];	/* Save seconds in temp struct	*/
		set_time ( &t );	/* Set RTC with new time	*/
	}
}

void	r_reset	( void )		/* Reset the read database	*/
{
byte	i;
	for	( i= 0; i< MB_SIZE; i++)
		d_rd[i]= 0;		/* Reset database read space	*/
}

void	w_reset	( void )		/* Reset the write database	*/
{
byte	i;
	for	( i= 0; i< MB_SIZE; i++)
		d_wr[i]= 0;		/* Reset database write space	*/
}

void	proc_mw	( void )		/* Process MODBUS Write		*/
{
	if	( d_wr[W_RES] )		/* Reset Write space Flag	*/
		w_reset ( );
	if	( d_wr[W_ACT] )		/* Execute Command Flag		*/
	{
		mw_time	( );		/* Set time			*/
		if	( d_wr[W_CON] )	/* Control Action Triggered	*/
			ce_sig ( d_wr[W_CON] );
		if	( d_wr[W_SLL] )	/* Set Low Level		*/
			ce_sllev ( d_wr[W_SLL] );
		if	( d_wr[W_FLO] )	/* Set Desired Flow Rate	*/
			ce_sflow ( d_wr[W_FLO] );
		w_reset	( );		/* Reset database write space	*/
	}
}

void	refr_mb	( void )		/* Refresh MODBUS Read Database	*/
{
time	t;
byte	i;
	r_reset	( );			/* Reset the read database	*/
	get_time( &t );			/* Read the Real Time Clock	*/
	d_rd[R_H]=	t.h;		/* Save hours in write DB	*/
	d_rd[R_M]=	t.m;		/* Save minutes in write DB	*/
	d_rd[R_S]=	t.s;		/* Save seconds in write DB	*/
	for	( i= 0; i< 8; i++ )	/* Store ADC Channels		*/
		d_rd[R_A+ 2* i]= adc[i];
	d_rd[R_I]=	ce_din;		/* Store digital input		*/
	d_rd[R_O]=	ce_dout;	/* Store digital output		*/
	d_rd[R_F]=	flow;		/* Store instantaneous flow	*/
	d_rd[R_V]=	f_tot;		/* Store Total Flow (Volume)	*/
	d_rd[R_L]=	l_set;		/* Store Low Level Setting	*/
	d_rd[R_D]=	f_set;		/* Store Desired Flow		*/
}

void	proc_mb	( void )		/* Process Modbus Frame		*/
{
byte	i;
	s_rnew=	0;			/* Reset frame reception flag	*/
	if ( modb_chk( s_rbuf, s_rlen ))/* Check frame integrity	*/
		return;			/* Frame Corrupt/ Wrong address	*/
	refr_mb	( );			/* Refresh MODBUS read Database	*/
	if ( modb_sl( s_rbuf, mb_tx ))	/* Process MODBUS command	*/
		return;			/* Invalid Data			*/
	out_stream=	IO_232;		/* Print to serial port		*/
	for	( i= 0; i< modb_l; i++ )
		putchar	( mb_tx[i] );	/* Send response frame		*/
	if	( modb_f )		/* MODBUS Write occured		*/
	{
		proc_mw ( );		/* Process MODBUS input		*/
		modb_f= 0;		/* Reset flag			*/
	}
	out_stream=	IO_RLCD;	/* Print to LCD by default	*/
}
