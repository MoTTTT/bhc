/*			Copyright Q Solutions				*/
/*	File:		flow.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Flow Module			*/
/*									*/
/*			History						*/
/* 19:50pm 05-17-1997  	Extracted from ce.c				*/
/* 10:49am 06-22-1997	Rewritten to poll for flow pulse		*/
/*									*/

/*		Flow Measurement Parameters and variables		*/
#define	F_DEF		15		/* Default Flow			*/
#define	F_TOTMAX	9999999		/* Maximum total flow		*/
#define	F_IN		T1		/* Flow pulse input		*/
#define	F_ZERO		1000		/* Max time to COS for nonzero	*/
uint	t_cos=		F_ZERO;		/* Number of samples since COS	*/
ulong	f_tot;				/* Flow total			*/
ulong	t_last;				/* Time at last flow interrupt	*/
ulong	t_litre;			/* Time for last litre		*/
byte	flow;				/* Public Flow variable		*/
bit	f_pulse=	1;		/* Last flow input state	*/

void	init_flow	( void )	/* Initialise Flow Measurement	*/
{
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

void	flow_upd	( void )	/* Calculate new flow		*/
{
	if	( ++f_tot==F_TOTMAX )	/* Increment Total Flow Counter	*/
		f_tot=	1;		/* Roll over 			*/
	if	( t_last> ce_time )	/* Timer rollover		*/
	{
		t_litre= CE_TMAX- t_last
			+ ce_time;	/* Calculate period (ms)	*/
	}
	else	t_litre= ce_time- t_last;
	t_last=	ce_time;		/* Save current time		*/
	flow=	1000/ t_litre;		/* Convert litre period to flow	*/
}

void	flow_poll	( void )	/* Check for flow pulse		*/
{
	if	( F_IN )		/* Digital High level		*/
	{
		if	( !f_pulse )	/* Change of state: New Pulse	*/
		{
			f_pulse= 1;	/* Set state variable		*/
			flow_upd( );	/* Update flow variables	*/
			t_cos= F_ZERO;	/* Reset flow timout counter	*/
		}
		else			/* No change of state		*/
		{
			if (!--t_cos)	/* Check for no flow		*/
				flow= 0;/* Set flow to zero		*/
		}
	}
	else				/* Digital Low level		*/
	{
		if	( f_pulse )	/* Change of state: End of Pulse*/
		{
			f_pulse= 0;	/* Reset state variable		*/
			t_cos= F_ZERO;	/* Reset flow timout counter	*/
		}
		else			/* No change of state		*/
		{
			if (!--t_cos)	/* Check for no flow		*/
				flow= 0;/* Set flow to zero		*/
		}

	}
}