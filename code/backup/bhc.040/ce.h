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
#define	CE_ERR		10		/* CE Error State		*/
#define	CE_OFF		0		/* CE Off Mode			*/
#define	CE_MAN		1		/* CE Manual Mode		*/
#define	CE_AUTO		2		/* CE Auto Mode			*/
#define	V_I		0		/* VALVE State: Idle		*/
#define	V_O		1		/* VALVE State: Opening		*/
#define	V_C		2		/* VALVE State: Closing		*/
#define	V_PO		3		/* VALVE State: Pulse Open	*/
#define	V_PC		4		/* VALVE State: Pulse Closed	*/
#define	V_S		5		/* VALVE State: Set		*/
#define	V_E		6		/* VALVE State: Error		*/
#define	A_PF		0x01		/* Alarm: Phase fail		*/
#define	A_OV		0x02		/* Alarm: Overload		*/
#define	A_PH		0x04		/* Alarm: Pressure High		*/
#define	A_LL		0x08		/* Alarm: Low Level Lockout	*/
#define	A_WL		0x10		/* Alarm: Water Low		*/
#define	A_PL		0x20		/* Alarm: Pressure Low		*/
#define	A_SM		0x0F		/* Alarm: Shutdown Mask		*/
#define	S_STA		1		/* Signal: Start Control Engine	*/
#define	S_RF		2		/* Signal: Reset Total Flow	*/
#define	S_RLL		3		/* Signal: Reset Low Level Lock	*/
#define	S_STO		4		/* Signal: Stop Control Engine	*/
#define	S_PT		5		/* Signal: Toggle Power LED	*/
#define	S_DEB		100		/* Signal: Debug		*/
#define	SEN_ERR		2

extern	bit	ui_flag;		/* User Interface flag		*/
extern	bit	db_flag;		/* Data Logger flag		*/
extern	byte	ce_run;			/* Public RUN flag		*/
extern	byte	ce_lll;			/* Low Level Lockout Flag	*/
extern	byte	ce_stat;		/* Control Engine state variable*/
extern	byte	ce_mode;		/* Control Engine mode variable	*/
extern	byte	ce_cos;			/* Control Engine ChangeOf State*/
extern	byte	ce_din;			/* Control Engine: Digital In	*/
extern	byte	ce_dout;		/* Control Engine: Digital Out	*/
extern	byte	v_stat;			/* Valve control state		*/
extern	byte	a_stat;			/* Alarm status			*/
extern	byte	v_set;			/* Desired valve position	*/
extern	byte	f_set;			/* Flow Setting			*/
extern	byte	l_set;			/* Low Level Setting		*/
extern	idata byte p_ave;		/* Average Pressure Reading	*/
extern	idata byte w_ave;		/* Average Water Reading 	*/
extern	idata byte flow;		/* Latest Flow Rate Calculation	*/
//extern byte	f_ave;			/* Average Flow Reading 	*/
extern	idata ulong f_tot;		/* Total Flow Reading 		*/
extern	byte	pressure;		/* Latest Pressure Calculation	*/
extern	byte	water_level;		/* Latest Level Calculation	*/
extern	time	ce_t0;			/* Control Engine Start Time	*/
extern	time	ce_tn;			/* Control Engine Latest Time	*/

void	init_ce		( void );	/* Initialise Control Engine	*/
void	ce_sig		( byte in );	/* Signal Control Engine	*/
bit	ce_sflow	( byte in );	/* Set Desired Flow		*/
bit	ce_sllev	( byte in );	/* Set Low Level		*/
void	calc_press	( void );	/* Update Pressure Variable	*/
void	calc_level	( void );	/* Update Level Variable	*/
void	calc_flow	( void );	/* Update Flow Variable		*/
