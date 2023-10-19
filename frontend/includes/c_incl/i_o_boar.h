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

#define IO_BASE	0x300		/** ADC1  BASE REGISTER **/

#define Loops 4;