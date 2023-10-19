/*			Copyright Q Solutions				*/
/*	File:		ce.h						*/
/*	Programmer:	MoT						*/
/*	Module:		Control Engine Header File			*/
/*									*/
/*			History						*/
/* 17:52 27/04/1997   	Written from scratch				*/
/*									*/

#define	CE_IDLE		0		/* CE Idle			*/
#define	CE_STA		1		/* CE started			*/
#define	CE_RUN		2		/* CE Running			*/
#define	CE_STOP		3		/* CE busy shutting down	*/
#define	CE_ERR		4		/* Already in Error State	*/
#define	V_I		0		/* VALVE State: Idle		*/
#define	V_O		1		/* VALVE State: Opening		*/
#define	V_C		2		/* VALVE State: Closing		*/
#define	V_P		3		/* VALVE State: Pre Set		*/
#define	V_S		4		/* VALVE State: Set		*/
#define	V_E		5		/* VALVE State: Error		*/
#define	A_PF		0x01		/* Alarm: Phase fail		*/
#define	A_OV		0x02		/* Alarm: Overload		*/
#define	A_PH		0x04		/* Alarm: Pressure High		*/
#define	A_LL		0x08		/* Alarm: Low Level Lockout	*/
#define	A_WL		0x10		/* Alarm: Water Low		*/
#define	A_PL		0x20		/* Alarm: Pressure Low		*/
#define	A_SM		0x0F		/* Alarm: Shutdown Mask		*/
#define	S_STO		0		/* Signal: Stop Control Engine	*/
#define	S_STA		1		/* Signal: Start Control Engine	*/
#define	S_RLL		2		/* Signal: Reset Low Level Lock	*/
#define	S_DEB		100		/* Signal: Debug		*/

extern	bit	ui_flag;		/* User Interface flag		*/
extern	bit	overload;		/* Overload Flag		*/
extern	bit	phase_fail;		/* Phase Fail flag		*/
extern	bit	ce_run;			/* Public RUN flag		*/
extern	bit	ce_lll;			/* Low Level Lockout Flag	*/
extern	bit	ce_man;			/* Manual flag			*/
extern	byte	ce_stat;		/* Control Engine state variable*/
extern	byte	v_stat;			/* Valve control state		*/
extern	byte	a_stat;			/* Alarm status			*/
extern	byte	v_set;			/* Desired valve position	*/
extern	byte	f_set;			/* Flow Setting			*/
extern	byte	l_set;			/* Low Level Setting		*/
extern	byte	p_ave;			/* Average Pressure Reading	*/
extern	byte	w_ave;			/* Average Water Reading 	*/
extern	byte	f_ave;			/* Average Flow Reading 	*/
extern	ulong	f_tot;			/* Total Flow Reading 		*/
extern	float	pressure;		/* Latest Pressure Calculation	*/
extern	float	water_level;		/* Latest Level Calculation	*/

void	init_ce		( void );	/* Initialise Control Engine	*/
void	ce_sig		( char in );	/* Signal Control Engine	*/
bit	ce_sflow	( byte in );	/* Set Desired Flow		*/
bit	ce_sllev	( byte in );	/* Set Low Level		*/
void	calc_press	( void );	/* Update Pressure Variable	*/
void	calc_level	( void );	/* Update Level Variable	*/
byte	ce_out		( byte out );	/* Set Control Engine Digital	*/