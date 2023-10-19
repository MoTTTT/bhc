typedef unsigned char Byte;		/* define Byte type */
typedef unsigned int  Int;		/* define Int type */
typedef unsigned long Long;		/* define Long type */

#define LOOPS_TOTAL 100
#define DIG 	100/*13650*/
#define ADC 	100/*65520*/
#define SPEED 100/*2730*/

#define TIME_BUFFER 5779
#define COUNTSPERLITER 1024
#define COUNTSPERKILO  104

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

#define  ADLINE         10		/* A/D first display column */
#define  DALINE         27              /* D/A first display column */
#define  DAUNIT         34              /* D/A unit display column */
#define  DILINE1        47              /* DI first line display column */
#define  DILINE2        55              /* DI second line display column */
#define  DOLINE1        67              /* DO first line display column */
#define  DOLINE2        75              /* DO second line display column */

#define  TIMAX          100

enum { Jan=1 ,Feb=2 ,Mar=3 ,Apr=4 ,May=5 ,Jun=6 ,Jul=7 ,Aug=8 ,Sep=9 ,Oct=10 ,Nov=11 ,Dec=12};
