/*			Copyright Q Solutions				*/
/*	File:		scada.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		SCADA Interface Module				*/
/*									*/
/*			History						*/
/* 00:24 01/12/1997	Written from scratch				*/
/* 00:49 01/05/1998 	Cleaned up					*/
/*									*/

#include	<modbus.h>		/* MODBUS protocol handler	*/

#define MADDR	1			/* MODBUS address		*/
#define	W_SLL	36			/* Tag Map Write: Set Low Level	*/
#define	W_FLO	38			/* Tag Map Write: Set Flow	*/
#define	R_I	24			/* Tag Map: Digital Input	*/
#define	R_O	26			/* Tag Map: Digital Output	*/
#define	R_F	28			/* Tag Map: Flow		*/
#define	R_V	30			/* Tag Map: Volume (Total Flow)	*/
#define	R_L	36			/* Tag Map: Low Level		*/
#define	R_D	38			/* Tag Map: Desired Flow	*/
#define	STADC	0			/* Don't start ADC sample	*/

void	proc_mw	( void )		/* Process MODBUS Write		*/
{
	modb_f= 0;			/* Reset flag			*/
	if ( modb_wr[MW_RST]== MC_RST )	/* Reset Write space Flag	*/
		modb_rw ( );
	if ( modb_wr[MW_ACT]== MC_ACT )	/* Execute Command Flag		*/
	{
		modb_wrtc( );		/* Set Real Time Clock's Time	*/
		if ( modb_wr[MW_CON] )	/* Control Action Triggered	*/
			ce_sig ( modb_wr[MW_CON] );
		if ( modb_wr[W_SLL] )	/* Set Low Level		*/
			ce_sllev ( modb_wr[W_SLL] );
		if ( modb_wr[W_FLO] )	/* Set Desired Flow Rate	*/
			ce_sflow ( modb_wr[W_FLO] );
		modb_rw	( );		/* Reset database write space	*/
	}
}

void	refr_mb	( void )		/* Refresh MODBUS Read Database	*/
{
	modb_refr	( STADC );	/* Refresh time and adc		*/
	modb_rd[R_I]=	ce_din;		/* Store digital input		*/
	modb_rd[R_O]=	ce_dout;	/* Store digital output		*/
	modb_rd[R_F]=	flow;		/* Store instantaneous flow	*/
	modb_rd[R_V]=	f_tot;		/* Store Total Flow (Volume)	*/
	modb_rd[R_L]=	l_set;		/* Store Low Level Setting	*/
	modb_rd[R_D]=	f_set;		/* Store Desired Flow		*/
}

void	proc_mb	( void )		/* Process Modbus Frame		*/
{
byte	i;
	ce_sig	( S_PT );
	refr_mb	( );			/* Refresh MODBUS read Database	*/
	modb_proc( );			/* Process MODBUS Frame		*/
	if	( modb_f ) proc_mw ( );	/* Process MODBUS input		*/
}
