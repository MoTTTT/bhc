/*			Copyright Q Solutions				*/
/*	File:		flow.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Flow Module			*/
/*									*/
/*			History						*/
/* 19:50pm 05-17-1997  	Extracted from ce.c				*/
/*									*/

/*		Flow Measurement Parameters and variables		*/
#define	FI		0		/* Flow interrupt vector	*/
#define	F_EI		EX0		/* Flow interrupt enable	*/
#define	F_TL		5		/* Flow threshold low		*/
#define	F_TH		10		/* Flow threshold high		*/
#define	F_DEF		15		/* Default Flow			*/
byte	f_ave=		0;		/* Flow average			*/
ulong	f_tot=		0;		/* Flow total			*/
byte	f_cnt=		0;		/* Flow interrupt count		*/
ulong	t_last=		0;		/* Time at last flow interrupt	*/

void	init_flow	( void )	/* Initialise Flow Measurement	*/
{
	f_cnt=		0;		/* Reset Flow interrupt count	*/
	f_flo=		0;		/* Reset Current Flow value	*/
	F_EI=		1;		/* Enable External Interrupt 	*/
	t_last=		ce_time;	/* Set period start time	*/
}

bit	ce_sflow	( byte in )	/* Set Desired Flow		*/
{
	if	(( in> 0 )& ( in< 40 ))	/* New Flow in range		*/
	{
		f_set=	in;		/* Set new value		*/
		return	( 1 );
	}
	return	( 0 );			/* Error, not setting		*/
}

void	flow_int( void ) interrupt FI	/* 1 Litre Flow interrupt	*/
{
ulong	p;
	f_tot++;			/* Increment Total Flow Counter	*/
	if	( t_last> ce_time )	/* Timer rollover		*/
		p= 0xFFFF- t_last+ ce_time;	/* Calculate period (ms)*/
	else	p= ce_time- t_last;
	f_flo=	(byte) (1000/p);	/* Convert to l/s		*/
}