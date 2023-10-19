/*			Copyright Q Solutions				*/
/*	File:		water.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Water Level Module		*/
/*									*/
/*			History						*/
/* 19:29pm 05-17-1997  	Extracted from ce.c				*/
/*									*/

/*		Water Level Measurement Parameters			*/
#define	W		2		/* ADC channel: Water level	*/
#define	W_FS		25		/* Water level: Full scale	*/
#define	W_T1		4		/* Water Low (in m above LL)	*/
#define	W_CNT		20		/* Number of water samples	*/
#define	W_SL		0.3*W_CNT	/* Water Low threshold off	*/
#define	W_SH		0.7*W_CNT	/* Water Low threshold on	*/
#define	W_DLL		5		/* Default Low Level (in m)	*/
byte idata w_tot1=	0;		/* Water Low Count 1		*/
byte idata w_tot2=	0;		/* Water Low Count 2		*/
byte idata w_cnt=	0;		/* Water sample count		*/
byte	w_ave=		0;		/* Water sample average		*/
byte	w_ll=		0;		/* Low Level Setting: Scaled	*/
byte	w_wl=		0;		/* Water Low Threshold		*/
float	water_level=	0;		/* Water level			*/

bit	ce_sllev	( byte in )	/* Set Low Level		*/
{
uint	t;
	if	(( in> 1 )& ( in< 20 ))	/* New Level in range		*/
	{
		t= l_set= in;		/* Set new value		*/
		t*= 255- ADC_IMIN;	/* Scale to ADC			*/
		t/= W_FS;		/* Scale to Unit ADC		*/
		w_ll= t+ ADC_IMIN;	/* Set Low Level Lockout Thresh	*/
		t= l_set+ W_T1;		/* Offset to Water low		*/
		t*= 255- ADC_IMIN;	/* Scale to ADC			*/
		t/= W_FS;		/* Scale to Unit ADC		*/
		w_wl= t+ ADC_IMIN;	/* Set Water Low Threshold	*/
		return	( 1 );
	}
	return	( 0 );			/* Error, not setting		*/
}

void	calc_level	( void )	/* Refresh Level Variable	*/
{
	if	( w_ave < ADC_IMIN )	/* Sensor error			*/
		water_level= 0;		/* Set sane value		*/
	else
	{
		water_level= w_ave- ADC_IMIN;	/* Subtract offset	*/
		water_level/= 255-ADC_IMIN;	/* Scale to unit	*/
		water_level*= W_FS;		/* Scale to measurement	*/
	}
}

void	p_water		( void )	/* Check Water condition	*/
{
	w_ave=	adc[W];			/* Get ADC value		*/
	if	( w_ave< w_ll )		/* Is level below low level	*/
		w_tot1++;		/* Notch one up			*/
	if	( w_ave< w_wl )		/* Is level below water low	*/
		w_tot2++;		/* Notch one up			*/
	if	( ++w_cnt== W_CNT )	/* Full set of samples		*/
	{
		if	( w_tot1> W_SH )/* Low level count high		*/
		{
			ce_lll=	1;	/* Set Low Level Lockout Flag	*/
			a_stat|= A_LL;	/* Switch on alarm flag		*/
		}
		else if	( w_tot1< W_SL )/* Low level count low		*/
			a_stat&= ~A_LL;	/* Reset alarm			*/
		if	( w_tot2> W_SH )/* Low level count high		*/
			a_stat|= A_WL;	/* Switch on alarm flag		*/
		else if	( w_tot2< W_SL )/* Low level count low		*/
			a_stat&= ~A_WL;	/* Reset alarm			*/
		w_cnt=w_tot1=w_tot2=0;	/* Reset count, sum		*/
	}
}