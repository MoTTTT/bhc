/*			Copyright Q Solutions				*/
/*	File:		valve.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Valve Module			*/
/*									*/
/*			History						*/
/* 19:34pm 05-17-1997  	Extracted from ce.c				*/
/*									*/

/*		Valve Measurement and Control Parameters and variables	*/
#define	V		0		/* ADC channel: Valve		*/
#define V_SF		256/100		/* Scale factor Byte - %	*/
#define	V_CLO		5*V_SF		/* Valve position: Closed (%)	*/
#define	V_OPN		50*V_SF		/* Valve position: Default (%)	*/
#define	V_FULL		95*V_SF		/* Valve position: Full open (%)*/
#define	V_PLEN		6		/* Pulse length x 10ms		*/
byte	v_pulse=	0;		/* Valve pulse timer		*/
byte	v_cur;				/* Current Valve position	*/

void	p_valv	( void )		/* Process Valve, every 10ms	*/
{
	v_cur=	adc[V];
	switch	( v_stat )
	{
	case V_I:			/* Valve control Idle		*/
		break;
	case V_O:			/* Valve Opening		*/
		if	( flow> f_set-1)/* Flow above lower threshold	*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
			break;
		}
		if	( v_cur>V_FULL)	/* Valve open full		*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
			break;
		}
		break;			/* Continue opening		*/
	case V_C:			/* Valve Closing		*/
		if	( v_cur< V_CLO )/* Valve Closed			*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_I;	/* Change state			*/
		}
		break;
	case V_PC:			/* Adjusting valve position	*/
		if	( flow< f_set )	/* Flow below threshold		*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
		}
		if	( !(v_pulse--) )/* Check for end of pulse	*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_PO:			/* Adjusting valve position	*/
		if	( flow> f_set )	/* Flow above threshold		*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
		}
		if	( !(v_pulse--) )/* Check for end of pulse	*/
		{
			ce_dclr( D_V );	/* Switch off valve control	*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_S:			/* Valve position set		*/
		if	( flow< f_set-1)/* Flow below threshold		*/
		{
			if(v_cur>V_FULL)/* Valve open full		*/
				break;	/* Do nothing			*/
			ce_dclr( D_V );	/* Switch off valve control	*/
			ce_dset( D_VO );/* Open Valve			*/
			v_stat= V_PO;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		if	( flow> f_set+1)/* Flow above threshold		*/
		{
			if(v_cur<V_CLO)	/* Valve closed			*/
				break;	/* Do nothing			*/
			ce_dclr( D_V );	/* Switch off valve control	*/
			ce_dset( D_VC );/* Close Valve			*/
			v_stat= V_PC;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		break;
	case V_E:			/* Valve control error		*/
		ce_dclr( D_V );		/* Switch off valve control	*/
		break;
	default:			/* Invalid state		*/
		ce_dclr( D_V );		/* Switch off valve control	*/
		v_stat= V_I;		/* Set to default		*/
		break;
	}
}