#pragma model=small

#include <stdio.h>
/* 80c552 Board with 4x4 keypad and 2-line 16 char display      */
/* Copyright (c) 1994  A Foster/E Coetzee for INFOTRANS cc      */
#define TRUE            1
#define FALSE           0
#define INSTR           0
#define DAT             1
#define RXBUFSIZE       20
#define DISPLEN         16
/************************************************************/
/* Defines for parallel display connected as follows        */
/* Note : data bus = 4-line mode                          */
/* P1.3...P1.0 = DB7...DB4     P4.7=E   P4.6=R/W   P4.5=RS  */
/************************************************************/
#define CLRDISP         0x01
#define RETHOME         0x02
/************************************************************/
#define EMSET           0x06    /* Cursor increments,       */
				/* no display shift         */
/************************************************************/
#define DISPCONTROL     0x08
#define DISPON          0x04    /* These 3 values should be */
#define CURSORON        0x02    /* OR'd with DISPCONTROL    */
#define BLINKON         0x01    /* as required              */
/************************************************************/
#define SHIFTCONTROL    0x10
#define DISPLAYSHIFT    0x00    /* These 4 values should be */
#define CURSORMOVE      0x08    /* OR'd with SHIFTCONTROL   */
#define SHIFTRIGHT      0x04    /* asrequired              */
#define SHIFTLEFT       0x00
/************************************************************/
#define FUNCSET         0x20
#define BITS8           0x10    /* These 3 values should be */
#define LINES2          0x08    /* OR'd with FUNCSET        */
#define DOTS10          0x04    /* as required              */
/************************************************************/
#define DDRAMSET        0x80    /* OR the DDRAM Address  */
				/* with this value          */
/************************************************************/
#define RS      P4.5
#define R_W     P4.6
#define E       P4.7
#define DBUS    P1
void initdisp();
void Print(char info, char mode, unsigned usecs);
void delay(unsigned int);
void printmessage(const char *p);
void do_messages(void);
char readkey(void);
void wait1(void);
void crlf(void);
void DisplayRXBuf();
void init4800(void);
void SetBaud(void);
void E_toggle(unsigned int deltim);
void check_busy(void);
char lookup(char i, char j);
/***********  Global variables  *****************************/
const char message1[] = "Infotrans cc";
const char message2[] = "Duynefontein" ;
const char message3[] = "Tel 021-553-4798";
const char message4[] = "Fax 021-553-1936";
const char message5[] = "TX/RX 4800,N,8,1";
const char message6[] = "Waiting...";
const char kbd[21]    = "123SO456LR789UD*0.EC";
static char idata RXbuffer[RXBUFSIZE];
volatile char idata rxidx;
static char idata dispidx;
static char idata baud;
static bit  cntrl;
/************************************************************/
void rxser ( ) interrupt S0  using 1
 {
 if(RI){
	RXbuffer[rxidx] = SBUF;
	RI = 0;
	if(++rxdx == RXBUFSIZE)        /* wrap buffer index  */
		rxidx=0;
       }
 }

void DisplayRXBuf()
  {
  char c1;
  while(dispidx != rxidx)
   {
   c1=RXbuffer[dispidx++];
   putchar(c1);
  /** Echo character to host  **/
  if(cntrl)
    {
    cntrl=FALSE;
    switch(c1)
      {
      case '0'    :
	Print(CLRDISP, INSTR, 0);  /* Clear & home */
	delay(1648);
	break;
      case '1'    :
	Print(RETHOME, INSTR, 0);     /* Cursor home */
	delay(1648);
	break;                        /* no clear    */
      case '3'    :
	Print(DISPCONTROL | DISPON | CURSORON, INSTR, 0);
      break;
      case '2'    :
	Print(DISPCONTROL | DISPON, INSTR, 0);
      break;  /** display ON, cursor OFF **/
      case '4'    :
	Print(DISPCONTROL | DISPON | CURSORON | BLINKON, INSTR, 0);
      break;
      case '5'    :
	Print(DISPCONTROL, INSTR, 0);
      break;  /** display OFF **/
      case 'b'    :
      case 'B'    :
	SetBaud();
      break;
      }
    } else
      {
      switch(c1)
	{
	case '^'     :
	  cntrl=TRUE;
	break;
	case 0x0a    :
	break;               /* ignore line feed */
	case 0x0d    :
	  crlf();              /* to next line     */
	break;
	default      :
	  if(c1 < ' ')
	  break;
	  Print(c1, DAT, 0);
	}
      } /** end if( cntrl ) else **/
    if( dispidx == RXBUFSIZE )       /* wrap buffer index  */
      dispidx = 0;
   }
  }

void SetBaud(void)
  {
  char br;

  while( dispidx==rxidx );
  br=RXbuffer[dispidx++];
  if( dispidx == RXBUFSIZE )       /* wrap buffer index  */
  dispidx = 0;
  printf("\r\nCONNECT ");
  switch(br)
    {
    case '0'  :
      printf("1200");
      wait1;
      PCON &= 0x7F;
      TH1=0xE8;
    break;
    case '1'  :
      printf("2400");
      wait1;
      PCON &= 0x7F;
      TH1=0xF4;
      break;
    case '2'  :
      printf("4800");
      wait1;
      PCON &= 0x7F;
      H1=0xFA;
    break;
    case '3'  :
      printf("9600");
      wait1;
      PCON |= 0x80;
      TH1=0xFA;
    break;
    default   :
    printf("\r\nInvalid parameter!");
    }
  printf("\n");
  }
  /************************************************************/
  /* Initialise serial port0 prior to use.                    */
  /*                              */
  /************************************************************/
  void init4800()
    {
    PCON &= 0x7F;  /** set to 4800 bps 11.0592 MHz clock  **/
    TH1   = 0xFA;
    }
  /************************************************************/
  /* Initialise LCD display prior to use.                     */
  /* written for 4-bit data bus only!                         */
  /************************************************************/
  void initdisp(void)
    {
    char ccode;
    E=0;                         /* pull control lines LOW */
    RS=0;
    R_W=0;
    delay(20000);                /* usec to delay (approx) */
    DBUS = (DBUS & 0xF0) | 0x03;
    E_toggle(4800);

    E_toggle(4800);

    E_toggle(4800);
   

    DBUS = (DBUS & 0xF0) | 0x02;

    E_toggle(4800);
   

    ccode = ( FUNCSET | LINES2 ); /* 4-bit mode, 2  lines */

    Print(ccode, INSTR, 0);     


    ccode = (EMSET);   

    Print(ccode, INSTR, 0);


    ccode = (DISPCONTROL | DISPON);

    Print(ccode, INSTR, 0);


    ccode = CLRDISP;             /* 0x01*/

    Print(ccode, INSTR, 0);

    delay(1648);
   

    /* end of reset operation */


    }    



  void E_toggle(unsigned int deltim)
    {        

    E=1;

    E=1;

    E=0;

    E=0;
   

    if(deltim)

      delay(deltim);

    else

      check_busy();


    }    



  void check_busy(void)

  {
  char BF;       


  RS=0;

  R_W=1;
   

  do
  {

  E=1;                 

  E=1;

  BF = (DBUS & 0x08);

  E=0;

  E=0;

  /* dummy read lo nibble */

  E=1;          

  E=1;            

  E=0;

  E=0;

  } while(BF);

  }





  /************************************************************/

  /* Print initial messages prior to executing main routine   */

  /*                                                          */

  /************************************************************/

  void do_messages(void)

    {

    printmessage(&message1[0]);

    crlf();

    printmessage(&message2[0]);

    wait1();

    printmessage(&message3[0]);

    crlf();

    printmessage(&message4[0]);

    wait1();

    printmessage(&message5[0]);

    crlf();

    printmessage(&message6[0]);

    Print(DISPCONTROL | CURSORON | DISPON | BLINKON, INSTR, 0);

    }         




    void crlf(void)

      {
      Print(DDRAMSET | 0x40, INSTR, 0);
       /** move cursor to row 2 col 0/
      }



    /************************************************************/
    /* Wait couple of seconds, then clear the display.          */
    /* (used for displaying sign-on screens)                    */
    /************************************************************/

    void wait1(void)

      {

      char n3;
   

      for(n3=0 ; n3<100 ; n3++)

	delay(30000);
   

      Print(CLRDISP, INSTR, 0);

      delay(1648);


      }


/*****************************************************************/

/* Module that scans a cross-bar switch keyboard, de-bounces it  *)
(* and translates the scan codes to ASCII                        *)
/*   The layout of the keyboard is as follows:                   */
/*         P4.3 -- 1 2 3 A B       A = RUN/STOP     B = OPEN         *)

(*         P4.2 -- 4 5 6 C D       C = LFT ARROW    D = RT ARROW     *)

(*         P4.1 -- 7 8 9 E F       E = UP ARROW     F = DN ARROW     *)

(*         P4.0 -- * 0 . G H       G = ENTER        H = CLOSE| | | | |
	   P5 bit  4 3 2 1 0
(* with the colomns connected to the low nibble of port P5 and   *)
(* the rows connected to the low  nibble of port P4.             *)
(*                                                               */
char readkey(void)
  {

  char i,j;
  

  P4 &= 0xF0;                   // All rows pulled down

  while((P5&0x1F)!=0x1F)        // wait until previous press released 

     DisplayRXBuf();
     

  for (i=0; i<=200; i++);       // remove possible bouncing
  

    while ((P5&0x1F)==0x1F)       // wait until pressed again

  DisplayRXBuf();

  for (i=0; i<=200; i++);     // again remove possible bouncing
  

			      // find out which key has been pressed 

  i=0x0E;

  while (i!=0x0F)
   {

   P4 &= 0xF0;

   P4 |= i;               /* ground next row to examine */

   j=(P5 & 0x1F);         /* only test bits 0..4  */


   if (j != 0x1F)
   {

     return (lookup(i, j));

   }

   i=i<<1 | 1;

   i&=0xF;

   }

   return 0;
   }
/************************************************************/

/* Look up the ASCII representation of the key pressed on   */

/* the 4 x 5 keyboard based on the value in the row/col     */
/* scan masks.	 */
/************************************************************/


char lookup(char i, char j)

   {

   char row, col;
   

   row = col = 0;


   if(i == 0x0F)

     return 0;       


   if(j == 0x1F)

     return 0;       
   

   i = ~i << 1;

   while(!(i & 0x10)){

   i = i << 1;

   row++;

   }        


   j = ~j << 1;

   while(!(j & 0x20)){

   j = j << 1;

   col++;

   }        

   return (kbd[row * 5 + col]);
   

   }
   



/************************************************************/
/* Print a char or control code to the LCD, then delay      */

/* by usecs or read Busy Flag if usecs=0                    */  

/* mode=instruction or data  (0 or 1)                       */
/************************************************************/

void Print(char info, char mode, unsigned usecs) 

{

R_W = 0;        

E=0;


if(mode)

RS = 1;

   else 

RS = 0;
 

DBUS = (DBUS & 0xF0) | ( info >> 4 );     /* do hi nibble  */

E=1;

E=1;

E=0;

E=0;
   

DBUS = (DBUS & 0xF0) | ( info & 0x0F);     /* do lo nibble  */


E_toggle(0);         /* check busy now */

}



/************************************************************/

/* wait for approximately 'delayval' microseconds           */
/*						   */
/************************************************************/


void delay(unsigned int delayval)

  {                      

  unsigned int loop1;
              

  for(loop1=(delayval>>4) ; loop1 > 0 ; loop1--);

  }                




/************************************************************/
/*  Print a message string up to a length of 16 chars       */
/*                                                          */

/************************************************************/

void printmessage(const char *p)

 {     

 char n1;             


 for(n1=0 ; n1 < 16 ; n1++)
   {

   if(*p==0) return();
        

   Print(*p++, DAT, 0);

   }
 
}               



/************************************************************/

/*                                                          */

/*                                                          */

/************************************************************/

void main(void)

{

char c;
  

rxidx=0;

dispidx=0;

cntrl=0;


initdisp();

do_messages();


fopen(stdin); /**  default = 9600,N,8,1 **/

init4800();   /**  4800,N,8,1  **/
  

ES0 = TRUE;    /* enable serial interrupt  */

EA = TRUE;     /* enable general interrupt */

TI = TRUE;
  

printf("Reset OK!\n");
  

do
  {                 
  

  c=readkey();

  if(c) 

    putchar(c);
  

  }while(TRUE);

}

