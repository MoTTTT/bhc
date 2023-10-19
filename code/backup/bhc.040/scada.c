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

#define	W_H	0			/* Tag Map: Hours		*/
#define	W_M	2			/* Tag Map: Minutes		*/
#define	W_S	4			/* Tag Map: Seconds		*/
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

void	proc_mb	( void )		/* Process Modbus Frame		*/
{
byte	i;
	s_rnew=	0;			/* Reset frame reception flag	*/
	if ( modb_chk( s_rbuf, s_rlen ))/* Check frame integrity	*/
		return;			/* Frame Corrupt/ Wrong address	*/
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
