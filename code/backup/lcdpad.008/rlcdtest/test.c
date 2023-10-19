/*			Copyright Q Solutions				*/
/*	File:		test.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Remote I2C LcdPad test program			*/
/*									*/
/*			History						*/
/* 15:58 20/04/1997  	Written from scratch				*/
/*			Initialisation and main loop.			*/

#pragma		ROM (COMPACT)
#pragma		LARGE

/* Library files							*/
#include	<stdio.h>
#include	<reg552.h>

typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/

#include	<iic.h>			/* IIC Bus routines		*/
#include	<nodelcd.h>		/* Local LCD module routines	*/
#include	<rlcdpad.h>		/* i2c Bus LCD module routines	*/
#include	<nkey.h>		/* Local Keypad module routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/

#define		IIC_ADDR 0x68		/* IIC Slave Address		*/
#define		REM_ADDR 0x60		/* IIC Remote Address		*/
#define		IO_NLCD	0x01		/* Output to uNode LCD		*/
#define		IO_RLCD	0x02		/* Output to Remote LCD		*/

const char *signon1= " Remote LcdPad  ";/* Line 1 of sign on screen	*/
const char *signon2= "     Tester     ";/* Line 2 of signon screen	*/

char	out_stream= IO_NLCD;		/* Output port			*/
uint	counter;			/* Routine cycle counter	*/
uint	iiccount;			/* i2c input counter		*/

void	signon		( )		/* Splash Screen		*/
{
	paint_nlcd ( signon1, signon2 );
}

void	rsignon		( )		/* Remote Screen		*/
{
	out_stream=	IO_RLCD;
	paint_rlcd ( signon1, signon2 );
	out_stream=	IO_NLCD;
}

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	init_nlcd	( );		/* Initialise local LCD		*/
	init_nkey	( );		/* Initialise local keypad	*/
	iic_init	( IIC_ADDR );	/* Initialise IIC Bus		*/
	signon		( );		/* Splash screen		*/
	init_rlcd	( );		/* Initialise Remote LcdPad	*/
	rsignon		( );		/* Remote Splash Screen		*/
//	init_rkpad	( );		/* Initialise remote keypad 	*/
}

char	putchar		( char out )
{
	switch	( out_stream )
	{
	case IO_NLCD:			/* Write to local LCD		*/
		nputchar	( out );
		break;
	case IO_RLCD:
		rputchar	( out );
		break;
	}
	return	( out );
}

void	service_iicin	( )
{
byte	in;
	if	( iic_sready )
	{
		iiccount++;
		set_nlcd( NLCD_L1 );
		in=	rgetkey( );
		printf	( "%3bu, Code: %3bu", iiccount, in );
//		iic_sready=0;
	}
}

void	service_keypad	( )		/* Read and process keypad	*/
{
byte	in;
	if	( x1_flag )
	{
		in= ngetkey	( );
		switch	( in )
		{
		case 0:
		case 1:
			set_nlcd	( NLCD_L2 );
			printf		( "  Sending 'A'   " );
			rputchar	( 'A' );
			break;
		case 2:
			set_nlcd	( NLCD_L2 );
			printf		( " Sending SIGNON " );
			rsignon		( );
			break;
		default:
			set_nlcd	( NLCD_L2 );
			printf		( "  Sending CLEAR " );
			set_rlcd	( RL_CLR );
			break;
		}
	}
}

void	main	( void )		/* Main Loop			*/
{
	initialise	( );		/* Set up drivers, sign on	*/
	while	( 1 )
	{
		wdog	( 10 );		/* Refresh watchdog		*/
		service_keypad	( );	/* Process input		*/
		service_iicin	( );	/* Process input		*/
		counter++;		/* Increment loop counter	*/
	}
}