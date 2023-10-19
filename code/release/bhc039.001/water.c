/*			Copyright Q Solutions				*/
/*	File:		water.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Water Level Module		*/
/*									*/
/*			History						*/
/* 19:29pm 05-17-1997  	Extracted from ce.c				*/
/*									*/

/*		Water Level Measurement Parameters			*/
#define	W		1		/* ADC channel: Water level	*/
#define	W_FS		25		/* Water level: Full scale	*/
#define	W_CNT		16		/* Number of water samples	*/
#define	W_DLL		5		/* Default Low Level (in m)	*/
#define	W_ERR		99		/* Water sensor error value	*/
byte	idata w_cnt=	0;		/* Water sample count		*/
byte	idata w_ave;			/* Water level average		*/
uint	idata w_sum=	0;		/* Sum of water level samples	*/
byte	idata w_ll;			/* Low Level Setting: Scaled	*/
byte	water_level;			/* Water level			*/

bit	ce_sllev	( byte in )	/* Set Low Level		*/
{
uint	t;
	if	(( in> 1 )& ( in< 20 ))	/* New Level in range		*/
	{
		t= l_set= in;		/* Set new value		*/
		t*= 255- ADC_IMIN;	/* Scale to ADC			*/
		t/= W_FS;		/* Scale to Unit ADC		*/
		w_ll= t+ ADC_IMIN;	/* Set Low Level Lockout Thresh	*/
		return	( 1 );
	}
	return	( 0 );			/* Error, not setting		*/
}

void	calc_level	( void )	/* Refresh Level Variable	*/
{
float	f;
	if	( w_ave < ADC_IMIN )	/* Sensor error			*/
		water_level= W_ERR;	/* Set error value		*/
	else
	{
		f= w_ave- ADC_IMIN;	/* Subtract offset		*/
		f/= 255-ADC_IMIN;	/* Scale to unit		*/
		f*= W_FS* 10;		/* Calibrate Full Scale 	*/
		water_level=(byte) f;	/* Set global value		*/
	}
}

void	p_water		( void )	/* Check Water condition	*/
{
byte	w;
	w_sum+=	w= adc[W];		/* Get ADC value, add to total	*/
	if	( ++w_cnt== W_CNT )	/* Full set of samples		*/
	{
		w_ave=	w_sum/ W_CNT;	/* Calculate average level	*/
		if	( w_ave< w_ll )	/* Average level low		*/
		{
			ce_lll=	1;	/* Set Low Level Lockout Flag	*/
			a_stat|= A_LL;	/* Switch on alarm flag		*/
		}
		else	a_stat&= ~A_LL;	/* Reset alarm			*/
		w_cnt= w_sum= 0;	/* Reset counter, sum, and fuzz	*/
	}
}