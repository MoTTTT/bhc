#pragma la
#pragma maxargs (4)

#define STATION "toto"
#define Flows 5
#define MaksFlow 10

/** **************************************************************** **/
/** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! **/
/**                                                                  **/
/**   This product with all documentation is the property of         **/
/**                                                                  **/
/**                    INFOTRANS CC                                  **/
/**                                                                  **/
/**   This product with all documentation and associated software    **/
/**      on Programmable Read Only Memoery ( PROM) should not be     **/
/**      copied, duplicated by any means without the permission      **/
/**      or shown to any body without the written permission         **/
/**      of the owner(s) INFOTRANS.                                  **/
/**                                                                  **/
/** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! **/
/** **************************************************************** **/
#include <c:\c51v4\inc\stdio.h>
#include <c:\c51v4\inc\math.h>
#include <c:\c51v4\inc\string.h>
#include <c:\c51v4\inc\stdlib.h>
#include <c:\c51v4\inc\intrins.h>
#include <c:\c51v4\inc\ctype.h>
#include <c:\c51v4\inc\reg552.h>

#define Test

#define T2tes
#define TRUE  1
#define DAYS  1
#define FALSE 0
#define DEBUG
#define NoLockOut  0xFE   /** & byte to stop lock out pump **/
#define LockOut    0x01   /** | byte to do lock out **/
#define PumpOFF    0xFD   /** & byte to switch pump off **/
#define PumpON     0x02   /** | byte to switch pump on **/
#define ValveClose 0x04   /** | byte to Close valve **/
#define ValveOpen  0x08   /** | byte to open valve **/
#define NoValve   ~0x0c   /** & byte to disable  valve control **/
#define ON         100
#define OFF          0
#define ClosePos    10     /** minimum value for valve closure  */

#define XRAM 0xffff

struct Clock
  {
  int sec;
  int hour;
  int min;
  int day;
  int month;
  int  year;
  };

struct mrec          /** measured record **/
  {
  int hour;
  int min;
  char Ip;
  int avlevel[8];
  float flow;
  float Tflow;
  };

/** **************************************************************** **/
/**                                                                  **/
/**   Global definitions                                             **/
/**                                                                  **/
/** **************************************************************** **/

char xdata CR _at_ 0x000b;
char xdata IP _at_ 0x0009;
char xdata OP _at_ 0x0008;

char bdata DigoutGB, TGB;
static char bdata ModeGC;
static int   xdata  SecGI, mSecGI,PhaseCountGI, LowlevelGI;
static int   xdata TGI=0;
int   bdata  CycleGI=0, LoopsGI,CounterGI, LoopiesGI=0, ValvectGI;

int   xdata  AdcGI, LdispGI=0, LdispindGI=0, FlowTestGI=0;
int   xdata  LevelGI, LevelMGI;
float xdata  FlowSetPointGF, MaksFlowSetPointGF;
float xdata  FlowGF, TFlowGF;

char  xdata  TxBufGCA[251],RxBufGCA[251], TirqGCA[100];
char  xdata  AdcMinGIA[8]= { 5,5,5,5,5,5,5,5};
float  xdata  ADCconv[] = { 25,100,1000, 0, 0, 0, 0, 0};
float xdata  ADCmaks[] = { 250,250,250,250,250,250,250,250};
char  xdata  *KarC;
float xdata  Tf1,TF2,TF3, TGF;
unsigned int xdata TI1, TI2,TI3;

xdata struct mrec MrecGS[60*24*DAYS];
xdata struct Clock CurrentTimeSG;

void Putchar (char Karakter)
  {
/** wait for tx buf to clear  **/

  while (!TI);
  S0BUF = Karakter; /* output CR  */
  while (!TI);
  TI=0;
  }

#ifdef PIA
InitPIA()
  {
  P1 &= ~2;      /** select i/o device **/
  CR = 0x82;    /** set 8255 port A = o/p Portb b = i/p **/
  OP = 0;
  P1 |= 2;    /** de select i/o device **/
  }
#endif

void Puts ( )
  {
  char *KarC;

  KarC = &TxBufGCA;
  P1 &= 0xef;         /** set 5 bit enable TX**/
  do
    {
    Putchar(*KarC);
    }while( *KarC++ != '~');
  P1 |= 0x10;         /** clear 5 bit disable TX**/
  }

void Display ( )
 {
 LdispindGI=1;      /** stop re-entry  **/
 memset( TxBufGCA, 0, sizeof( TxBufGCA));
 TI1   = (int) MrecGS[LoopsGI].avlevel[0];
 TGF   = (float) ( TI1 - AdcMinGIA[0]);
 Tf1   = TGF / 250;
 Tf1   = Tf1 * 25;

 TI2   = (int) MrecGS[LoopsGI].avlevel[1];
 TGF   = (float) ( TI1 - AdcMinGIA[1]);
 TF2   = TGF / 250;
 TF2   = TF2 * 100;

 TI3   = (int) MrecGS[LoopsGI].avlevel[2];
 TGF   = (float) ( TI3 - AdcMinGIA[2]);
 TF3   = TGF / 250;
 TF3   = TF3 * 1000;

 sprintf( TxBufGCA ,"  ^0Fl %2.0fLs Lev %4.1f M   \r", MrecGS[LoopsGI].flow, Tf1);
 sprintf( TirqGCA, "VP %3.0fp %4.0f Kpa  ~", TF2, TF3  );
 strcat ( TxBufGCA, TirqGCA);
 Puts();

 }

/** Send end of command Line !!! **/
void Endofcommand()
  {
  memset( TxBufGCA,0,sizeof(TxBufGCA));
  strcpy ( TxBufGCA, "UUUUU!&~");
  Puts();
  }

void Nocommand()
  {
  memset( TxBufGCA,0,sizeof(TxBufGCA));
  strcpy ( TxBufGCA, "UUUUU^&~");
  Puts();
  }


/** Adc conversion routine **/

int adconV ( unsigned char adcUC)
 {

 ADCON = 0;       /** strip previous ADC and Select SOFTW start**/
 ADCON = adcUC;   /** Select channel **/
 ADCON |=  0x8;   /** Start ADC **/
/** Whait for ADC to end **/
 while ( (ADCON &0x10) != 0x10);
 return ( (int) ADCH);
 }

/** Close the actuator  **/
closeValve()
  {
  xdata int TempI, LoopsI;

  TempI = adconV(1);
  LoopsI = 50;
  if( TempI > 50)
    {
    do
      {
      DigoutGB |= ValveClose;
      TempI = adconV(1);
      if ( TempI  < 34)
	LoopsI--;
      }while ( LoopsI !=0);
    DigoutGB &= NoValve;
    }
  }

/** Open the actuator  **/

openValve()
  {

  xdata int TempI;

  TempI = adconV(1);
  if( TempI < 220)
    {
    do
     {
     DigoutGB |= ValveOpen;
     } while ( adconV(1) < 230);
    DigoutGB &= NoValve;
    }
  }

/** set controler clock **/
settime(char *CharP)
  {

  CharP = strchr( CharP, ',');
  CharP++;
  CurrentTimeSG.hour = atoi ( CharP);
  CharP = strchr( CharP, ',');
  CharP++;
  CurrentTimeSG.min  = atoi ( CharP);
  CharP = strchr( CharP, ',');
  CharP++;
  CurrentTimeSG.sec  = atoi ( CharP);
  CharP = strchr( CharP, ',');
  CharP++;
  CurrentTimeSG.day  = atoi ( CharP);
  CharP = strchr( CharP, ',');
  CharP++;
  CurrentTimeSG.year = atoi ( CharP);
  }

/** Irq to perf ADC I/O & controle **/
timer2()  interrupt 14 using 1
  {

  PCON |= 0x10;    /** reset watch dog timer **/
  T3   =  153;
  P1 &= 0xfd;      /** select i/o device **/
  MrecGS[LoopsGI].Ip = IP;
  TGB = IP;
  P1 |= 2;
  if( AdcGI++ == 3)  /** Perform a compete ADC every  3 +/- 210 mS **/
    {
    MrecGS[LoopsGI].avlevel[0]= adconV( 0);
    MrecGS[LoopsGI].avlevel[1]= adconV( 1);
    MrecGS[LoopsGI].avlevel[2]= adconV( 2);
    MrecGS[LoopsGI].avlevel[3]= adconV( 3);
    MrecGS[LoopsGI].avlevel[4]= adconV( 4);
    MrecGS[LoopsGI].avlevel[5]= adconV( 5);
    MrecGS[LoopsGI].avlevel[6]= adconV( 6);
    MrecGS[LoopsGI].avlevel[7]= adconV( 7);
    AdcGI=0;
    }
/**  controle vlave every 3 * 70 mSec **/
  if( ValvectGI++ > 0)  /** on period **/
    {
    DigoutGB &= ~ValveClose;
    DigoutGB &= ~ValveOpen;
    }
  if ( ModeGC == ON  )
    {
/**  do the phase check **/
    if ( (MrecGS[LoopsGI].Ip  & 1) == 0)
      {
      if( PhaseCountGI != 0)
	{
	PhaseCountGI--;
	} else
	  DigoutGB &= ~PumpON;
      } else
	{
	if ( PhaseCountGI < 30)
	  PhaseCountGI++;
	if ( PhaseCountGI > 28)
	   DigoutGB |= PumpON;
	}
/** check phase fault   **/
    if ( (TGB & 0x06) !=0)   /**  check the staus of borehole **/
    {
/** check borehole level **/
    if (MrecGS[LoopsGI].avlevel[0] >  LevelGI)
      {
/**  Inhibit Lowlevel loc-out clearance **/
      if ( LowlevelGI < 71)
	LowlevelGI++;
/** Valve control  **/
      if( ValvectGI > 20 )    /**  wait time for on 6 * 70 mSec **/
	{
	ValvectGI=0;
	if (FlowGF >  (FlowSetPointGF+1))
	  {
	  if ( PhaseCountGI > 10)
	    DigoutGB |=ValveClose;      /**   control valve close **/
	  } else if (FlowGF <  (FlowSetPointGF-1))
	     {
	     if ( PhaseCountGI > 10)
	       DigoutGB |=ValveOpen;    /**   control valve open **/
	     } else
	      {
	      DigoutGB &= ~ValveClose;
	      DigoutGB &= ~ValveOpen;
	      }
	}
/** End of Valve control  **/

      } else if (MrecGS[LoopsGI].avlevel[0] <  LevelGI )
	{
	if (LowlevelGI-- ==0)
	  DigoutGB |=  LockOut;  /** trip bore hole **/
	DigoutGB &= ~PumpON;     /** switch pump on **/
	DigoutGB &= ~ValveClose;
	DigoutGB &= ~ValveOpen;
	}
    } else
      {

      if ( MrecGS[LoopsGI].avlevel[0]  > ClosePos)  /** 30 digital units  */
	DigoutGB |= ValveClose;    /**   close valve **/
	else
	{
	DigoutGB &= ~ValveClose;
	DigoutGB &= ~ValveOpen;
	}
      DigoutGB &= ~PumpON;
      }

    }else
      {
/** 30 digital units  **/
      if ( MrecGS[LoopsGI].avlevel[0]  > ClosePos)
	DigoutGB |= ValveClose;
/**   close valve **/
      DigoutGB &=  ~PumpON;       /**  switch pump off **/
      }

/**  treat time  **/
  if( mSecGI++ == 13)  /** 13 samples +/- 1 second **/
    {
    FlowGF   = (float) TL0;
    TFlowGF += (float) TL0;
    TL0=0;
    MrecGS[LoopsGI].min  = CurrentTimeSG.min;
    MrecGS[LoopsGI].hour =  CurrentTimeSG.hour;
    mSecGI = 0;

    if( ++CurrentTimeSG.sec >59)
      {
      if (  LoopsGI++ > DAYS*60*24)
	 LoopsGI=0;
      MrecGS[LoopsGI].avlevel[0]= adconV( 0);
      MrecGS[LoopsGI].avlevel[1]= adconV( 1);
      MrecGS[LoopsGI].avlevel[2]= adconV( 2);
      MrecGS[LoopsGI].flow  = FlowGF;
      MrecGS[LoopsGI].Tflow = TFlowGF;
      CurrentTimeSG.sec=0;
      if ( PhaseCountGI < 2 )
	PhaseCountGI++;
      if( ++CurrentTimeSG.min >59)
	{
	if ( DigoutGB & 1 == 0)
	  DigoutGB &= ~LockOut;    /** reset bore hole **/
	CurrentTimeSG.min=0;
	CurrentTimeSG.hour++;
	if( CurrentTimeSG.hour > 23)
	  {
	  CurrentTimeSG.hour=0;
	  CurrentTimeSG.day++;
	  }
	}
      }
    MrecGS[LoopsGI].min        = CurrentTimeSG.min;
    MrecGS[LoopsGI].hour       = CurrentTimeSG.hour;
    MrecGS[LoopsGI].avlevel[0] = adconV( 0);
    MrecGS[LoopsGI].avlevel[1] = adconV( 1);
    MrecGS[LoopsGI].avlevel[2] = adconV( 2);
    MrecGS[LoopsGI].flow       = FlowGF;
    MrecGS[LoopsGI].Tflow      = TFlowGF;
    }  /** end of time  **/

  if(ModeGC == OFF  )
    {
     DigoutGB &= PumpOFF;  /*  switch pump off **/
    if ( MrecGS[LoopsGI].avlevel[2] >  50)
      DigoutGB |= ValveClose; /** close valve **/
    }

  switch ( TGI)
    {
    case 1:
      DigoutGB |= 0x80;
    break;
    case 2:
      DigoutGB &= ~0x80;
    default :
	 TGI =0;
    }
  TGI++;

  P1 &= 0xfd;     /** select I/O  **/
  OP = DigoutGB;  /** set    I/O  **/
  P1 |= 2;
  T20V =0;
  P1 &= ~0x80;
  }

/** Get 1 cahr from comms port **/

char Getchar()
  {
  xdata char c;

bdata int CounterI=0;

/*
  do
    {
    if( CounterI ++ > 1000)
      {
      CounterI =0;
      }
    }
*/
  while(! RI);
  c= S0BUF;
  RI=0;
  return ( c);
  }

main()
  {

  int  xdata posI, CounterI,LoopsI;
  float xdata TF;
  char xdata CounterC=0, *CharP, AdcC, TP[40], TC;
  unsigned char xdata LowUC, HighUC;

#ifdef PIA
  InitPIA();
#endif

  ValvectGI =0;
  LowlevelGI=71;
  TGI=0;

  TMOD  = 0x25;  /** 0x25 **/
  /** start of serial port init **/
  S0CON = 0x5A;  /** init mode 1 uart 7 bit E 1 stop  Enable RX**/
  TH1   = 250;
  TR1   =   1;   /** Start Timer 1 **/
  /** end of serial port init **/

  TM2CON = 0x81;  /** init timer2 16 bit ovrf, c/8Fosc/12**/

  ET2    = 1;
  IP1 = 0x80;

  DigoutGB=0;
  P1  = 0x10;       /** Force RAM select ! serial **/
  TH0 = 0;          /** set counter High value **/
  TL0 = 0;          /** set counter low  value **/
  TR0 = 1;          /** Start Timer 0 **/
  PhaseCountGI=30;
  TFlowGF = 0;
  FlowGF=0;
  FlowSetPointGF     = Flows;
  MaksFlowSetPointGF = MaksFlow;
  AdcGI=0;
  mSecGI  = 0;
  SecGI   = 0;
  LoopsGI = 0;
  CurrentTimeSG.sec  =0;
  CurrentTimeSG.hour =0;
  CurrentTimeSG.min  =0;
  CurrentTimeSG.day  =17;
  CurrentTimeSG.month=03;
  CurrentTimeSG.year =95;
  LevelGI  = 0x30;
  LevelMGI = 0x50;
  ModeGC   = ON;
  EA       = TRUE;

/*  PCON |= 0x10;
  ** start dead man timer
  T3   =  153;
*/
  MrecGS[LoopsGI].min  = CurrentTimeSG.min;
  MrecGS[LoopsGI].hour =  CurrentTimeSG.hour;
  MrecGS[LoopsGI].avlevel[0]= adconV( 0);
  MrecGS[LoopsGI].avlevel[1]= adconV( 1);
  MrecGS[LoopsGI].avlevel[2]= adconV( 2);
  MrecGS[LoopsGI].flow = FlowGF;
  MrecGS[LoopsGI].Tflow = TFlowGF;
for(;;)
  {
	      sprintf( TxBufGCA ,"UUUU,%s:%d,%d:", STATION,\
		 (int)MrecGS[LoopsI].hour,\
		 (int)MrecGS[LoopsI].min);
	      sprintf(
	       &TP, "%d,%d,%d,%d,%d,%d,%d,%d,",\
		 MrecGS[LoopsI].avlevel[0],\
		 MrecGS[LoopsI].avlevel[1],\
		 MrecGS[LoopsI].avlevel[2],\
		 MrecGS[LoopsI].avlevel[3],\
		 MrecGS[LoopsI].avlevel[4],\
		 MrecGS[LoopsI].avlevel[5],\
		 MrecGS[LoopsI].avlevel[6],\
		 MrecGS[LoopsI].avlevel[7]
	      );
	      strcat( TxBufGCA, TP);
	      memset ( TP, 0 , sizeof ( TP));
	      sprintf( &TP, "%d,", (int) MrecGS[LoopsGI].Ip);
	      strcat( TxBufGCA, TP);
	      memset ( TP, 0 , sizeof ( TP));
	      sprintf( &TP, "%.0f,", MrecGS[LoopsGI].flow);
	      strcat( TxBufGCA, TP);
	      memset ( TP, 0 , sizeof ( TP));
	      sprintf( &TP, "%.0f&\r~", MrecGS[LoopsGI].Tflow);
	      strcat( TxBufGCA, TP);
	      Puts();
  }
  for(;;)
    {


    /** COMS INTERFACE **
    }         /** end of for(;;)  **/
  }           /** end of main **/