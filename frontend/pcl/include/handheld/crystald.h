/*#define CD 							0X01	/** Clear display ram **/*/
#define CLEARDISP				0X01	/** Clear display ram **/
#define RTNHOME 				~0X03	/** Returns cursor to home **/
#define EMSET						4   	/** Entry mode is set Increment by 1 **/
#define DISPOFF					0X08	/** Display control  8 == on, 4 cursor off, char !flash **/
#define DISPON					0X0F 	/** Display control  8 == on, 4 cursor off, char flash **/
#define RESET_DISP			0X30	/** Reset the display to original state **/
#define EM_SHIFT_CUR_L	0x04  /** Entry mode move cursor to left **/
#define EM_SHIFT_CUR_R	0X06  /** Entry mode move cursor to right **/
#define EM_SHIFT_SCR_L	0X05   /** Entry mode move screen to left **/
#define EM_SHIFT_SCR_R	0X07  /** Entry mode move screen to left **/
#define FUNC_SET				0X38	/** || 0x30 Set interface to 8 bits
															|| 0x08 Sets to 2 lines
									dat bits 			2^3	2^2
															--------------------------
															| N  | F  | No. of lines |
															-------------------------
															| 0  | 1  |  1 line      |
															--------------------------
															| 1  | X  |  2 lines     |
															--------------------------
													**/

#define CURLEFT				0X10		/** Move the cursor left **/
#define CURRIGHT			0X14		/** Move the cursor right **/
#define SCREENLEFT		0X18		/** Move the screen left **/
#define SCREENRIGHT		0X1C		/** Move the screen right **/
#define R0W						0x00		/** R = 0, R/W = 0, E = 0 Write to register **/
#define R1W						0x02		/** R = 1, R/W = 0, E = 0 Write to display ram **/

#define CR						0x310 	/** Select control register hardware add **/
#define DR						0x311		/** Select display data register hardware add **/

#define E						0x04 		/** Set E line to 5 V **/
#define e						0xFB 		/** ! E **/

#define CLR_DISP		Outport_Cbyte ( CLEARDISP);	Outport_Cbyte ( RTNHOME);
#define RTN_HOME		Outport_Cbyte ( RTNHOME);

#define DISP_OFF		Outport_Cbyte ( DISPOFF); Outport_Cbyte ( DISPOFF);
#define DISP_ON			Outport_Cbyte ( DISPON); Outport_Cbyte ( DISPON);

#define WAIT1				delay(1); /** wait 1 ms **/