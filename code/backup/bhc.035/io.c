/*			Copyright Q Solutions				*/
/*	File:		io.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine: I/O				*/
/*									*/
/*			History						*/
/* 23:28pm 07-08-1997 	Extracted from ce.c				*/
/*									*/
#define	ADC_IMIN	64		/* ADC minimum current loop read*/
#define	I_MIN		64		/* ADC minimum current loop read*/
#define	ADC_EN	((char*) 0x2F02FL )	/* ADC enable latch		*/
#define	DIG_OUT	((char*) 0x2F20FL )	/* Digital Output		*/
#define	D_VO		0x80		/* Digital Out: Valve Open	*/
#define	D_VC		0x40		/* Digital Out: Valve Close	*/
#define	D_V		0xC0		/* Digital Out: Valve (both)	*/
#define	D_RUN		0x20		/* Digital Out: Motor Run	*/
#define D_L		0x08		/* Digital Out: Low Lev Lockout	*/
#define	D_T		0x02		/* Digital Out: Test		*/
#define D_P		0x01		/* Digital Out: Power		*/
#define	D_I		0x00		/* Digital Out: Idle		*/
#define	DIG_IN	((char*) 0x2F20FL )	/* Digital Input		*/
#define	D_PF		0x40		/* Digital In: Phase Fail	*/
#define	D_OV		0x20		/* Digital In: Overload		*/
#define	D_MAN		0x10		/* Digital In: Manual		*/
#define	D_AUTO		0x08		/* Digital In: Auto		*/
#define	D_R		0x04		/* Digital In: Running		*/
#define	PF_CNT		10		/* Number of samples to take	*/
#define	PF_SH		0.7* PF_CNT	/* Proportion of sucesses for 1	*/
#define	PF_SL		0.3* PF_CNT	/* Proportion of sucesses for 0	*/
#define	O_CNT		10		/* Number of samples to take	*/
#define	O_SH		0.7* O_CNT	/* Proportion of sucesses for 1	*/
#define	O_SL		0.3* O_CNT	/* Proportion of sucesses for 0	*/

byte	pf_tot=		0;		/* Phase fail sample successes	*/
byte	pf_cnt=		0;		/* Number of samples taken	*/
byte	o_tot=		0;		/* Overload sample successes	*/
byte	o_cnt=		0;		/* Number of samples taken	*/
byte	ce_dout;			/* Control Engine Dig Output(ro)*/
byte	ce_din;				/* Control Engine Dig Input	*/
byte	ce_sin;				/* Control Engine Signal Input	*/
byte	ce_run;				/* Public RUN flag		*/
byte	ce_lll;				/* Low Level Lockout Flag	*/
byte	ce_man;				/* Manual Flag			*/
byte	f_set;				/* Flow Setting			*/
byte	l_set;				/* Low Level Setting		*/
bit	ui_flag;			/* User interface timer flag	*/
bit	db_flag;			/* Data Logger timer flag	*/

byte	ce_d		( byte out )	/* Write to Output Port		*/
{
	return(*DIG_OUT= ce_dout= out);	/* Set Output Port Absolute	*/
}

byte	ce_dset		( byte msk )	/* Set Output Port Pin/s	*/
{
	return(*DIG_OUT=ce_dout|=msk);	/* Set bits in mask		*/
}

byte	ce_dclr		( byte msk )	/* Clear Output Port Pin/s	*/
{
	return(*DIG_OUT=ce_dout&=~msk);	/* Clear bits in mask	 	*/
}

byte	ce_dtog		( byte msk )	/* Toggle Output Port Pin/s	*/
{
	if	( ce_dout& msk )	/* If any of the bits are set	*/
		return( ce_dclr( msk ));/*  Clear them all, otherwise	*/
	else	return( ce_dset( msk ));/*  Set them all.		*/
}

void	p_phase	( void )		/* Check Phase Fail Condition	*/
{
	if	( ce_din& D_PF )	/* Phase Fail condition sampled	*/
		pf_tot++;		/* Notch one up			*/
	if	( ++pf_cnt== PF_CNT )	/* Full sample set ?		*/
	{
		if ( pf_tot> PF_SH )	/* Success count high		*/
			a_stat|= A_PF;	/* Set alarm			*/
		if ( pf_tot< PF_SL )	/* Success count low		*/
			a_stat&= ~A_PF;	/* Reset Alarm			*/
		pf_tot= pf_cnt=	0;	/* Reset counters		*/
	}
}

void	p_overl	( void )		/* Check Overload Condition	*/
{
	if	( ce_din& D_OV )	/* Overload condition sampled	*/
		o_tot++;		/* Notch one up			*/
	if	( ++o_cnt== O_CNT )	/* Full sample set ?		*/
	{
		if ( o_tot> O_SH )	/* Success count high		*/
			a_stat|= A_OV;	/* Set alarm			*/
		if ( o_tot< O_SL )	/* Success count low		*/
			a_stat&= ~A_OV;	/* Reset Alarm			*/
		o_tot= o_cnt=	0;	/* Reset counter		*/
	}
}

void	p_sig		( void )	/* Process signal input		*/
{
	switch	( ce_sin )		/* Which signal ?		*/
	{
	case S_STA:			/* Recieved START Signal	*/
		if ( ce_mode!= CE_AUTO )/* Check for permission		*/
			break;
		ce_run=		1;	/* Set Control Engine RUN flag	*/
		break;
	case S_STO:			/* Recieved STOP Signal		*/
		if ( ce_mode!= CE_AUTO )/* Check for permission		*/
			break;
		ce_run=		0;	/* Reset Control Engine RUN flag*/
		break;
	case S_RLL:			/* Reset Low Level Lockout	*/
		ce_dclr	( D_L );	/* Reset LLL output		*/
		break;
	case S_DEB:			/* Debug test event		*/
		ce_dtog	( D_T );	/* Toggle Test Output		*/
		break;
	case S_PT:			/* Toggle Power Indicator	*/
		ce_dtog	( D_P );	/* Toggle Power Output		*/
		break;
	case S_RF:			/* Reset Total Flow		*/
		f_tot=	0;		/* Reset counter		*/
		break;
	default: break;
	}
	ce_sin=	0;			/* Reset Signal			*/
}

void	ce_sig		( byte in )	/* Userland signal		*/
{
	ce_sin=	in;			/* Store the signal		*/
}