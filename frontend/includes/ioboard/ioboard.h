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

#define TIMER_BASE1 0x318 /** TIMER1 BASE REGISTER **/
#define TIMER_BASE2 0x31C /** TIMER2 BASE REGISTER **/

#define TIMER1LCR0			0x318	/** Load / read couter 0 **/
#define TIMER1LCR1      0x319	/** Load / read couter 1 **/
#define TIMER1LCR2      0x31A	/** Load / read couter 2 **/
#define TIMER1LCRWORD   0x31B	/** Write controle word to timer 1 **/

#define TIMER2LCR0			0x31C	/** Load / read couter 0 **/
#define TIMER2LCR1      0x31D	/** Load / read couter 1 **/
#define TIMER2LCR2      0x31E	/** Load / read couter 2 **/
#define TIMER2LCRWORD   0x31F	/** Write controle word to timer 1 **/

#define MODE0	0x00		/** Go low on terminal count **/
#define MODE1 0x02		/** Programable 1 shot **/
#define MODE2 0x04		/** Square wave gen 0 for 1 cycle on terminal count **/
#define MODE3 0x06		/** Square wave gen
															if count is  even count o/p high for n/2
															if count is  odd  count o/p high for (n+1)/2
																							and low for (n+1)/2 counts **/
#define MODE4 0x08		/** o/p normally 1 goes low on terminal count **/
#define MODE5 0x0A		/** o/p normally 1 goes low on terminal count,
															counting is enabled on rising edge  **/

#define BIN		0x00
#define BCD		0x01

#define COUNTERLATCH	0x00
#define RLLSBO				0x10
#define RLMSBO				0x20
#define RLLSBF				0x30

#define SC0   				0x00
#define SC1   				0x40
#define SC2   				0x80
