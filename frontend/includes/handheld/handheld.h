/** ********************************************************************* **

		CRYSTALD.H

		The include file for the liquid display types ( manufactured by Hitachi )

			RS - 585-084
			RS - 585-090
			RS - 585-107
			RS - 585-113
			RS - 585-129

		Version - 0
		Date		- 12/04/92
		By      - E. Coetzee

** ********************************************************************* **/

/** ********************************************************************* **

		Controle charachters -

				These charchters will vary depending the digital connection
** ********************************************************************* **/

#define GOLD_BERG

#define CLEARDISP				0X01	/** Clear display ram **/
#define RTNHOME 				0X03	/** Returns cursor to home **/
#define EMSET						4   	/** Entry mode is set Increment by 1 **/
#define DISPOFF					0X08	/** Display control  8 == on, 4 cursor off, char !flash **/
#define DISPON					0X0D 	/** Display control  8 == on, 4 cursor off, char flash **/
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
#define E						0x04 			/** Set E line to 5 V **/
#define e						0xFB 			/** ! E **/

#define COLMS_LOW			0X8c			/** set all rows low noy used in version 0**/
#define KBD_LOOPS 		1600			/** set all rows low **/
#define KBD_COLOMS 		3					/** defining the amount of coloms **/
#define KBD_ROWS	 		4					/** defining the amount of rows **/


/** ********************************************************************* **

		Register addresses -

				These addresses will vary depending the digital connection
** ********************************************************************* **/

#ifndef GOLD_BERG
/** This is valid for DENIS **/
#define CR						784   	/** Select control register hardware add **/
#define DR						0x311	/** Select display data register hardware add **/
#else
#define CR						0x310 	/** Select control register hardware add **/
#define DR						0x311		/** Select display data register hardware add **/
#endif
#define KBD_INPUT 		0X314		/** data from keyboard **/


/** ********************************************************************* **

		Some common MAKROS -

** ********************************************************************* **/

#define CLR_DISP		Outport_Cbyte ( CLEARDISP);	Outport_Cbyte ( RTNHOME);
#define DISP_OFF		Outport_Cbyte ( DISPOFF); Outport_Cbyte ( DISPOFF);
#define DISP_ON			Outport_Cbyte ( DISPON); Outport_Cbyte ( DISPON);
#define WAIT1				delay(0); /** wait 1 ms **/
/** *********************************************************************

		definning strings

 ** ********************************************************************* **/

/**                   |                | last char on a row **/
#define ALC_TEST			"Alcohol test      blow in pipe"
#define DRIVE_OFF			"Drive off        "
#define FORWARD_KEY		"Push #           "
#define ILLIGAL_ENTRY	"Illegal entry    "
#define INIT_BOX			"Init. controller "
#define MEM_ERROR			"Memory error     "
#define MINUTES				"Minutes          "
#define NOBOXNO       "No Param. passed    to box !!!!!"
#define PIN_CODE			"Enter PIN code -    ********"
#define PUSHSTAR      "Push '*'         "
#define REPLACE_DISK	"Replace disk     "
#define START					"Starting main    "
#define START_CAR			"Start vehicle    "
#define TEST_OK				"Test passed      "
#define VECH_DISABLE	"Vehicle disabled  for"
#define VECH_SPEED		"Veh.  speed =    "
#define RX_WAIT				"Wait for RX      "
#define RX_RI 				"Wait for ring ind"
#define ILLIGAL_ENTRY	"Illegal entry    "
#define INIT_BOX			"Init. controller "
#define MEM_ERROR			"Memory error     "
#define MINUTES				"Minutes          "
#define REPLACE_DISK	"Replace disk     "
#define STARS					"     *****"
#define START					"Starting main    "
#define START_CAR			"Start vehicle    "
#define TEST_OK				"Test passed      "
#define VECH_SPEED		"Veh.  speed =    "
#define RX_WAIT				"Wait for RX      "
#define WAIT_INI			"Wait for Init.   from base."

#define DOWN_LOAD     "Down load in progress"
