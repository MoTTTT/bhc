#pragma		ROM (COMPACT)
#pragma		SMALL
#include	<stdio.h>
#include	<reg552.h>
#define 	BHC_ADR	0x68		/* Main Controller Slave Addr	*/

#define		IO_RLCD	0x01		/* io_stream value for LCD data */
#define		IO_RLCC	0x02		/* io_stream value for LCD ctrl	*/
#define		IO_232	0x03		/* io_stream value for RS232	*/
#define		IO_NLCD	0x04		/* io_stream val for Local LCD	*/

typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/

#include	<iic.h>			/* IIC Bus routines		*/
#include	<rtc.h>			/* Real time clock routines	*/
#include	<nodelcd.h>		/* Local LCD module routines	*/
#include	<lcdpad.h>		/* Remote LCD & keypad routines	*/
#include	<nkey.h>		/* Local Keypad module routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/

const char *signon1= "Master  Borehole";/* Line 1 of sign on screen	*/
const char *signon2= "   Controller   ";/* Line 2 of signon screen	*/
const char *connect= "   Connected    ";/* Line 2 of signon screen	*/
const char *prompt=  "Command:       ";	/* Command prompt		*/
const char *status=  "Status:        ";	/* Status prompt		*/