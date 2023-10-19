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
#define	V_PRE		3		/* Valve Contr. Presc. (x10ms)	*/
#define V_SF		256/100		/* Scale factor Byte - %	*/
#define	V_CLO		5*V_SF		/* Valve position: Closed (%)	*/
#define	V_OPN		50*V_SF		/* Valve position: Default (%)	*/
#define	V_PLEN		1		/* Valve pulse length		*/
byte	v_pre=		V_PRE;		/* VALVE Prescale counter	*/
//sbit	v_open=		0x91;		/* Valve open control line	*/
//sbit	v_close=	0x92;		/* Valve close control line	*/
bit	v_open=		0;		/* Valve open control line	*/
bit	v_close=	0;		/* Valve close control line	*/
byte	v_pulse=	0;		/* Valve pulse timer		*/
byte	v_cur;				/* Current Valve position	*/

void	p_valv	( void )		/* Process Valve		*/
{
	v_cur=	adc[V];
	switch	( v_stat )
	{
	case V_I:			/* Valve control Idle		*/
		break;
	case V_O:			/* Valve Opening		*/
		if((v_cur>V_OPN)|!ce_run)/* Default Position, or STOP	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_C:			/* Valve Closing		*/
		if((v_cur<V_CLO)|ce_run)/* Closed, or START received	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_I;	/* Change state			*/
		}
		break;
	case V_P:			/* Adjusting valve position	*/
		if	( !(v_pulse--) )/* Check for end of pulse	*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_S;	/* Change state			*/
		}
		break;
	case V_S:			/* Valve position set		*/
		if	( f_flo< F_TL )	/* Flow below threshold		*/
		{
			v_open=	1;	/* Start Actuator Open		*/
			v_close= 0;	/* No Actuator Close		*/
			v_stat= V_P;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		if	( f_flo> F_TH )	/* Flow above threshold		*/
		{
			v_open=	0;	/* No Actuator Open		*/
			v_close= 1;	/* Start Actuator Close		*/
			v_stat= V_P;	/* Change state			*/
			v_pulse=V_PLEN;	/* Set pulse length		*/
		}
		break;
	case V_E:			/* Valve control error		*/
		break;
	default:			/* Invalid state		*/
		v_stat= V_I;		/* Set to default		*/
		break;
	}
}