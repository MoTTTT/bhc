/*			Copyright Q Solutions				*/
/*	File:		press.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Pressure Module			*/
/*									*/
/*			History						*/
/* 19:24pm 05-17-1997  	Extracted from ce.c				*/
/*									*/

/*		Pressure Measurement Parameters and Variables		*/
#define	P		2		/* ADC channel: Pressure	*/

#define	P_FS		10		/* Pressure: Full Scale (Bar)	*/
#define	P_THB		7		/* Pressure: Hi Thresh. (Bar)	*/
#define	P_TLB		3		/* Pressure: Lo Thresh. (Bar)	*/
#define	P_SF		(256-I_MIN)/P_FS/* Pressure Measure scale fact	*/
#define	P_TH		P_THB*P_SF+I_MIN/* Pressure: Hi Thresh. (0..255)*/
#define	P_TL		P_TLB*P_SF+I_MIN/* Pressure: Lo Thresh.	(0..255)*/

#define	P_CNT		30		/* Number of pressure samples	*/
#define	P_SH		0.7*P_CNT	/* Pres: sample hi theshold	*/
#define	P_SL		0.3*P_CNT	/* Pres: sample lo theshold	*/

byte idata p_htot=	0;		/* Pressure high sum		*/
byte idata p_ltot=	0;		/* Pressure low sum		*/
byte idata p_cnt=	0;		/* Pressure sample count	*/
byte idata p_ave=	0;		/* Pressure (0..255)		*/
byte	pressure=	0;		/* Pressure (Barx10)		*/

void	calc_press	( void )	/* Refresh Pressure Variable	*/
{
float	p;
	if	( p_ave < ADC_IMIN )	/* Sensor error			*/
		pressure= 0;		/* Set error value		*/
	else
	{
		p= p_ave- ADC_IMIN;	/* Subtract offset		*/
		p/= 255-ADC_IMIN;	/* Scale to unit		*/
		p*= P_FS* 10;		/* Scale to measurement		*/
		pressure= (byte) p;	/* Store public variable	*/
	}
}

void	p_press		( void )	/* Check pressure condition	*/
{
	p_ave=	adc[P];			/* Get ADC Value		*/
	if	( p_ave> P_TH )		/* High Pressure ?		*/
		p_htot++;		/* Notch one up			*/
	else if	( p_ave< P_TL )		/* Low Pressure ?		*/
		p_ltot++;		/* Notch one up			*/
	if	( ++p_cnt== P_CNT )	/* Full set of samples		*/
	{
		if	( p_htot> P_SH )/* High pressure count high	*/
			a_stat|= A_PH;	/* Switch on warning flag	*/
		else if	( p_htot< P_SL )/* High pressure count low	*/
			a_stat&= ~A_PH;	/* Reset warning		*/
		if	( p_ltot> P_SH )/* Low pressure count high	*/
			a_stat|= A_PL;	/* Switch on warning flag	*/
		else if	( p_ltot< P_SL )/* High pressure count low	*/
			a_stat&= ~A_PL;	/* Reset warning		*/
		p_cnt=p_htot=p_ltot=0;	/* Reset count, sum		*/
	}
}