/*			Copyright Q Solutions				*/
/*	File:		mesg.c						*/
/*	Programmer:	MoT						*/
/*	Project:	Borehole Controller				*/
/*	Module:		User Interface Message Strings			*/
/*									*/
/*			History						*/
/* 20:31 27/04/1997  	(ui.c) Extracted from main bhc.c 		*/
/* 10:34am 07-01-1997 	(mesg.c) Extracted from ui.c 			*/
/*									*/

/*			Refresh and Sign on strings			*/
const char *clear=   "                ";/* Blank Line			*/
const char *rso1=    "Borehole  Master";/* Line 1 of remote sign on 	*/
const char *rso2=    "   Controller   ";/* Line 2 of remote sign on	*/
/*			Menu States and strings				*/
const char *s_menu=  "\x7e and \x7f for Menu";	/* Menu: Prompt		*/
const char *s_enter= "ENTER to Select ";/* Mene: Menu Help		*/
const char *s_stime= "    Set Time    ";/* Menu: Set Time		*/
const char *s_ptime= " Time: hh:mm:ss ";/* Time framework		*/
const char *s_tset=  "    Time Set    ";/* Menu: Set Time		*/
const char *s_sflow= "    Set Flow    ";/* Menu: Set Flow		*/
const char *s_pflow= " Flow:     l/s  ";/* Menu: Set Flow Prompt	*/
const char *s_fset=  "    Flow Set    ";/* Menu: Flow Set		*/
const char *s_rlll=  "   Reset LLL    ";/* Menu: Reset Low Level Lockout*/
const char *s_rstng= " Resetting LLL  ";/* Menu: Reset Low Level Lockout*/
const char *s_nrst=  " Level Still Low";/* Menu: Reset Low Level Lockout*/
const char *s_sll=   " Set Low Level  ";/* Menu: Set Low Level 		*/
const char *s_pll=   " Low Level:    m";/* Water Level framework	*/
const char *s_lset=  " Low Level Set  ";/* Menu: Low Level Set		*/
const char *s_stop=  " Stop Borehole  ";/* Menu: Start/Stop Prompt:Stop	*/
const char *s_stpng= "    Stopping    ";/* Status: Stop pressed		*/
const char *s_start= " Start Borehole ";/* Menu: Start/Stop Prompt:Start*/
const char *s_rning= "    Starting    ";/* Status: Start pressed	*/
const char *s_nrng=  "System Shut Down";/* Status: Start pressed, errors*/
const char *s_noff=  "   System Off   ";/* Status: Start pressed: ce off*/
const char *s_nman=  "Manual Override ";/* Status: Start pressed manual	*/
const char *s_nrll=  " System in LLL  ";/* Status: Start pressed, lll	*/
const char *s_oor=   "  Out Of Range  ";/* Menu: Input out of range	*/
/*			Info Strings					*/
const char *s_flow=  "Flow:           ";/* Flow framework		*/
const char *s_pres=  "Pressure:       ";/* Pressure framework		*/
const char *s_wl=    "Water Level:    ";/* Water Level framework	*/
const char *s_time=  "Time:           ";/* Time framework		*/
const char *s_stat=  "Status:         ";/* Status framework		*/
