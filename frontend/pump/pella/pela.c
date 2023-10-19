#include <alloc.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <graphics.h>

#include "c:\include\c_incl\kbd_char.h"
#include "c:\include\c_incl\vec_strn.h"
#include "c:\include\serial\serial.h"
#include "C:\include\c_incl\intstruc.h"
#include "c:\wcrc\include\weer.h"
#include "c:\wcrc\pela\pela.h"

#define SixMeterTankLD   40000
#define EightMeterTankLD 10000

int LOOPIES=10,TestIG=1730, StartTotalGI=3, StopTotalGI =6;
int AND[9], SumpLevelMaksGI, SumpLevelDecPerGI, SumpLevelMinGI;
int SumpLevelSet6GI, SumpLevelSet8GI, StartFlowCI=3,StopFlowCI=6;
int SensitivityGI, EightmeterSGI, SixmeterSGI;

int Off_set[] = {
	2048,2048,2048,2049,2048,2048,2048,2048,\
	2048,2048,2048,2048,2048,2048,2048,2048,2048
	};
int GainIGA[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
int DelayTimeGI = 1600;
time_t mytime;
int BASE=0x220;
int ContmodeI=1;  /** 1 == individual,  0 == both **/
float  devider_25 =(float) MAKS_BITS/2500, devider_10 =(float) 1646/10;
float  SixmeterfGF=0, EightmeterfGF=0, OutputfGF=0;
float  SixmeterfsetGF=0, EightmeterfsetGF=0;
float  SixMeterTflowGF=0, EightMeterTflowGF=0, MamreTotalfGF=0;
float EightMeterflowGF, SixMeterFlowGF;
int SixmeterGI, EightmeterGI;
long SixmetercGI=0, EightmetercGI =0, OutputfGI=0;
double AnalogTotalsGFA[16], AnalogTotalperMinGFA[16];
unsigned char O_P1=0, O_P2=0;
int v_error[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int v_min[]   = { 327,327,819,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800};
int v_maks[]  = { 2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048};
float v_level[] = { 250, 1500, 1, 9,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Switch[]  = { 0,0,0,0};
FILE	*fp_output;
char *BOX_NO[] = { "hadc1278"}, OperationModeGC;
char *Fonts[] =
	{
  "DefaultFont",   "TriplexFont",   "SmallFont",
  "SansSerifFont", "GothicFont"
	};

char *TextDirect[] =
	{
  "HorizDir",  "VertDir"
	};

char *HorizJust[] =
	{
  "LeftText",   "CenterText",   "RightText"
	};

char *VertJust[] =
	{
  "BottomText",  "CenterText",  "TopText"
	};

int	X_POS_L[CLICK_POS+3];
int	X_POS_R[CLICK_POS+3];
int	Y_POS_L[CLICK_POS+3];
int	Y_POS_R[CLICK_POS+3];
int CLICK_TYPE[ CLICK_POS+3];

char rx_buffer[tx_buf_size];
char tx_buffer[tx_buf_size];
char   STATION_NAME[32];
int    Waardes[DATA_BYTES], LOOPS, old_minute=71;
double AspectRatio;		/** Aspect ratio of a pixel on the screen*/
int    GraphDriver;		/** The Graphics device driver		*/
int    GraphMode;		  /** The Graphics mode value		*/
int    MaxX, MaxY;		/** The maximum resolution of the screen */
int    MaxColors;		  /** The maximum # of colors available	*/
int    ErrorCode;		  /** Reports any graphics errors		*/
int    Counts, Min_interval, Maks_speed, Display_time, Maks_temp, Min_temp;
int    tnc_port, id, tnc_num, p_mode, poled_loops;
int    plc_port, select_port, old_day, DAY, MIN, SECOND;
void *menu;

struct palettetype palette;		/** Used to read palette info	*/
struct tm *my_time;

file_open()
	{

	char karakter[256], *wcr_env;
	time_t mytime;
	time( &mytime);

	my_time  = localtime(&mytime);
	old_minute = MIN;
/*	wcr_env = getenv( "WCR_DAT");*/
	sprintf( karakter, "c:\\wcrc\\data\\pela_%02d.doc", my_time->tm_mday);
	if ( (fp_output = fopen( karakter, "at+")) ==0)
		if ( (fp_output = fopen( karakter, "wt")) ==0)
			{
			error(2);
			exit(2);
			}
	 }
/*																														*/
/*	INITIALIZE: Initializes the graphics system and reports 	*/
/*	any errors which occured.					*/
/*									*/

	Initialize()
	{
  int xasp, yasp;			/** Used to read the aspect ratio*/

  GraphDriver = DETECT; 		/** Request auto-detection	*/
  initgraph( &GraphDriver, &GraphMode, "" );
  ErrorCode = graphresult();		/** Read result of initialization*/
	if( ErrorCode != grOk )
		{		/** Error occured during init	*/
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
		}

  getpalette( &palette );		/** Read the palette from board	*/
  MaxColors = getmaxcolor() + 1;	/** Read maximum number of colors*/

  MaxX = getmaxx();
  MaxY = getmaxy();			/** Read size of screen		*/

  getaspectratio( &xasp, &yasp );	/** read the hardware aspect	*/
  AspectRatio = (double)xasp / (double)yasp; /** Get correction factor	*/
	}

 int read_adc( adc)
	 int adc;
	 {
	 int address, val;
	 long loop_counter;
	 unsigned char data;

	 outportb ( BASE+9,GainIGA[adc]);
	 outportb( BASE+10, adc);
	 outportb( BASE+12, 0);  /* trigger adc */
	 for( loop_counter =0; loop_counter < 1000; loop_counter++);
/*	 delay(1);*/
	 data =inportb (BASE+5);
	 data &= 0x0f; /** mask off un wanted bits **/
	 val =(int) data <<8;
	 val +=inportb (BASE+4);
	 return (val);
	 }

set_data()
	{
	unsigned char ValhUC, VallUC;
	int tmpI, tmp2I;
	float PlevelSixF, PlevelEightF, tmpF, TotalFlowF;

	tmpF   = (float)0x1fff*SixmeterGI;
	tmpF   = tmpF/100;
	tmpI   = (int)tmpF;
	tmp2I  = tmpI/256;
	ValhUC = (unsigned char) tmp2I;
	VallUC = (unsigned char) tmpI;
	outportb( BASE+6, VallUC);
	outportb( BASE+7, ValhUC);

	tmpF   = (float)0x1fff*EightmeterGI;
	tmpF   = tmpF/100;
	tmpI   = (int)tmpF;
	tmp2I  =  tmpI/256;
	ValhUC = (unsigned char) tmp2I;
	VallUC = (char) tmpI;
	outportb( BASE+4, VallUC);
	outportb( BASE+5, ValhUC);
	}

void check_data()
	{
	int counter, LevelI;
	char *pchar;
	float LevelF, PlevelSixF, PlevelEightF, TotalFlowF;
	float TLevelF, TF;
	float PFSixF=0, PFEightF=0;

	LevelF = ((float)Waardes[2] / (float)v_maks[2])*100;
	LevelI = (int) LevelF;

	if( OperationModeGC == 'a'|| OperationModeGC =='A')
		{
		SumpLevelSet8GI = SumpLevelSet6GI=100;
		PlevelSixF   =  (float) Waardes[0]/MAKS_BITS;
		TF = SixMeterTankLD;
		TLevelF = PlevelSixF   =(float) TF*PlevelSixF;
		PlevelEightF =  (float) Waardes[1]/MAKS_BITS;
		TF = EightMeterTankLD;
		TLevelF += PlevelEightF = TF*PlevelEightF;
		if( TLevelF >0)
			{
			SumpLevelSet8GI = PlevelEightF = (PlevelEightF/TLevelF)*100;
			SumpLevelSet6GI = PlevelSixF   = (PlevelSixF/TLevelF)*100;
/*
			if( SumpLevelSet8GI >9)
					SumpLevelSet8GI -=9;
			if( SumpLevelSet6GI <91)
					SumpLevelSet6GI +=9;
*/			}
		TotalFlowF =  EightMeterflowGF + SixMeterFlowGF;
		if( TotalFlowF > 0)
			{
			if( EightMeterflowGF >0)
				PFEightF = (EightMeterflowGF/TotalFlowF)*100;
			if( SixMeterFlowGF >0)
				PFSixF	 = (SixMeterFlowGF/TotalFlowF)*100;
			}
		if( LevelI < SumpLevelMinGI)
			{
			if ( LOOPIES++ >SensitivityGI)
				{
				LOOPIES=1;
/**   Check spread of flow **/
				if( EightmeterGI < 100 )
					EightmeterGI += SumpLevelDecPerGI;
				if( SixmeterGI < 100)
					SixmeterGI += SumpLevelDecPerGI;
/*
				 if(  PFEightF <  PlevelEightF || LevelI < SumpLevelMinGI)
					 {
					 if( EightmeterGI < 100 )
						 EightmeterGI += SumpLevelDecPerGI;
					 }	else if(  EightMeterflowGF > PFEightF )
						 {
						 EightmeterGI -= SumpLevelDecPerGI;
						 if( EightmeterGI < 0)
							 EightmeterGI = 0;
					 }
				 if(   PFSixF < PlevelSixF || LevelI < SumpLevelMinGI)
					 {
					 if( SixmeterGI < 100)
						 SixmeterGI += SumpLevelDecPerGI;
					 else
						 SixmeterGI =100;
					 } else if(  SixMeterFlowGF > PFSixF )
						 {
						 if( SixmeterGI > SumpLevelDecPerGI &&EightMeterflowGF > PFEightF)
							 SixmeterGI -= SumpLevelDecPerGI;
						 else
							 SixmeterGI =0;
						 }
*/
					 }
			} else 	if( LevelI > SumpLevelMaksGI)
			 {
			 if ( LOOPIES++ >SensitivityGI)
				 {
				 LOOPIES=1;
				 EightmeterGI -= SumpLevelDecPerGI;
				 if( EightmeterGI < 0)
					 EightmeterGI =0;
				 SixmeterGI -= SumpLevelDecPerGI;
				 if( SixmeterGI < 0)
							 SixmeterGI = 0;
/*
				 if( EightmeterGI > 0 && SixMeterFlowGF > PFSixF)
					 EightmeterGI -= SumpLevelDecPerGI;
					 if( EightmeterGI < 0)
						 EightmeterGI -= SumpLevelDecPerGI;
				 if( SixmeterGI > 0 && EightMeterflowGF > PFEightF)
						 {
						 SixmeterGI -= SumpLevelDecPerGI;
						 if( SixmeterGI < SumpLevelDecPerGI)
							 SixmeterGI = 0;
						 }
*/
					 }
				 }else 	if( LevelI <= SumpLevelMaksGI &&LevelI >= SumpLevelMinGI )
					 {
					 if ( LOOPIES++ >SensitivityGI)
				 		 {
				 		 LOOPIES=1;
						 if( EightmeterGI > SumpLevelSet8GI)
							 {
							 EightmeterGI -= SumpLevelDecPerGI;
							 } else if( EightmeterGI < SumpLevelSet6GI)
								 {
								 EightmeterGI += SumpLevelDecPerGI;
								 if( EightmeterGI < SumpLevelDecPerGI)
									 EightmeterGI = 0;
								 }

						 if( EightmeterGI < 0)
							 	 EightmeterGI = 0;
						 if( SixmeterGI > SumpLevelSet6GI)
							 {
							 SixmeterGI -= SumpLevelDecPerGI;
							 if( SixmeterGI < SumpLevelDecPerGI)
								 SixmeterGI = 0;
							 } else if( SixmeterGI < SumpLevelSet6GI)
								 {
								 SixmeterGI += SumpLevelDecPerGI;
								 if( SixmeterGI < SumpLevelDecPerGI)
									 SixmeterGI = 0;
								 }
					 }
				 }
		} else
		 {
		 if( LevelI > SumpLevelMinGI)
			 {
			 if ( LOOPIES++ >SensitivityGI)
				{
				LOOPIES=0;
				SixmeterGI   -= SumpLevelDecPerGI;
				EightmeterGI -= SumpLevelDecPerGI;
				if(  EightmeterGI  < 0 )
					EightmeterGI   =0;
				if(  EightmeterGI < 0)
					EightmeterGI = 0;
				}
			 } else 	if( LevelI < SumpLevelMaksGI)
				 {
				 if ( LOOPIES++ >SensitivityGI)
					 {
					 LOOPIES=0;
					 if(  SixmeterGI   < SumpLevelSet6GI )
						 SixmeterGI   += SumpLevelDecPerGI;
					 else if(  SixmeterGI   > SumpLevelSet6GI )
						 SixmeterGI   -= SumpLevelDecPerGI;
					 if(  SixmeterGI  < 0 )
						 SixmeterGI =0;
					 if(  EightmeterGI < SumpLevelSet8GI )
						 EightmeterGI += SumpLevelDecPerGI;
					 else if(  SixmeterGI   > SumpLevelSet6GI )
						 SixmeterGI   -= SumpLevelDecPerGI;
					 if(  EightmeterGI  < 0 )
						 EightmeterGI =0;
					 } else 	if( LevelI <= SumpLevelMaksGI &&LevelI >= SumpLevelMinGI )
					 {
					 if ( LOOPIES++ >SensitivityGI)
				 		 {
				 		 LOOPIES=1;
						 if( EightmeterGI > SumpLevelSet8GI)
							 {
							 EightmeterGI -= SumpLevelDecPerGI;
							 } else if( EightmeterGI < SumpLevelSet6GI)
								 {
								 EightmeterGI += SumpLevelDecPerGI;
								 if( EightmeterGI < SumpLevelDecPerGI)
									 EightmeterGI = 0;
								 }

						 if( EightmeterGI < 0)
							 	 EightmeterGI = 0;
						 if( SixmeterGI > SumpLevelSet6GI)
							 {
							 SixmeterGI -= SumpLevelDecPerGI;
							 if( SixmeterGI < SumpLevelDecPerGI)
								 SixmeterGI = 0;
							 } else if( SixmeterGI < SumpLevelSet6GI)
								 {
								 SixmeterGI += SumpLevelDecPerGI;
								 if( SixmeterGI < SumpLevelDecPerGI)
									 SixmeterGI = 0;
								 }
					 }
				 }
}
		}
	 if( old_minute != MIN)
		{
		SixmeterfGF=EightmeterfGF=OutputfGF=0;
		EightmetercGI = SixmetercGI =0; OutputfGI=0;
		}
	}

void Get_data( )
	{

	char *pchar;
	int LOOPS,	loops;

	for( loops=0; loops < 16; loops++)
		{
		Waardes[loops] = read_adc(loops);
		if (Waardes[ loops] < v_min[loops])
			v_error[loops] =1;
		else
			v_error[loops] =0;
		Waardes[loops] -= Off_set[ loops];
		if( Waardes[loops] > -15  && Waardes[loops] < 0)
			Waardes[loops] = 0;
		AnalogTotalperMinGFA[loops] += (double)Waardes[loops];
		}
	SixmeterfGF   +=Waardes[3];
	EightmeterfGF += Waardes[4];
	OutputfGF     +=Waardes[5];
	SixmetercGI++, EightmetercGI++, OutputfGI++;
	Waardes[loops++] = inportb( BASE+8); /** read dig i/p 0 - 7 **/
	Waardes[loops++] = inportb( BASE+9); /** read dig i/p 7 - 15 **/
/*	Waardes[2] =TestIG;*/
 }

error( string)
int string;
	{
	char *error_string[]=
		{
		"Program aborting",                       /* 0 */
		"Wrong parameters passed to program",     /* 1 */
		"DOS error could not open file",          /* 2 */
		"NO RESPONCE FROM ",                   /* 3 */
		"DOS error could not write to the file",  /* 4 */
		};
	int tone;

	setbkcolor (RED);
	clearviewport();
	setcolor( WHITE);
	outtextxy(200,200,error_string[string]);
	for ( tone = 800; tone > 400; tone -=100)
		{
		sound (tone);
		delay( 90);
		}
	nosound();
	}


draw_cursor( xpos, ypos)
	{
	setcolor(EGA_YELLOW);
	line( xpos-10, ypos   , xpos+10, ypos);
	line( xpos   , ypos-10, xpos   , ypos+10);
	}

on_off( xpos, ypos)
int xpos, ypos;
	{

	char kar;
	int level=0, maks, kbd_loops=0;

		if ( level == 0)
			{
			setcolor( YELLOW);
			setfillstyle ( SOLID_FILL, YELLOW);
			} else
				{
				setcolor( RED);
				setfillstyle ( SOLID_FILL, RED);
				}
		bar(  xpos, ypos, xpos+30, ypos+30);
		if ( level == 1)
			{
				setcolor( YELLOW);
				setfillstyle ( SOLID_FILL, YELLOW);
			} else
				{
			setcolor( GREEN);
			setfillstyle ( SOLID_FILL, GREEN);
				}
		bar(  xpos+50, ypos, xpos+80, ypos+30);
		setcolor( WHITE);
		outtextxy ( xpos+10, ypos+10, "ON");
		outtextxy ( xpos+55, ypos+10, "OFF");
	do
		{
		if ( kbhit())
			{
		kar = getch();
		if ( kar ==0)
			{
			kar = getch();
			switch( kar)
				{
				case 'M':
					level =1;
				break;
				case 'K':
					level = 0;
				break;
				}
		if ( level == 0)
			{
			setcolor( YELLOW);
			setfillstyle ( SOLID_FILL, YELLOW);
			} else
				{
				setcolor( RED);
				setfillstyle ( SOLID_FILL, RED);
				}
		bar(  xpos, ypos, xpos+30, ypos+30);
		if ( level == 1)
			{
				setcolor( YELLOW);
				setfillstyle ( SOLID_FILL, YELLOW);
			} else
				{
			setcolor( GREEN);
			setfillstyle ( SOLID_FILL, GREEN);
				}
		bar(  xpos+50, ypos, xpos+80, ypos+30);
		setcolor( WHITE);
		outtextxy ( xpos+10, ypos+10, "ON");
		outtextxy ( xpos+55, ypos+10, "OFF");
		} else if ( kar == 0x0d)
			{
			Waardes[1] = level;
			kar =0;
			}
		} else
			{
			kar=1;
			if ( kbd_loops++ > 19500)
				kar=0;
			}
		} while( kar != 0x0);
	}

execute_cr( xpos,ypos)
int xpos, ypos;
	{
	struct viewporttype t_type;
	struct linesettingstype l_type;
	char kar, tmp_buf[15];
	int test_counter=0, test_val=0, t1=1, t2=1, pstep;
	int maks=1023, level=50, display, kbd_loops=0;
	double percentage=0;

	do
		{
		if ( t1 >0)
			if ((xpos >= X_POS_L[test_counter]) && ( xpos <= X_POS_R[test_counter]) \
			&&  (ypos >= Y_POS_L[test_counter]) && (ypos <= Y_POS_R[test_counter]))
			{
				t1= 0;
				t2= 0;
			}
		test_val = t1|t2;
		}while (test_val !=0 && test_counter++ <= CLICK_POS);
		if( test_counter <= CLICK_POS+1)
			{
			if ( CLICK_TYPE[test_counter] == PuMP)
				{
				on_off(10, 10);
				} else if ( CLICK_TYPE[test_counter] == MOTOR)
					{
					on_off(10, 10);
					} else if ( CLICK_TYPE[test_counter] == VALVE)
						{
			xpos=MaxX-151;ypos=MaxY-151;
			getviewsettings(&t_type);
			getimage ( xpos, ypos, xpos+100, ypos+120, menu);
			setviewport( xpos, ypos, xpos+100, ypos+120, 1);
			setfillstyle ( SOLID_FILL, LIGHTGRAY);
			floodfill( 3,3,LIGHTGRAY);
			getlinesettings( &l_type);
			setlinestyle ( SOLID_LINE, l_type.upattern, THICK_WIDTH);
			setcolor( WHITE);
			rectangle( 1,1, 99,119);
			setlinestyle ( l_type.linestyle, l_type.upattern, l_type.thickness);
						percentage=0;
						xpos = 15 ; ypos =113;
						if( test_counter ==0)
							percentage = SumpLevelSet6GI;
						else
							percentage =  SumpLevelSet8GI;
						level =(int)((maks*percentage)/100);
						do
							{
							setcolor( WHITE);
							setfillstyle ( SOLID_FILL, WHITE);
							percentage =(float) level;
							percentage /= maks;
							percentage *=100;
							display =(int) percentage;
/*											xpos	ypos	width height alarm text  value   type*/
				display_level ( xpos, ypos, 9    , 88   , 75 ,  100, display, 'l',1);
							sprintf( tmp_buf, "%5.1f", percentage);
							Outtextxy ( MaxX-110, MaxY-48, tmp_buf, 6, 0);
							if ( kbhit())
							 {
							 kar = getch();
							 if ( kar ==0)
								 {
								 kar = getch();
									 switch( kar)
										{
										case 'H':
											 if ( level <maks)
												 level+= 1;
											 if (level > 1023)
												level =1023;
										break;
										case 'P':
											if ( level > 0)
												level -= 1;
										break;
										}
								} else if ( kar == 0x0d)
									{
									if( test_counter ==0)
										 SumpLevelSet6GI = (int) percentage;
									else
										 SumpLevelSet8GI= (int) percentage;
									kar =0;
									}
								kbd_loops=0;
							} else
								{
								if (kbd_loops++ > 300)
									kar =0;
								}
							} while( kar != 0x0);
						}else
							{
				getviewsettings(&t_type);
				setviewport( 5, 395, 120, 425, 1);
				if( OperationModeGC == 'a')
					{
					OperationModeGC = 'm';
					SumpLevelSet6GI = SixmeterSGI;
					SumpLevelSet8GI = EightmeterSGI;
/*
					SumpLevelSet6GI = SixmeterGI   = SixmeterSGI;
					SumpLevelSet8GI = EightmeterGI = EightmeterSGI;
*/					} else
						{
						OperationModeGC = 'a';
						}
					clearviewport();
					setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
					return;
							}
			putimage ( 0, 0, menu, COPY_PUT);
			setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
			}
	}
int o_p_adc( adc)
	 int adc;
	 {
	 int address, val;
	 unsigned char data;

	 outportb( BASE+10, adc);
	 delay(5);
	 data =inportb (BASE+5);
	 data &= 0x0f; /** mask off un wanted bits **/
	 val =(int) data <<8;
	 val +=inportb (BASE+4);
	 return (val);
	 }

pela()
	{

	char kar, text[13], date_buf[128];

	char *char_p;
	int min, y_half, devider, width, xpos=35, txpos, ypos;
	int y_pos, degrees, from ,to, counter;
	float barv_length, barv_pos, tt, travel, vtravel, Degrees;
	struct textsettingstype my_settings;
	struct viewporttype	tmp_type;
	time_t temp_time;
	struct tm *local_time;
	struct linesettingstype l_settings;

	getlinesettings (&l_settings);
	gettextsettings (&my_settings);
	setbkcolor( EGA_BLUE);
	setcolor( EGA_WHITE);
	rectangle(0,0,MaxX,MaxY);
	setfillstyle(SOLID_FILL, LIGHTGRAY);
	setcolor( WHITE);

	ypos =50;
	draw_tank ( xpos-1, ypos, 300, 100);
	txpos = xpos+150;
	line( txpos, ypos+100, txpos, ypos+200);
	xpos = MaxX - 135;   /**  35 from right + tank width **/
	draw_tank ( xpos-1, ypos, 100, 100);
	line( xpos+50, ypos+100, xpos+50, ypos+240);
	rectangle( xpos+40, ypos+240, xpos+60,ypos+268);
	line( xpos+50, ypos+268, xpos+50, ypos+300);
	arrow( xpos+50, ypos+300, 'd');
	draw_valve( txpos+(xpos-txpos)/2,  ypos+150, 1, 'h');
/*	pump_position ( txpos+155, ypos+190, ContmodeI);*/
	setcolor (WHITE);
	line( txpos, ypos+150+7, (txpos+(xpos-txpos)/2),  ypos+150+7);
	line( (txpos+(xpos-txpos)/2)+15, ypos+150+7, xpos+50,  ypos+150+7);
	line( txpos, ypos+200, txpos+20,  ypos+200);
	rectangle( txpos+20, ypos+192, txpos+50,ypos+208);
	line( txpos+50, ypos+200, txpos+70,  ypos+200);
	draw_valve( txpos+70,  ypos+193, 1, 'h');
	line( xpos+50, ypos+200, xpos+20,  ypos+200);
	rectangle( xpos+20, ypos+192, xpos-10,ypos+208);
	line( xpos-10, ypos+200, xpos-30,  ypos+200);
	draw_valve( xpos-45,  ypos+193, 1, 'h');
	line( txpos+85, ypos+200, xpos-45,  ypos+200);
	xpos = txpos + ((xpos - txpos) /2);
	ypos += 200;
	draw_tank ( xpos-75, ypos+50, 150, 100);
	line( xpos, ypos, xpos, ypos+50);
	arrow( xpos, ypos+50, 'd');
	line( xpos, ypos+150, xpos, ypos+200);
	arrow( xpos, ypos+199, 'd');
	line( xpos, ypos, xpos+25, ypos);
	line( xpos, ypos+190, xpos-110, ypos+190);
	rectangle( xpos-110, ypos+181, xpos-140,ypos+199);
	line( xpos-140, ypos+190, xpos-170, ypos+190);
	arrow( xpos-170, ypos+190, 'l');
	xpos-=70;
	ypos =10;

	setcolor(EGA_LIGHTMAGENTA);
	outtextxy( xpos, ypos+20 , "DEVELOPED BY");
	outtextxy( xpos, ypos+30 , "INFOTRANS CC");
	settextstyle( my_settings.font, HORIZ_DIR, 2);
	outtextxy( xpos-70  , ypos      , "PELLA RESERVOIR");
	setcolor(EGA_WHITE);
	settextstyle( my_settings.font, HORIZ_DIR, 1);
	setcolor(EGA_LIGHTCYAN);
	outtextxy( xpos+45, ypos+405 , SUMP);
	outtextxy( xpos+45, ypos+415 , "0-3 meter");
	outtextxy( xpos-165, ypos+155 , "Bulk Reservoir");
	outtextxy( xpos+215, ypos+155 , "Bulk Reservoir");
	outtextxy( xpos-145 , ypos+165 , "0-6 meter");
	outtextxy( xpos+245 , ypos+165 , "0-8 meter");
	}
/*	Begin main function						*/
/*									*/

int main(argc, argv)
int argc;
char *argv[];
	{

FILE *fp_temp;
void *cursor, *scr1;
struct time new_time;
struct viewporttype t_type;
struct linesettingstype l_type;

	char kar, *char_p, temp_buf[1048], *char_b;
	char connect[] = { "c XXXXXX\r"};
	char karakter[16];
	int Buffers, xpos=1,ypos, txpos=0, typos, tt;
	int counts, write_bytes, kbd_counter, char_send, data_loops=79;
	unsigned int size;
	int Loops, size_of_write_buf, old_sec, old_min, err_tx, ttxpos, ttypos, ttxposI;
	double FlowCalcF;

	v_level[0] =  devider_25*250;
	v_level[1] =  devider_25*1500;
	v_level[2] =  devider_10*1.3;
	v_level[3] =  devider_10*9;
	for ( Loops=0; Loops<16; Loops++)
		AnalogTotalperMinGFA[Loops]=0;
	memset( Waardes, -1, sizeof( Waardes));
	txpos=0;
	X_POS_L[txpos]    = 270;
	X_POS_R[txpos]    = 290;
	Y_POS_L[txpos]    = 220;
	Y_POS_R[txpos]    = 270;
	CLICK_TYPE[txpos++] = VALVE;

	X_POS_L[txpos]    = 420;
	X_POS_R[txpos]    = 440;
	Y_POS_L[txpos]    = 220;
	Y_POS_R[txpos]    = 270;
	CLICK_TYPE[txpos++] = VALVE;

	X_POS_L[txpos]    = 5;
	X_POS_R[txpos]    = 45;
	Y_POS_L[txpos]    = 395;
	Y_POS_R[txpos]    = 417;
	CLICK_TYPE[txpos++] = NOTHING;


	clrscr();
/*	system( "pcl-812");*/
	memset( Waardes, 0, sizeof(Waardes));
	Initialize();
	size   = imagesize(1,1,150,150);
	menu   = malloc( size);
	size   = imagesize(1,1,20,20);
	cursor = malloc( size);
/*
	size   = imagesize(1,1,100,50);
	scr1   = malloc( size);
*/
	xpos = MaxX/2; ypos = MaxY/2;
	if ( argc < 9)
	{
		error(0);
		sleep(1);
		error(1);
		exit(1);
	}
	strncpy ( &connect[2], argv[1], 6);
	strcpy ( STATION_NAME, connect);
/** int port,int speed,int bits,char parity,int stops) **/
	plc_port=select_port;

	if( *argv[2] == '1')
		{
		if (initRS232(COM1,9600,8,'N',2))
		 exit(1);
		} else
		if (initRS232(COM2,9600,8,'N',2))
		 exit(1);
	EightmeterSGI = EightmeterGI = SumpLevelSet8GI = atoi ( argv[3]);
	SixmeterSGI   = SixmeterGI   = SumpLevelSet6GI = atoi ( argv[4]);
	SumpLevelMaksGI   = atoi ( argv[5]);
	SumpLevelDecPerGI = atoi ( argv[6]);
	SumpLevelMinGI	  = atoi ( argv[7]);
	OperationModeGC   = *argv[8];
	SensitivityGI       = atoi( argv[9]);
	tnc_port=select_port;

	time( &mytime);
	my_time  = localtime(&mytime);
	old_day  = my_time->tm_mday;

	txpos=xpos-10; typos=ypos-10;
	clearviewport();
	pela();
	getimage( txpos,typos,txpos+20,typos+20, cursor);
	draw_cursor( xpos, ypos);
	time( &mytime);
	my_time = localtime(&mytime);
	ctime( &mytime);
	DAY = my_time->tm_mday;
	MIN = my_time->tm_min;
	old_sec= my_time->tm_sec;
	old_minute = MIN;
	for(;;)
		{
		set_data();
		time( &mytime);
		my_time = localtime(&mytime);
		ctime( &mytime);
		DAY = my_time->tm_mday;
		MIN = my_time->tm_min;
		SECOND = my_time->tm_sec;
		if (old_sec != my_time->tm_sec)
			{
			old_sec = my_time->tm_sec;
			Outtextxy( 15, 320, char_p, strlen( char_p)-6, 0);
			}
		if (old_minute != my_time->tm_min)
			{
			clearviewport();
			pela();

			display_data();
			sprintf( temp_buf, "%d", OutputfGI);
			ttxposI=3;
			setcolor( LIGHTCYAN);
			rectangle( 2, MaxY-140,183, MaxY-88);
			outtextxy( 5, MaxY-137, " 6M Tf Kl    8M TF Kl");
			outtextxy( 5, MaxY-112, "MAM TF Kl   PEL TF Kl");
			Loops=3;
			FlowCalcF =(float) MAKS_FLOW/MAKS_BITS;
			FlowCalcF *= AnalogTotalperMinGFA[Loops];
			FlowCalcF /=OutputfGI+1;
			FlowCalcF *=60;
			FlowCalcF /=1000;
			AnalogTotalsGFA[Loops] += FlowCalcF;
			sprintf( temp_buf, "%9.0f", AnalogTotalsGFA[Loops++]);
			Outtextxy ( ttxposI, MaxY-127, temp_buf, 10, 0);
			ttxposI += 95;
			FlowCalcF =(float) MAKS_FLOW/MAKS_BITS;
			FlowCalcF *= AnalogTotalperMinGFA[Loops];
			FlowCalcF /=OutputfGI+1;
			FlowCalcF *=60;
			FlowCalcF /=1000;
			AnalogTotalsGFA[Loops] += FlowCalcF;
			sprintf( temp_buf, "%9.0f", AnalogTotalsGFA[Loops++]);
			Outtextxy ( ttxposI, MaxY-127, temp_buf, 10, 0);
			ttxposI = 3;
			FlowCalcF =(float) MAKS_MAMRE/MAKS_BITS;
			FlowCalcF *= AnalogTotalperMinGFA[Loops];
			FlowCalcF /=OutputfGI+1;
			FlowCalcF *=60;
			FlowCalcF /=1000;
			AnalogTotalsGFA[Loops] += FlowCalcF;
			sprintf( temp_buf, "%9.0f", AnalogTotalsGFA[Loops++]);
			Outtextxy ( ttxposI, MaxY-102, temp_buf, 10, 0);
			ttxposI += 95;
			FlowCalcF =(float) MAKS_ATLANTIS/MAKS_BITS;
			FlowCalcF *= AnalogTotalperMinGFA[Loops];
			FlowCalcF /=OutputfGI+1;
			FlowCalcF *=60;
			FlowCalcF /=1000;
			AnalogTotalsGFA[Loops] += FlowCalcF;
			sprintf( temp_buf, "%9.0f", AnalogTotalsGFA[Loops++]);
			Outtextxy ( ttxposI, MaxY-102, temp_buf, 10, 0);
			sprintf( rx_buffer,"%02d:%02d", my_time->tm_hour, my_time->tm_min);
			for ( Loops=0; Loops < 19; Loops++)
				{
				memset( temp_buf,0 ,sizeof( temp_buf));
				if( Waardes[Loops] < -1)
					Waardes[Loops] = -1;
				sprintf( temp_buf," %d", Waardes[Loops]);
				strcat( rx_buffer, temp_buf);
				}
			for ( Loops = StartTotalGI; Loops < StopTotalGI; Loops++)
				{
				memset( temp_buf,0 ,sizeof( temp_buf));
				sprintf( temp_buf," %.2f", AnalogTotalsGFA[Loops]);
				strcat( rx_buffer, temp_buf);
				}

			memset( temp_buf,0 ,sizeof( temp_buf));
			sprintf( temp_buf," %d", SumpLevelSet6GI);
			strcat( rx_buffer, temp_buf);
			memset( temp_buf,0 ,sizeof( temp_buf));
			sprintf( temp_buf," %d", SixmeterGI);
			strcat( rx_buffer, temp_buf);
			memset( temp_buf,0 ,sizeof( temp_buf));
			sprintf( temp_buf," %d", SumpLevelSet8GI);
			strcat( rx_buffer, temp_buf);
			memset( temp_buf,0 ,sizeof( temp_buf));
			sprintf( temp_buf," %d ", EightmeterGI);
			strcat( rx_buffer, temp_buf);
			sprintf( temp_buf," %c ", OperationModeGC);
			strcat( rx_buffer, temp_buf);
			strcat (rx_buffer, "\r");
			file_open();
			fwrite (  rx_buffer, strlen ( rx_buffer), sizeof( char),fp_output);
			fflush( fp_output);
			fclose( fp_output);
			for ( Loops=0; Loops<16; Loops++)
				AnalogTotalperMinGFA[Loops]=0;
			Loops = 10000;
			old_minute = my_time->tm_min;
			OutputfGI=0;
			}
		Get_data();
		check_data();
		time( &mytime);
		my_time = localtime(&mytime);
		char_p  = ctime( &mytime);
		my_time = localtime(&mytime);
		char_p = ctime( &mytime);
		my_time = localtime(&mytime);
		char_p = ctime( &mytime);
		for( Loops=0; Loops<1; Loops++)
			{
		time( &mytime);
		my_time = localtime(&mytime);
		ctime( &mytime);
		if( recRS232(  &karakter[0], -1) !=0)
		{
		if ( rx_packet ( rx_buffer) ==0)
			{
			char_p = rx_buffer;
			while ( *char_p == 'U')
				char_p++;
			if ( *char_p == 'O')
				{
				char_p++;
				switch( *char_p)
					{
					case 'B' :
						fflush(fp_output);
						char_p++;
					break;
					case 'H' :
						char_p++;
						tt = atoi( char_p);
						char_b = strchr ( char_p, ' ')+1;
						if( *char_b == '1')
							{
							tt= AND[tt];
							} else
								 {
								 tt = 1<<tt;
								 }
					break;
					case 'F':
						do
							{
							recRS232(&kar, -1);
							} while ( kar !=0);
						char_p = strchr( char_p, ' ')+1;
						char_b = strchr( char_p, '&');
						*char_b =0;
						fp_temp = fopen( char_p, "rt");
						if ( fp_temp ==0)
							{
							error(2, char_p);
							sprintf( rx_buffer, "Coul not found file !!  @&\r");
							sendRS232( strlen( rx_buffer), &rx_buffer);
							} else
							{
							do
								{
								memset( tx_buffer, 0 ,sizeof( tx_buffer));
								size_of_write_buf =0;
								do
									{
									tx_buffer[size_of_write_buf] = fgetc ( fp_temp);
									if ( feof( fp_temp))
										break;
									} while( tx_buffer[size_of_write_buf++] != '\n');
									Outtextxy( 10, 300, tx_buffer, 6);
									strncpy ( &tx_buffer[size_of_write_buf-1], "&\r", 4);
									sendRS232( strlen( tx_buffer), &tx_buffer);
									err_tx = counts=0;
									do
										{
										rx_buffer[counts] = recRS232( &kar, 1);
										if( kar == 0)
											{
											counts++;
											err_tx =0;
											} else
												if( err_tx++ > 3)
													break;
										}while ( rx_buffer[counts-1] != '&');
									char_p = &rx_buffer[5];
									if( counts > 11)
										while( *char_p++ == 'U');
									char_p--;
									if( strncmp ( char_p, "Oack&",5) !=0)
										break;
									memset( tx_buffer, 0, sizeof( tx_buffer-23));
									} while ( !feof( fp_temp));
								strcat ( tx_buffer, "@&\r");
								sendRS232( strlen( tx_buffer), &tx_buffer);
								memset( tx_buffer, 0, sizeof( tx_buffer-23));
							}
					fclose ( fp_temp);
					break;
					case 'S':
						fp_temp = fopen( "pela.VAL", "wt");
						if( fp_temp ==0)
							{
							error(2, "WITSAND.VAL");
							exit(2);
							}
						counts=0;
						memset( rx_buffer, 0, sizeof( rx_buffer));
						do
							{
							counts=0;
							memset( rx_buffer, 0, sizeof( rx_buffer));
							do
								{
								rx_buffer[counts] = recRS232( &kar, 1);
								if( kar == 0)
									counts++;
								else
									break;
								}while ( rx_buffer[counts-1] != '&');
							counts = strlen (rx_buffer)-1;
							if( counts > 3)
								fwrite ( rx_buffer, sizeof(char), strlen( rx_buffer), fp_temp);
							else
								break;
          								}while ( rx_buffer[counts-1] != '@');
							fclose( fp_temp);
					break;
					case 'D':
/*						old_pos = atoi( &rx_buffer[13]);*/
							char_p = strchr( &rx_buffer[13], ',');
							Loops  = atoi(char_p);
							fclose( fp_output);
							fp_output = fopen( karakter, "rt");
							fseek ( fp_output,size_of_write_buf, SEEK_END );
							for ( counts=0; counts < Loops; counts++)
								fread ( temp_buf, sizeof(char), size_of_write_buf, fp_output);
/* send data*/
							fclose( fp_output);
							fp_output = fopen( karakter, "at");
					break;
					case 'T':
						char_p++;
						mytime = atol( char_p)+1;
						stime( &mytime);
						ttxpos=5;ttypos=310;
						getviewsettings(&t_type);
						setviewport( ttxpos, ttypos, ttxpos+100, ttypos+ 60, 1);
						getimage ( ttxpos, ttypos, ttxpos+100, ttypos+120, menu);
					break;
					case 'K':
						ttxpos=5;ttypos=310;
						getviewsettings(&t_type);
						getimage ( ttxpos, ttypos, ttxpos+100, ttypos+120, menu);
						setviewport( ttxpos, ttypos, ttxpos+100, ttypos+ 60, 1);
						setfillstyle ( SOLID_FILL, RED);
						floodfill( 3,3,RED);
						getlinesettings( &l_type);
						memset( rx_buffer,0 ,sizeof( rx_buffer));
						sprintf( rx_buffer,"%02d:%02d", my_time->tm_hour, my_time->tm_min);
						for ( Loops=0; Loops < 19; Loops++)
							{
							memset( temp_buf,0 ,sizeof( temp_buf));
							if( Waardes[Loops] < -1)
								Waardes[Loops] = -1;
							sprintf( temp_buf," %d", Waardes[Loops]);
							strcat( rx_buffer, temp_buf);
							}
						for ( Loops = StartTotalGI; Loops < StopTotalGI; Loops++)
							{
							memset( temp_buf,0 ,sizeof( temp_buf));
							sprintf( temp_buf," %.2f", AnalogTotalsGFA[Loops]);
							strcat( rx_buffer, temp_buf);
							}

						memset( temp_buf,0 ,sizeof( temp_buf));
						sprintf( temp_buf," %d", SumpLevelSet6GI);
						strcat( rx_buffer, temp_buf);
						memset( temp_buf,0 ,sizeof( temp_buf));
						sprintf( temp_buf," %d", SixmeterGI);
						strcat( rx_buffer, temp_buf);
						memset( temp_buf,0 ,sizeof( temp_buf));
						sprintf( temp_buf," %d", SumpLevelSet8GI);
						strcat( rx_buffer, temp_buf);
						memset( temp_buf,0 ,sizeof( temp_buf));
						sprintf( temp_buf," %d ", EightmeterGI);
						strcat( rx_buffer, temp_buf);
						sprintf( temp_buf," %c ", OperationModeGC);
						strcat( rx_buffer, temp_buf);
						strcat (rx_buffer, "&\r");
						sendRS232( strlen( rx_buffer) ,  &rx_buffer);

					break;
					case 'V' :
						char_p = strchr( char_p, ' ')+1;
							/* change MAKS remote position **/
						SumpLevelSet6GI = SixmeterGI   = SixmeterSGI = atoi( char_p);
						char_p = strchr( char_p, ' ')+1;
							/* change MAKS remote position **/
						SumpLevelSet8GI = EightmeterGI = EightmeterSGI = atoi( char_p);
					break;
					case 'M' :
					if( OperationModeGC == 'a')
						{
						OperationModeGC = 'm';
						} else
							OperationModeGC = 'a';
					break;
					}
				putimage ( 0, 0, menu, COPY_PUT);
				setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
				Loops = 1000;
				}
			}
			}
			if (kbhit() !=0)
				{
				do
					{
					if (kbhit() !=0)
					{
/*					kbd_counter=0;*/
					kar = getch();
					if ( kar ==0)
						{
						kar = getch();
						switch (kar)
							{
							case 'H':
							 Loops=401;
							 if ( ypos >=11)
								 ypos--;
							break;
							case 'P':
							 Loops=401;
							 if ( ypos < MaxY-11)
								 ypos++;
							break;
							case 'K':
							 Loops=401;
							 if ( xpos >=11)
								 xpos--;
							break;
							case 'M':
							 Loops=401;
							 if ( xpos < MaxX-11)
								 xpos++;
							break;
							case -115:   /** ctrl up    **/
							 Loops=401;
							 if ( ypos-10 >= 11)
								 ypos-=10;
							break;
							case -111:   /** ctrl down  **/
							 Loops=401;
							 if ( ypos+10 <= MaxY-11)
								 ypos+=10;
							break;
							case 's':   /** ctrl left  **/
							 Loops=401;
							 if ( xpos-10 >= 11)
								 xpos-=10;
							break;
							case 't':   /** ctrl right **/
							 Loops=401;
							 if ( xpos+10 <= MaxX-11)
								 xpos+=10;
							break;

							case -104:   /** alt up    **/
							 Loops=401;
							 if ( ypos-50 >= 11)
								 ypos-=50;
							break;
							case -96 :   /** alt down  **/
							 Loops=401;
							 if ( ypos+50 <= MaxY-11)
								 ypos+=50;
							break;
							case -101:   /** alt left  **/
							 Loops=401;
							 if ( xpos-50 >= 11)
								 xpos-=50;
							break;
							case -99:   /** alt right **/
							 Loops=401;
							 if ( xpos+50 <= MaxX-11)
								 xpos+=50;
							break;
							}
						} else if( kar == 0x0d)
							 {
/*
							 putimage( txpos      ,typos, cursor, COPY_PUT);
*/
							 execute_cr( xpos, ypos);
							 kbd_counter = 10000;
							 }
/*
					sprintf ( temp_buf, "x - %d y - %d", xpos, ypos);
					Outtextxy ( 10, MaxY-15, temp_buf, 30, 0);
*/
					putimage( txpos      ,typos, cursor, COPY_PUT);
					sprintf( temp_buf, "%d %d", xpos,ypos);
					txpos=xpos-10; typos=ypos-10;
					getimage( txpos,typos,txpos+20,typos+20, cursor);
					draw_cursor( xpos, ypos);
					}
				 do
					{
					if (kbhit() ==0)
						kbd_counter++;
					else
						kbd_counter=-1;
					}while ( kbd_counter< 10000 && kbd_counter>0);
					}while ( kbd_counter < 10000);
				}
			}

		ttxpos=5;ttypos=395;
		getviewsettings(&t_type);
		setfillstyle ( SOLID_FILL, LIGHTGRAY);
		if( OperationModeGC == 'a')
			{
			setviewport( ttxpos, ttypos, ttxpos+40, ttypos+20, 1);
			floodfill( 1,1, LIGHTGRAY);
			setcolor( WHITE);
			setcolor( RED);
			rectangle(1,1,40,20);
			strcpy ( temp_buf, "Auto");
			outtextxy ( 5, 7, temp_buf);
			} else
				{
				setviewport( ttxpos, ttypos, ttxpos+60, ttypos+20, 1);
				floodfill( 1,1, LIGHTGRAY);
				setcolor( WHITE);
				setcolor(EGA_LIGHTCYAN);
				rectangle(1,1,60,20);
				strcpy ( temp_buf, "Manual");
				outtextxy ( 8, 7, temp_buf);
				}
		setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
		kbd_counter =0;
		if( data_loops++==80)
			{
			display_data();
			data_loops=0;
			}
		}
 }