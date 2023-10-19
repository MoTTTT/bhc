/*			Copyright Q Solutions				*/
/*	File:		ce.h						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Header File			*/
/*									*/
/*			History						*/
/* 17:52 27/04/1997   	Written from scratch				*/
/*									*/

void	init_ce	( void );		/* Initialise Control Engine	*/
bit	ce_cos	( void );		/* Public Change Of State test	*/
void	ce_start( void );		/* Start Control Engine		*/
void	ce_stop	( void );		/* Stop Control Engine		*/
