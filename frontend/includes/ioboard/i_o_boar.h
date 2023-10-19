/******************************************************************************/
/**                                                                          **/
/**  Define for intel 8253 programable counter                               **/
/**    and for I_O_BOARD                                                     **/
/**                                                                          **/
/**    TIMER1 = the base address                                             **/
/**    TIMER2 = the base address                                             **/
/**                                                                          **/
/**    CNT0   = load counter 0                                               **/
/**    CNT1   = load counter 1                                               **/
/**    CNT2   = load counter 2                                               **/
/**    CR     = Controle Register                                            **/
/**                                                                          **/
/**    MODE0  O/P = 0 at end of count                                        **/
/**    MODE1  Programable one shot                                           **/
/**           O/P = 0 while counting                                         **/
/**    MODE2  Puls generator                                                 **/
/**    MODE3  Square wave generator                                          **/
/**    MODE4  O/P = 1 and changes to 0 on terminal count                     **/
/**    MODE5  O/P = 1 and changes to 0 on teminal count                      **/
/**                                                                          **/
/**    SLC0   Select counter 0                                               **/
/**    SLC1   Select counter 1                                               **/
/**    SLC2   Select counter 2                                               **/
/**                                                                          **/
/**    CNLATC Latches the counter                                            **/
/**                                                                          **/
/**    RDLDLBO Read/Load lsd byte only                                       **/
/**    RDLDMBO Read/Load msb byte only                                       **/
/**    RDLDLS1 Read/Load lsb first                                           **/
/**                                                                          **/
/**    BCD      Binary Coded Decimal                                         **/
/**    BIN      Binary                                                       **/
/**                                                                          **/
/**    E. Coetzee                                                            **/
/**                                                                          **/
/**    13 - 01 - 1992                                                        **/
/******************************************************************************/

#define TIMER1_CNT0 TIMER+0; /**  Off set for the Counter register 0          **/
#define TIMER1_CNT1 TIMER+1; /**  Off set for the Counter register 1          **/
#define TIMER1_CNT2 TIMER+2; /**  Off set for the Counter register 2          **/

#define TIMER1_CR   TIMER+3; /**  Off set for the Controle register           **/

#define  TIMER1     0x318;   /**  = the base address                          **/
#define  TIMER2     0x31c;   /**  = the base address                          **/

#define  CNT0       0x00;    /**  = load counter 0                            **/
#define  CNT1       0x01;    /**  = load counter 1                            **/
#define  CNT2       0x02;    /**  = load counter 2                            **/

#define  TCR         0x03;    /**  = Controle Register                         **/

#define  MODE0      0x00;    /**  O/P = 0 at end of count                     **/
#define  MODE1      0x02;    /**  Programable one shot O/P = 0 while counting **/
#define  MODE2      0x04;    /**  Puls generator                              **/
#define  MODE3      0x06;    /**  Square wave generator                       **/
#define  MODE4      0x08;    /**  O/P = 1 and changes to 0 on terminal count  **/
#define  MODE5      0x0a;    /**  O/P = 1 and changes to 0 on teminal count   **/

#define  SLC0       0x00;    /**  Select counter 0                            **/
#define  SLC1       0x04;    /**  Select counter 1                            **/
#define  SLC2       0x80;    /**  Select counter 2                            **/

#define  CNLATC     0x00;    /**  Latches the counter                         **/

#define  RDLDLBO    0x00;    /**  Read/Load lsd byte only                     **/
#define  RDLDMBO    0x01;    /**  Read/Load msb byte only                     **/
#define  RDLDLS1    0x02;    /**  Read/Load lsb first                         **/

#define  BCD        0x01;    /** Binary Coded Decimal                         **/
#define  BIN        0x00;    /** Binary                                       **/

/**  Defines for the I/O    **/

#define  BYTE_I_0   0x310;    /** Addres for input byte 0                      **/
#define  BYTE_I_1   0x311;    /** Addres for input byte 1                      **/
#define  BYTE_I_2   0x312;    /** Addres for input byte 2                      **/
#define  BYTE_I_3   0x313;    /** Addres for input byte 3                      **/
#define  BYTE_I_4   0x314;    /** Addres for input byte 4                      **/
#define  BYTE_I_5   0x315;    /** Addres for input byte 5                      **/

#define  BYTE_O_0   0x310;    /** Addres for output byte 0                     **/
#define  BYTE_O_1   0x311;    /** Addres for output byte 1                     **/
#define  BYTE_O_2   0x312;    /** Addres for output byte 2                     **/
#define  BYTE_O_3   0x313;    /** Addres for output byte 3                     **/

#define Loops 4;

#define IO_BASE	0x300		/** ADC1  BASE REGISTER **/

#define ADC0	0x300			/** ADC1  BASE REGISTER **/
#define ADC1	ADC+0			/** ADC2  BASE REGISTER **/
#define ADC2  ADC+1			/** ADC3  BASE REGISTER **/
#define ADC3	ADC+2			/** ADC4  BASE REGISTER **/
#define ADC4	ADC+3			/** ADC5  BASE REGISTER **/
#define ADC5	ADC+4			/** ADC6  BASE REGISTER **/
#define ADC6	ADC+5			/** ADC7  BASE REGISTER **/
#define ADC7	ADC+6			/** ADC8  BASE REGISTER **/
#define ADC8	ADC+7			/** ADC9  BASE REGISTER **/
#define ADC9	ADC+8			/** ADC10 BASE REGISTER **/
#define ADC10	ADC+9			/** ADC11 BASE REGISTER **/
#define ADC11	ADC+10		/** ADC12 BASE REGISTER **/
#define ADC12	ADC+11		/** ADC13 BASE REGISTER **/
#define ADC13	ADC+12		/** ADC14 BASE REGISTER **/
#define ADC14	ADC+13		/** ADC15 BASE REGISTER **/
#define ADC15	ADC+14		/** ADC16 BASE REGISTER **/

#define OUTPUT1 0x310				/** OUTPUT1 BASE REGISTER **/
#define OUTPUT2 0x311				/** OUTPUT2 BASE REGISTER **/
#define OUTPUT3 0x312				/** OUTPUT3 BASE REGISTER **/
#define OUTPUT4 0x313				/** OUTPUT4 BASE REGISTER **/

#define INPUT1 0x310 				/** INPUT1 BASE REGISTER **/
#define INPUT2 0x311 				/** INPUT2 BASE REGISTER **/
#define INPUT3 0x312 				/** INPUT3 BASE REGISTER **/
#define INPUT4 0x313 				/** INPUT4 BASE REGISTER **/
#define INPUT5 0x314 				/** INPUT5 BASE REGISTER **/
#define INPUT6 0x315 				/** INPUT6 BASE REGISTER **/

#define TIMER1LCR0			0x310	/** Load / read couter 0 **/
#define TIMER1LCR1      0x311	/** Load / read couter 1 **/
#define TIMER1LCR2      0x312	/** Load / read couter 2 **/
#define TIMER1LCRWORD   0x313	/** Write controle word to timer 1 **/

#define TIMER2LCR0			0x310	/** Load / read couter 0 **/
#define TIMER2LCR1      0x311	/** Load / read couter 1 **/
#define TIMER2LCR2      0x312	/** Load / read couter 2 **/
#define TIMER2LCRWORD   0x313	/** Write controle word to timer 1 **/

#define COUNTERLATCH	0x00
#define RLLSBO				0x10
#define RLMSBO				0x20
#define RLLSBF				0x30

#define SC0   				0x00
#define SC1   				0x40
#define SC2   				0x80

/** I/O PORT DEFINES**/

#define O_P_1         0
#define I_P_1         0
#define OK            0


/** SPEACH MESSAGE DEFINITIONS **/

#define  NO            0
#define  ALC           0
#define  RAND          0
#define  MODE          0
#define  IMOB          0
#define  BEEP          0
#define  SELECTLOW     0
#define  RANDOM        0
/*#define  STARS         0*/

