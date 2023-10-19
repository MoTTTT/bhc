/***************************************************************************/
/**                                                                       **/
/**  Set up varaibles                                                     **/
/**                                                                       **/
/***************************************************************************/

#define ADC_PRES 8    /** Adc present **/
#define DIG_PRES 8    /** Dig present (groups of 8) **/

/***************************************************************************/
/**  End of the set_up vairables                                          **/
/***************************************************************************/
typedef unsigned char Byte;		/* define Byte type */
typedef unsigned long Long;		/* define unsigned Long */
typedef unsigned int  Int;		/* define unsigned Long */

/** Select counter 1 and set the timer counter **/
#define c1MSb 0xff
#define c1LSb 0xff

/** Select counter 2 and set the rpm **/
#define c2MSb 0x1
#define c2LSb 0x1

/** Select counter 3 and set the .1 km **/
#define c3MSb 0x1
#define c3LSb 0x1

/** Select counter 4 and set the .1 liter used **/
#define c4MSb 0x01
#define c4LSb 0xff

/** Select counter 5 and set the .1 liter return **/
#define c5MSb 0x1
#define c5LSb 0x1

/** Select counter 6 and set the kmh **/
#define c6MSb 0x1
#define c6LSb 0x1


#define COUNTSPERLITER 1024
#define COUNTSPERKILO  1024

#define MEMORY 65535
#define TX_BUFFER  4096
#define RX_BUFFER  4096
#define LOOP_COUNTER 1926   /* calc by mem / (DIG_BUF + ADC +TIME) */

#define ZERO		0x00
#define CTRL_B	0x02
#define CTRL_D	0x04
#define CTRL_E	0x05
#define CTRL_H	0x08
#define CTRL_I	0x09

/* key definition */
#define  UPARR          0x4800
#define  DNARR          0x5000
#define  ESC            0x11b
#define  RTARR          0x4d00
#define  LTARR          0x4b00
#define  PGUP           0x4900
#define  PGDN           0x5100
#define  HOME           0x4700
#define  ENTER          0x1c0d
#define  BACKSP         0x0e08
#define  F1             0x3b00
#define  F10            0x4400

#define  ON             '1'
#define  OFF            '0'
#define  SETUP          1 		/* setup window command */
#define  RECOVER        2               /* erase window command */

#define  TRIGGER        0x01            /* A/D software trigger command */

/** Time delays for car disabeling **/

#define Random          10    /** in seconds after random test failure **/
#define Alchol          10    /** in seconds after alchol test failure **/

#define  TIMAX          100

struct vechile_his {
	Long trip_km;
	Long vech_km;
	Int  cnts_speed;
	Int  cnts_liter;
	Int  cnts_kilo;
	Byte adc;
	Byte dig_output;
	Byte dig_input;
} *p_vech;

struct   specific{
         int base;              /* base address */
         int resolute;          /* D/A & A/D resolution */
         int adno;              /* A/D channel number */
         int dano;              /* D/A channel number */
         int dino;              /* D/I channel number */
         int dono;              /* D/O channel number */
         char trig;             /* A/D trigger port address */
         char adrdy;            /* A/D ready check Byte */
         int defad;             /* A/D default range */
         int defadh;            /* A/D default high value */
         int defadl;            /* A/D default low value */
         int defda;             /* D/A default range */
         int defdah;            /* D/A default high value */
         int defdal;            /* D/A default low value */
				 }feature={
0x220, 4096, 8, 1, 16, 16,12,0x10,2, 5,-5,3, 5, 0};

	enum { Jan=1 ,Feb=2 ,Mar=3 ,Apr=4 ,May=5 ,Jun=6 ,Jul=7 ,Aug=8 ,Sep=9 ,Oct=10 ,Nov=11 ,Dec=12};
	enum { CLOSEWINDOW=1, SELECTPIN=2,ALCHOLTEST=3,RANDOMTEST=4,TESTFAILED=5,CAROUT=6,SELECTLOW=7,EXCEEDSPEED=8};
/*****************************global variable********************************/

	Byte Adc_p;   /** Input analogs for vectron **/
	Byte Dig_i_p; /** Digital inputs for vectron **/
	Byte Dig_o_p; /** Digital outputs for vectron **/

	int temp1=0,temp2=0,temp3,crc_error_p1,crc_error_p2,crc_good=0;
	Byte crc_error=0,ctrl_abyte=0,ctrl_ibyte=0, tx_buffer[1024], rx_buffer[1024],vechile_speed;
	Byte maks_speed = 90 ,speed=0;
	char temp_buf[1024];
	Byte tmp_min=0 ,day=1 ,caron=0 ,adc_counter = 0;
	char *tyd;
	FILE *fp1, *fp2, *fp3;

	Int  adc_buffer_counter    = 0;
	Int  time_buffer_counter   = 0;
	Int  dig_buffer_counter    = 0;
	Int  speed_buffer_counter  = 0;
	Byte or_bit[8];

	int  tx_buf_counter = 0;
	int  rx_buf_counter = 0;
	int  tx_counter_tmp = 0;
	int  rx_counter_tmp = 0;
	int  select_port;
	int  crc_error_p2;
	long rpm , rpm_set = 500;  /** long's for rpm **/

	long vech_km;       /** long   for vech km **/
	long tmp_vechkm;    /** counter for pulses from wheels **/
	long vechkm_set;    /** counts per km  **/
	long trip_km;  		  /** counts per km  **/

	long trip_ltr;      /** long   for trip liters **/
	long tmp_liter;     /** counter for pulses from wheels **/
	long liter_set;     /** counts per km  **/

/** adc string buffers **/

	Byte adc_min[8];     /** min for cycle time **/
	Byte adc_maks[8];    /** maks for cycle time **/
	Byte adc_lv_maks[8]; /** maks set point **/
	Byte adc_lv_min[8];  /** min  set point **/

	Byte dig_io[6];     /** digital i/o buffer**/

	Byte dig_string[8];

	Byte bad_d, bad_s, bad_r;
	Byte *dig_buffer, *adc_buffer, *time_buffer, *speed_buffer ,*event_buffer ,*rpm_buffer;
	char *mem_spares[11];

	char rdriver[7][9];		/**  Driver 1 id received **/

	time_t Lt;

struct vechile_data {
	char box_no[9];
	Byte Adc_maks[8];
	Byte Adc_min[8];
	Byte Speed;
	char driveri1[9]; 	/**  Driver 1 id  								**/
	char driveri2[9]; 	/**  Driver 2 id  								**/
	char driveri3[9]; 	/**  Driver 3 id  								**/
	char driveri4[9]; 	/**  Driver 4 id  								**/
	char driveri5[9]; 	/**  Driver 5 id  								**/
	char driveri6[9];  	/**  Driver 6 id									**/
	char driveri7[9];  	/**  Driver 7 id									**/
	char doc_no[12];	  /**  document number							**/
	Byte Fuel_enter;    /** Fuel entered at base          **/
	char tr_no1[9];		/**  Trailer ID 1	  							**/
	char tr_no2[9];		/**  Trailer ID 1	  							**/
	char direction[1];	/**  Direction of vechile 				**/
	Int  trip_km;				/**  trip kilometer 							**/
	Long vechile_km;		/**  Vehicle speedometer reading 	**/
	Int trip_fuel[9]; 	/**  Fuel used										**/
}t_vech_data;

struct driver {
	char time[21];
	char driveri[14];	/**  Driver **/
	char driverO[12];	/**  Driver **/
	char drivero[12];	/**  Driver **/
	char fuel[9];
	char trip_km[9];
}signon;

struct tyd {
    Int year;
    Byte month;
    Byte day;
    Byte hour;
    Byte min;
    Byte sec;
}my_time;

struct stm {
	short tm_sec;
	short tm_min;
	short tm_hour;
	short tm_mday;
	short tm_mon;
	short tm_year;
	short tm_wday;
	short tm_yday;
	short tm_isdst;
	short tm_hsec;
}mytime;

Byte Bits[9];
Byte or_bits[9];

/** Select counter 1 and set the timer counter **/
Byte C1MSb = c1MSb;
Byte C1LSb = c1LSb;

/** Select counter 2 and set the rpm **/
Byte C2MSb = c2MSb;
Byte C2LSb = c2LSb;

/** Select counter 3 and set the .1 km **/
Byte C3MSb = c3MSb;
Byte C3LSb = c3LSb;

/** Select counter 4 and set the .1 liter used **/
Byte C4MSb = c4MSb;
Byte C4LSb = c4LSb;

/** Select counter 5 and set the .1 liter return **/
Byte C5MSb = c5MSb;
Byte C5LSb = c5LSb;

/** Select counter 6 and set the kmh **/
Byte C6MSb = c6MSb;
Byte C6LSb = c6LSb;

Byte Adc_buffer   =  16;
Byte Speed_buffer =  16;

Long Total_buf;
Long	size_adc_buf;
Long	size_speed_buf;
long	size_event_buf;
Long	size_rpm_buf;

/************************* end of global variable *****************************/
