/*			Copyright Q Solutions				*/
/*	File:		db.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		Database Structure, and related functions	*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	Extracted from main bhc.c 			*/
/* 01:45am 09-13-1997 	Change Data Base Record structure		*/
/*									*/


#define	DB_SIZE	2000			/* Database size		*/
#define	ADC_MAX	8			/* Number of ADC Channels	*/

typedef struct				/* Database Record Definition	*/
{
	byte	a[ADC_MAX];		/* ADC Values			*/
	byte	i;			/* Digital Input		*/
	byte	o;			/* Digital Output		*/
	byte	f;			/* Water Flow			*/
	ulong	t;			/* Total Flow			*/
}	sample;

sample	DB[ DB_SIZE ];			/* Sample Data Base		*/
sample	*log=	DB;			/* Pointer to database record	*/
uint	db_idx=	0;			/* Database Index		*/
byte	db_page=0;			/* Database page counter	*/

void	db_store	( uint idx )	/* Write a record to the dbase	*/
{
byte	i;
	for	( i= 0; i< ADC_MAX; i++ )
		DB[idx].a[i]=	adc[i];	/* Store ADC value 		*/
	DB[idx].i=	ce_din;		/* Store digital input		*/
	DB[idx].o=	ce_dout;	/* Store digital output		*/
	DB[idx].f=	flow;		/* Store flow			*/
	DB[idx].t=	f_tot;		/* Store total flow		*/
}

void	io_print	( byte out )	/* Print a digtal value		*/
{
byte	i, t;
	t= 	out;			/* Temporary reg for shifting	*/
	for	( i= 0; i< 8; i++ )	/* Check eight bits, from left	*/
	{
		if	( t& MSB )	/* Most significant bit is set	*/
			putchar	( '1' );/* Print a "1"			*/
		else	putchar	( '0' );/* Print a "0"			*/
		t<<=	1;		/* Shift to examine next bit	*/
	}
	putchar	( ' ' );		/* Print a space at the end	*/
}

void	db_print	( uint idx )	/* Print a database record	*/
{
byte	i;
	for	( i= 0; i< ADC_MAX; i++)/* Print an array of values	*/
		printf	( "%03bu ",
			DB[idx].a[i]);	/* Print ADC value 		*/
	io_print( a_stat );		/* Print Errors			*/
	io_print( DB[idx].i );		/* Print Input			*/
	io_print( DB[idx].o );		/* Print Output			*/
	printf	( "%02bu %07lu",	/* Print flow value		*/
		DB[idx].f, DB[idx].t );	/* Current Flow, Total Flow	*/
	sputchar ( 0x0A );
}

void	db_log		( void )	/* Create a new database record	*/
{
	db_store( db_idx );		/* Write status to database	*/
	if	( ++db_idx== DB_SIZE )	/* Increment, check page full	*/
	{
		db_idx=	0;		/* Reset index			*/
		if (++db_page== 3)	/* Increment page, check end	*/
			db_page= 0;	/* Start at first page		*/
	}
}
