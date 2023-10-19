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
#include "prem4r.h"

int DATA[DATA_bytes];
int TimeLowLevel1GI=0, TimeLowLevel2GI=0, TimeLowLevel3GI=0, LowLevelTimeGI;
int AND[9], DelayTimeGI, DefaultDelayTimeGI;

int Off_set[] = {	2047,2047,2047,2048+2,2047+2,2047,2047, \
									2047,2047,2047,2047,2047,2047,2047};

unsigned char GainGUCA[] = {
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	 };

time_t mytime;
int BASE=0x220,PumpsRunningGI=0; PumpsRunningGIA[] = {1,2,3};
int Pump1RunningGI=0, Pump2RunningGI=0, Pump3RunningGI=0;
float  devider_25 =(float) MAKS_BITS/2500, devider_10 =(float) 1646/10;
float  FLOW=0, FLOWT, PRESSURE1=0, PRESSURET1=0, PRESSURE2=0, PRESSURET2=0, PRESSURE3=0, PRESSURET3=0;
long peela_c=0, pressure_c1=0,  pressure_c2=0, pressure_c3=0;
unsigned char O_P1=0, O_P2=0;
int v_error[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int v_min[]   = { 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000};
int v_maks[]  = { 2042, 2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042};
float v_level[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Switch[]  = { 0,0,0,0};
FILE	*fp_output;
char *BOX_NO[] = { "hadc1278"};
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
int    Waardes[DATA_BYTES], LoopsGI=0, old_minute=71;
float  Valve_position=50;
double AspectRatio;		/** Aspect ratio of a pixel on the screen*/
int    GraphDriver;		/** The Graphics device driver		*/
int    GraphMode;		  /** The Graphics mode value		*/
int    MaxX, MaxY;		/** The maximum resolution of the screen */
int    MaxColors;		  /** The maximum # of colors available	*/
int    ErrorCode;		  /** Reports any graphics errors		*/
int    Counts, Min_interval, Maks_speed, Display_time, Maks_temp, Min_temp;
int    tnc_port, id, tnc_num, p_mode, poled_loops;
int    plc_port, select_port, old_day, DAY, MIN;
long   TimeOn1GL, TimeOn2GL, TimeOn3GL;

void *menu;

struct palettetype palette;		/** Used to read palette info	*/
struct tm *my_time;

display_text()
	{
	char date_buf[256];
	int xpos, color;
	float barv_length, value;

/** HEAD FLOW meter **/
	barv_length =(float) MAKS_FLOW/v_maks[3];
	value = (float) Waardes[3];
	barv_length *= value;
	if ( Waardes[3] < 0)
		color = 1;
	else
	 color=0;
	sprintf ( date_buf, "%6.2f L/s", barv_length);
	Outtextxy ( 265, 350 , date_buf, 12, color);

/** Pressure transducer 1 **/
	barv_length =(float) MAKS_PRESSURE/v_maks[4];
	barv_length *= Waardes [4];
	if ( Waardes[4] < 0)
		color = 1;
	else
	 color=0;
	sprintf ( date_buf, "%4.0f Kpa", barv_length);
	Outtextxy (380, 149  , date_buf, 8, color);

/** Pressure transducer 2 **/
	barv_length =(float) MAKS_PRESSURE/v_maks[5];
	barv_length *= Waardes [5];
	if ( Waardes[5] < 0)
		color = 1;
	else
	 color=0;
	sprintf ( date_buf, "%4.0f Kpa", barv_length);
	Outtextxy (380, 219  , date_buf, 8, color);

/** Pressure transducer 3 **/
	barv_length =(float) MAKS_PRESSURE/v_maks[6];
	barv_length *= Waardes [6];
	if ( Waardes[6] < 0)
		color = 1;
	else
	 color=0;
	sprintf ( date_buf, "%4.0f Kpa", barv_length);
	Outtextxy (380, 289 , date_buf, 8, color);

	pump_position( 15, MaxY-10, (Waardes[17] & 0x40) >> 6);
	pump_position( 55, MaxY-10, (Waardes[17] & 0x80) >> 7);
	pump_position( 340, 115, (Waardes[16] & 0x01));
	pump_position( 340, 185, (Waardes[16] & 0x10) >> 4);
	pump_position( 340, 255, (Waardes[17] & 0x01));
	HighliftPumps(225, 125,0);
	HighliftPumps(225, 195,1);
	HighliftPumps(225, 265,2);
	}

file_open()
	{

	char karakter[256], *wcr_env;
	time_t mytime;
	time( &mytime);

	my_time  = localtime(&mytime);
	wcr_env = getenv( "WCR_DAT");
	sprintf( karakter, "%spr3%02d_%02d.doc",wcr_env, my_time->tm_mon+1, my_time->tm_mday);
	if ( (fp_output = fopen( karakter, "at+")) ==0)
		if ( (fp_output = fopen( karakter, "wt")) ==0)
			{
			error(2);
			exit(2);
			}
	 }
/*									*/
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
	 int address, val, loop_counter;
	 unsigned char data;

	 outportb( BASE+10, adc);
	 outportb ( BASE+9,GainGUCA[adc]);
	 outportb( BASE+11, 1);  /* trigger adc */
	 outportb( BASE+12, 0);  /* trigger adc */
	 do
		 {
		 data = inportb (BASE+5);
		 data &= 0x10;
		 }while ( data >0);
	 data =inportb (BASE+5);
	 data &= 0x0f; /** mask off un wanted bits **/
	 val =(int) data <<8;
	 val +=inportb (BASE+4);
	 return (val);
	 }

void check_data()
	{

	int counter;
	char tmp_buf[513];
	unsigned char high, low;
	float TempF, LevelF;

	LevelF =(float) 3/v_maks[0];
	LevelF *=Waardes[0];

	if( old_minute != MIN)
		{
		file_open();
		memset ( rx_buffer, 0 , sizeof( rx_buffer));

		sprintf( tmp_buf, "%d Hr", TimeOn1GL/*3600*/);
		OuttextXY(380, 107, tmp_buf, strlen(tmp_buf), YELLOW);
		sprintf( tmp_buf, "%d Hr", TimeOn2GL/*3600*/);
		OuttextXY(380, 177, tmp_buf, strlen(tmp_buf), YELLOW);
		sprintf( tmp_buf, "%d Hr", TimeOn3GL/*3600*/);
		OuttextXY(380, 247, tmp_buf, strlen(tmp_buf), YELLOW);
		sprintf( tmp_buf, "%02d:%02d", my_time->tm_hour, my_time->tm_min);
		strcat ( rx_buffer, tmp_buf);
		for ( counter =0; counter < DATA_BYTES; counter++)
			{
			memset( tmp_buf, 0, sizeof( tmp_buf));
			sprintf ( tmp_buf, " %d", Waardes[counter]);
			strcat ( rx_buffer, tmp_buf);
			}

		memset( tmp_buf, 0, sizeof( tmp_buf));
		TempF =(float) MAKS_FLOW/v_maks[0];
		FLOW =(float) (FLOW/peela_c);
		FLOW *=60;
		FLOW /=1000;
		FLOW *=TempF;
		FLOWT += FLOW;
		PRESSURET1 = PRESSURE1/pressure_c1;
		PRESSURET2 = PRESSURE2/pressure_c2;
		PRESSURET3 = PRESSURE3/pressure_c3;
		sprintf ( tmp_buf, "%12.1f", FLOWT);
		OuttextXY(5, 297, tmp_buf, strlen(tmp_buf), YELLOW);
		memset( tmp_buf, 0, sizeof( tmp_buf));
		sprintf ( tmp_buf, " %.1f %.2f %.2f %.2f\n", FLOWT, PRESSURET1, PRESSURET2, PRESSURET3);
		strcat ( rx_buffer, tmp_buf);

		old_minute = strlen( rx_buffer);
		old_minute = fwrite( rx_buffer, sizeof( char), old_minute, fp_output);
		fflush( fp_output);
		fclose ( fp_output);
		PRESSURE1= PRESSURE2=PRESSURE3=0;
		peela_c = pressure_c1 = pressure_c2=pressure_c3 =0;
		old_minute = MIN;
		}
	if( old_day != DAY)
		{
		old_day = DAY;
		}
	}

void Get_data( )
	{

	char *pchar;
	int LOOPS,	loops;

	for( loops=0; loops < 16; loops++)
		Waardes[loops] = read_adc(loops);
	for( loops=0; loops < 16; loops++)
		{
		if (Waardes[ loops] < v_min[loops])
			v_error[loops] =1;
		else
			v_error[loops] =0;
		Waardes[loops] -= Off_set[ loops];
		if( Waardes[loops] > -15  && Waardes[loops] < 0)
			Waardes[loops] = 0;
		}
	if( Waardes[3] >0)
		{
		FLOW +=Waardes[3];
		peela_c++;
		}
	if( Waardes[4] >0)
		{
		PRESSURE1 +=Waardes[4];
		pressure_c1++;
		}
	if( Waardes[5] >0)
		{
		PRESSURE2 +=Waardes[5];
		pressure_c2++;
		}
	if( Waardes[6] >0)
		{
		PRESSURE3 +=Waardes[6];
		pressure_c3++;
		}
	Waardes[loops++] = inportb( BASE+6); /** read dig i/p 0 - 7 **/
	Waardes[loops] = inportb( BASE+7); /** read dig i/p 8 - 15 **/
	if( ((Waardes[loops] & 0x80) >> 8) > 0)
		O_P1 &= 0x7f;
	else
		O_P1 |=80;
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
	int maks=1023, level=0, display, kbd_loops=0;
	double percentage=0;

	level = (int)(maks/100* Valve_position);
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
			xpos=MaxX-101;ypos=MaxY-121;
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
			if ( CLICK_TYPE[test_counter] == PuMP)
				{
				on_off(10, 10);
				} else if ( CLICK_TYPE[test_counter] == MOTOR)
					{
					on_off(10, 10);
					} else if ( CLICK_TYPE[test_counter] == VALVE)
						{
						percentage=0;
						xpos = 15 ; ypos =113;
						do
							{
							setcolor( WHITE);
							setfillstyle ( SOLID_FILL, WHITE);
						/**   xpos  ypos width         text  value in %    type**/
  						percentage = (float)level/maks;
							percentage *=100;
							display =(int) percentage;
/*											xpos	ypos	width height alarm text  value   type*/
				display_level ( xpos, ypos, 9    , 88   , 75 ,  100, display, 'l');
				sprintf( tmp_buf, "%5.2f%", percentage);
				Outtextxy ( MaxX-90, MaxY-115, tmp_buf, 6);
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
												 level+= maks/100;
											 if (level > 1023)
												 level =1023;
											break;
											case 'P':
											 if ( level > 0)
												 level -= maks/100;
											break;
									 }
								} else if ( kar == 0x0d)
									{
									Valve_position= percentage;
									Waardes[7] = level;
									kar =0;
									}
							} else
								{
								kbd_loops++;
								if (kbd_loops > 300)
									kar =0;
								}
							} while( kar != 0x0);
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

prem4()
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

	ypos =99;
	draw_tank ( xpos-1, ypos, 150, 150);
	ypos +=143;
	xpos +=150;
	setlinestyle( l_settings.linestyle,l_settings.upattern ,THICK_WIDTH);
	line( xpos, ypos, xpos+112, ypos);
	setlinestyle( l_settings.linestyle,l_settings.upattern ,NORM_WIDTH);
	xpos +=175;
	ypos +=30;
	line( xpos-63, ypos-140, xpos-63, ypos);
	draw_pumps( xpos+15, ypos-140, 1);
	line( xpos-63, ypos-140, xpos-45, ypos-140);
	rectangle ( xpos+5, ypos-145, xpos+10, ypos-165);
	line( xpos+8, ypos-145, xpos+8, ypos-140);
	draw_pumps( xpos+15, ypos-70, 1);
	line( xpos-63, ypos-70, xpos-45, ypos-70);
	rectangle ( xpos+5, ypos-75, xpos+10, ypos-95);
	line( xpos+8, ypos-75, xpos+8, ypos-70);
	draw_pumps( xpos+15, ypos, 1);
	line( xpos-63, ypos, xpos-45, ypos);
	rectangle ( xpos+5, ypos-5, xpos+10, ypos-25);
	line( xpos+8, ypos-5, xpos+8, ypos);
	rectangle ( 4, 285, 114, 310);
	line( xpos+15, ypos-140, xpos+15, ypos+75);
	rectangle ( xpos+22, ypos+75, xpos+5, ypos+95);
	line( xpos+15, ypos+95, xpos+15, ypos+125);
	arrow(xpos+15, ypos+125, 'd');
	xpos -=367;
	line( xpos, ypos+75, xpos, ypos+4);
	line( xpos, ypos+95, xpos, ypos+125);
	arrow(xpos, ypos+125, 'd');
	setcolor(EGA_LIGHTMAGENTA);
	xpos += 2; ypos = 10;
	outtextxy( (MaxX-129)/2, ypos+20 , "DEVELOPED BY");
	outtextxy( (MaxX-130)/2, ypos+30 , "INFOTRANS CC");

	settextstyle( my_settings.font, HORIZ_DIR, 2);
	outtextxy( (MaxX-330)/2  , ypos      , "WITSAND PUMP STATION 2");
	setcolor(EGA_WHITE);
	settextstyle( my_settings.font, HORIZ_DIR, 1);
	setcolor(EGA_LIGHTCYAN);
	outtextxy( xpos+70, ypos+65 , PUMP);
	outtextxy( xpos+110, ypos+65 , SUMP);
	outtextxy( xpos+70, ypos+75 , "0-3 meter");
	outtextxy( xpos+305, ypos+135 , HIGHLIFT);
	outtextxy( xpos+305, ypos+145 , PUMP);
	outtextxy( xpos+305, ypos+205 , HIGHLIFT);
	outtextxy( xpos+305, ypos+218 , PUMP);
	outtextxy( xpos+305, ypos+278 , HIGHLIFT);
	outtextxy( xpos+305, ypos+288 , PUMP);
	outtextxy( xpos+340, ypos+145 , "1");
	outtextxy( xpos+340, ypos+218 , "2");
	outtextxy( xpos+340, ypos+288 , "3");
	outtextxy( xpos+385, ypos+120 , "Pressure");
	outtextxy( xpos+385, ypos+130 , "Tansdcer");
	outtextxy( xpos+385, ypos+190 , "Pressure");
	outtextxy( xpos+385, ypos+200 , "Tansdcer");
	outtextxy( xpos+385, ypos+260 , "Pressure");
	outtextxy( xpos+385, ypos+270 , "Tansducer");
	outtextxy( xpos+270, ypos+328 ,  HEADMETER);
	outtextxy( 5, MaxY-40         ,  "DOOR");
	outtextxy( 5, MaxY-30         ,  "POS");
	outtextxy( 40, MaxY-30        ,  "IRRIG");
	outtextxy( 90, MaxY-30        ,  "P Start T");
	outtextxy( 190, MaxY-30        ,  "P Stop T");
	outtextxy( 6, 287             ,  "Total Flow Kl");

	}

stoppump(int StopAll)
	{

	int  MinPump, MinPumpPI, TempI, PumpsRunningIA[4]= {1,2,3};
	long TimeOn1L, TimeOn2L, TimeOn3L, TempL;

	TimeOn1L = TimeOn1GL;
	TimeOn2L = TimeOn2GL;
	TimeOn3L = TimeOn3GL;

	if( TimeOn3L > TimeOn2L)
		{
		TempL = TimeOn2GL;
		TimeOn2L = TimeOn3L;
		TimeOn3L = TempL;
		TempI = PumpsRunningIA[1];
		PumpsRunningIA[1] = PumpsRunningIA[2];
		PumpsRunningIA[2]= TempI;
		}

	if( TimeOn2L > TimeOn1L)
		{
		TempL = TimeOn1GL;
		TimeOn1L = TimeOn2L;
		TimeOn2L = TempL;
		TempI = PumpsRunningIA[0];
		PumpsRunningIA[0] = PumpsRunningIA[1];
		PumpsRunningIA[1]= TempI;
		}

	if( TimeOn2L < TimeOn3L)
		{
		TempL = TimeOn3L;
		TimeOn3L = TimeOn2L;
		TimeOn2L = TempL;
		TempI = PumpsRunningIA[2];
		PumpsRunningIA[2] = PumpsRunningIA[1];
		PumpsRunningIA[1]= TempI;
		}

	if( PumpsRunningGI ==3)
		MinPump=0;
	else if( PumpsRunningGI==1)
		MinPump=2;
	else
		MinPump=1;
	MinPump = PumpsRunningIA[MinPump];
	if ( StopAll  < 0)
		{
		O_P1 &= 0xFE;
		Pump1RunningGI=0;
		TimeLowLevel1GI=0;
		O_P1 &= 0xFE;
		Pump2RunningGI=0;
		TimeLowLevel2GI=0;
		O_P1 &= 0xFE;
		Pump3RunningGI=0;
		TimeLowLevel3GI=0;
	 } else
		 {
		switch ( MinPump)
			{
			case 1:
				O_P1 &= 0xFE;
				Pump1RunningGI=0;
				TimeLowLevel1GI=0;
			break;

			case 2:
				O_P1 &= 0xFD;
				Pump2RunningGI=0;
				TimeLowLevel2GI=0;
			break;

			case 3:
				O_P1 &= 0xFB;
				Pump3RunningGI=0;
				TimeLowLevel3GI=0;
			break;
			}
		 }
		if( PumpsRunningGI > 0)
			PumpsRunningGI--;
	}

sortpump()
	{
	int TempI;
	long TimeOn1L, TimeOn2L, TimeOn3L, TempL;

	TimeOn1L = TimeOn1GL;
	TimeOn2L = TimeOn2GL;
	TimeOn3L = TimeOn3GL;

	if( TimeOn3L < TimeOn2L)
		{
		TempL = TimeOn2GL;
		TimeOn2L = TimeOn3L;
		TimeOn3L = TempL;
		TempI = PumpsRunningGIA[1];
		PumpsRunningGIA[1] = PumpsRunningGIA[2];
		PumpsRunningGIA[2]= TempI;
		}

	if( TimeOn2L < TimeOn1L)
		{
		TempL = TimeOn1GL;
		TimeOn1L = TimeOn2L;
		TimeOn2L = TempL;
		TempI = PumpsRunningGIA[0];
		PumpsRunningGIA[0] = PumpsRunningGIA[1];
		PumpsRunningGIA[1]= TempI;
		}

	if( TimeOn2L > TimeOn3L)
		{
		TempL = TimeOn3L;
		TimeOn3L = TimeOn2L;
		TimeOn2L = TempL;
		TempI = PumpsRunningGIA[2];
		PumpsRunningGIA[2] = PumpsRunningGIA[1];
		PumpsRunningGIA[1]= TempI;
		}
	}

startpump( )
	{

	long MinTimeL;
	int PumpsOkAI[3]= {1,1,1}, MinPumpPI, PumpsRuningI, MinPumpI;
	char TestC;

	TestC = Waardes[16] &0x0f;
	if( TestC  <14 )
		{
		PumpsOkAI[0] = 0;
		}
	TestC = Waardes[16] &0xf0;
	TestC >>= 4;
	TestC &= 0x0f;
	if( TestC  < 14 )
		{
		PumpsOkAI[1] = 0;
		}
	TestC = Waardes[17] &0x0f;
	if( TestC  < 14 )
		{
		PumpsOkAI[2] = 0;
		}
	if (      PumpsOkAI[0] ==0&& DelayTimeGI ==0&& Pump1RunningGI ==0)
		MinPumpI =1;
	else if ( PumpsOkAI[1]==0 && DelayTimeGI ==0 && Pump2RunningGI ==0)
		MinPumpI =2;
	else if ( PumpsOkAI[2] ==0&& DelayTimeGI ==0&& Pump3RunningGI ==0)
		MinPumpI =3;
	else
			MinPumpI =4;

/*	MinPumpI = PumpsRunningGIA[ PumpsRunningGI];*/

	switch ( MinPumpI)
		{
		case 1:
		if( PumpsOkAI[0] ==0)
			{
			O_P1 |= 0x01;
			Pump1RunningGI=1;
			DelayTimeGI= DefaultDelayTimeGI;
			TimeLowLevel1GI=0;
			}

		break;

		case 2:
		if( PumpsOkAI[1] ==0)
			{
			O_P1 |= 0x02;
			Pump2RunningGI=1;
			DelayTimeGI= DefaultDelayTimeGI;
			TimeLowLevel2GI=0;
			}

		break;

		case 3:
		if( PumpsOkAI[2] ==0)
			{
			O_P1 |= 0x04;
			Pump3RunningGI=1;
			DelayTimeGI= DefaultDelayTimeGI;
			TimeLowLevel3GI=0;
			}
		break;
		}
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
	int TcharI;
	char kar, *char_p, temp_buf[2048], *char_b;
	char connect[] = { "c XXXXXX\r"}, TestC;
	char karakter[16];
	int MinPumpI, TtempI=2024, Ttemp1I =924;
	int Buffers, xpos=1,ypos, txpos=0, typos, tt, TestPump1I=0;
	int TestPump2I=0,TestPump3I=0;
	int old_sec=-1, counts, write_bytes, kbd_counter, char_send;
	unsigned int size, dodo=0;
	int Loops, size_of_write_buf, old_min=-1, err_tx, ttxpos, ttypos, TI;
/*	int TTI1, TTI2, TTI3, Start1OKI=0, Start2OKI=0,  Start3OKI=0, PumpsRuningI;*/

	float LevelF;


/*	TTI1 = TTI2 = TTI3 = 960;*/
	clrscr();
	outportb ( BASE+13, 0);
	outportb ( BASE+14, 0);
	memset( Waardes, 0, sizeof(Waardes));
	Initialize();
	size   = imagesize(1,1,100,120);
	menu   = malloc( size);
	size   = imagesize(1,1,20,20);
	cursor = malloc( size);
	size   = imagesize(1,1,100,50);
	scr1   = malloc( size);
	xpos = MaxX/2; ypos = MaxY/2;
	if ( argc < 13)
		{
		error(0);
		sleep(1);
		error(1);
		exit(1);
		}
	strncpy ( &connect[2], argv[1], 6);
	strcpy ( STATION_NAME, connect);
/** int port,int speed,int bits,char parity,int stops) **/

	 for ( Loops=0; Loops< 5; Loops++)
		 v_level[Loops] = atof( argv[Loops+5]);

	DelayTimeGI =0;
	DefaultDelayTimeGI = atoi( argv[10]);
	TimeOn1GL = atol ( argv[11]);
	TimeOn2GL = atol ( argv[12]);
	TimeOn3GL = atol ( argv[13]);
/*
	TimeOn1GL *=3600;
	TimeOn2GL *=3600;
	TimeOn3GL *=3600;
*/	if( *argv[2] == '1')
		{
		if (initRS232(COM1,9600,8,'N',1))
		 exit(1);
		} else
		if (initRS232(COM2,9600,8,'N',1))
		 exit(1);
	tnc_port=select_port;

	LowLevelTimeGI= atoi( argv[4]);
	time( &mytime);
	my_time  = localtime(&mytime);
	old_day  = my_time->tm_mday;

	txpos=xpos-10; typos=ypos-10;
	clearviewport();
	prem4();
	getimage( txpos,typos,txpos+20,typos+20, cursor);
	getimage( MaxX-100    ,1       ,MaxX, 50, scr1);
	draw_cursor( xpos, ypos);
	time( &mytime);
	my_time = localtime(&mytime);
	ctime( &mytime);
	DAY = my_time->tm_mday;
	MIN = my_time->tm_min;
	old_minute = MIN;
	Get_data();
	display_data();
	for(;;)
		{
		time( &mytime);
		my_time = localtime(&mytime);
		ctime( &mytime);
		DAY = my_time->tm_mday;
		MIN = my_time->tm_min;
		LoopsGI++;
		if (old_min != my_time->tm_min)
			{
			old_min = my_time->tm_min;
			Loops = 10000;
			clearviewport();
			prem4();
			display_data();
			sprintf( temp_buf, "%d", LoopsGI);
			Outtextxy ( 10, MaxY-115, temp_buf, 6, 0);
			my_time = localtime(&mytime);
			char_p = ctime( &mytime);
			LoopsGI=0;
			Outtextxy( 15, 335, char_p, strlen( char_p)-6,0);
			FLOW +=Waardes[3];
			PRESSURE1 +=Waardes[4];
			PRESSURE2 +=Waardes[5];
			PRESSURE3 +=Waardes[6];
			peela_c++;
			pressure_c1++;
			pressure_c2++;
			pressure_c3++;
			}
		Get_data();
/*
		Waardes[0] = TtempI;
		for (TI =1;TI<15;TI++)
			Waardes[TI] = Ttemp1I;
		Waardes[4] = TTI1;
		Waardes[5] = TTI2;
		Waardes[6] = TTI3;
*/
		check_data();
		outportb ( BASE+13, O_P1);
		outportb ( BASE+14, O_P2);
		if (old_sec != my_time->tm_sec)
			{
			gotoxy(1,1);
			printf( "%d %d %d %d", PumpsRunningGI,Pump1RunningGI , Pump2RunningGI, Pump3RunningGI);
			TestC = Waardes[16] &0x0f;
			if( TestC  ==10 && Pump1RunningGI==1)
				TimeOn1GL++;
			TestC = Waardes[16] &0xf0;
			TestC >>= 4;
			TestC &= 0x0f;
			if( TestC  ==10 && Pump2RunningGI==1)
				TimeOn2GL++;
			TestC = Waardes[17] &0x0f;
			if( TestC  ==10 && Pump3RunningGI==1)
				TimeOn3GL++;
			display_text();
			time( &mytime);
			my_time = localtime(&mytime);
			char_p  = ctime( &mytime);
			Outtextxy( 15, 335, char_p, strlen( char_p)-6, 0);
			old_sec = my_time->tm_sec;

			LevelF =(float) MAKS_PRESSURE/v_maks[4];
			LevelF *= Waardes[4];
			if( LevelF < v_level[0]) /** pressure < 250 kpa **/
				{
				if ( TimeLowLevel1GI++ > LowLevelTimeGI &&Pump1RunningGI!=0)
					{
					O_P1 &= 0xFE;
					Pump1RunningGI=0;
					TestPump1I= 1;
					TimeLowLevel1GI=0;
					}
				} else if( LevelF > v_level[1]) /** pressure > 1500 kpa **/
					{
					if ( TimeLowLevel1GI++ > LowLevelTimeGI &&Pump1RunningGI!=0)
						{
						O_P1 &= 0xFE;
						Pump1RunningGI=0;
						TestPump1I= 1;
						TimeLowLevel1GI=0;
						}
					}else
					 {
					 TimeLowLevel1GI=0;
					 TestPump1I= 0;
					 }
			TestPump1I=1;
			LevelF =(float) MAKS_PRESSURE/v_maks[5];
			LevelF *= Waardes[5];
			if( LevelF < v_level[0]) /** pressure < 250 kpa **/
				{
				if ( TimeLowLevel2GI++ > LowLevelTimeGI &&Pump2RunningGI!=0)
					{
					O_P1 &= 0xFd;
					Pump2RunningGI=0;
					TestPump2I=1;
					TimeLowLevel2GI=0;
					}
				} else if( LevelF > v_level[1]) /** pressure > 1500 kpa **/
					{
					if ( TimeLowLevel2GI++ > LowLevelTimeGI &&Pump2RunningGI!=0)
						{
						O_P1 &= 0xFd;
						Pump2RunningGI=0;
						TestPump2I=1;
						TimeLowLevel2GI=0;
						}
					}else
					 {
					 TimeLowLevel2GI=0;
					 TestPump2I= 0;
					 }
			LevelF =(float) MAKS_PRESSURE/v_maks[6];
			LevelF *= Waardes[6];
			if( LevelF < v_level[0]) /** pressure < 250 kpa **/
				{
				if ( TimeLowLevel3GI++ > LowLevelTimeGI &&Pump3RunningGI!=0)
					{
					O_P1 &= 0xFb;
					Pump3RunningGI=0;
					TestPump3I=1;
					TimeLowLevel3GI=0;
						}
				} else if( LevelF > v_level[1]) /** pressure > 1500 kpa **/
					{
					if ( TimeLowLevel3GI++ > LowLevelTimeGI &&Pump3RunningGI!=0)
						{
						O_P1 &= 0xFb;
						Pump3RunningGI=0;
						TestPump3I=1;
						TimeLowLevel3GI=0;
						}	else
					TestPump3I=0;
					} else
					 {
					 TimeLowLevel3GI=0;
					 TestPump3I= 0;
					 }
/*			Pump1RunningGI+ Pump2RunningGI+ Pump3RunningGI;*/
			LevelF =(float) 3/v_maks[0];
			LevelF *= Waardes[0];
			if( LevelF > v_level[2] && ( TestPump1I ==0 || TestPump2I ==0 || TestPump3I ==0 ))
				 {
				 if( DelayTimeGI > 0 && PumpsRunningGI<3)
						DelayTimeGI--;
				 LevelF =(float) 3/v_maks[0];
				 LevelF *=Waardes[0];
				 if ( PumpsRunningGI ==0)
						sortpump();
				 if ( PumpsRunningGI < 1 && LevelF > v_level[2])
					 {
					 startpump();
					 } else if ((PumpsRunningGI < 2) && (DelayTimeGI <=0) && LevelF > v_level[3])
						 {
						 startpump();
						 } else if ( PumpsRunningGI < 3 && DelayTimeGI <= 0 && LevelF > v_level[4])
							{
							startpump();
							}
				 }
					LevelF =(float) 3/v_maks[0];
					LevelF *=Waardes[0];
					if ( PumpsRunningGI > 2 && LevelF < v_level[4] )
						{
						stoppump(0);
						} else if (   (PumpsRunningGI > 1) && LevelF < v_level[3])
						 {
						 stoppump(0);
						 } else if ( PumpsRunningGI > 0 && LevelF < v_level[2])
							{
							stoppump(-1);
							}
			TestC = Waardes[16] &0x0f;
			if( TestC  !=10 )
				{
				O_P1 &= 0xFE;
				Pump1RunningGI=0;
				}
			TestC = Waardes[16] &0xf0;
			TestC >>= 4;
			TestC &= 0x0f;
			if( TestC  !=10 )
				{
				O_P1 &= 0xFD;
				Pump2RunningGI=0;
				}
			TestC = Waardes[17] &0x0f;
			if( TestC  !=10 )
				{
				O_P1 &= 0xFB;
				Pump3RunningGI=0;
				}
			PumpsRunningGI = Pump1RunningGI+Pump2RunningGI+Pump3RunningGI;
			memset( temp_buf, 0 , sizeof( temp_buf));
			sprintf( temp_buf,"%d ", DelayTimeGI);
			OuttextXY( 110 ,MaxY-15, temp_buf, strlen( temp_buf), YELLOW);
	sprintf( temp_buf,"%d %d %d ", TimeLowLevel1GI, TimeLowLevel2GI, TimeLowLevel3GI);
			OuttextXY( 200 ,MaxY-15, temp_buf, strlen( temp_buf), YELLOW);
			}
		time( &mytime);
		my_time = localtime(&mytime);
		ctime( &mytime);
		if( recRS232(  &karakter[0], -1) !=0)
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
							fp_temp = fopen( "WITSAND.VAL", "wt");
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
							char_p = strchr( &rx_buffer[13], ',');
							Loops  = atoi(char_p);
							fclose( fp_output);
							fp_output = fopen( karakter, "rt");
							fseek ( fp_output,size_of_write_buf, SEEK_END );
							for ( counts=0; counts < Loops; counts++)
								{
								fread ( temp_buf, sizeof(char), size_of_write_buf, fp_output);
/* send data*/
								}
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
							for ( Loops=0; Loops < DATA_bytes; Loops++)
							 {
							 memset( temp_buf,0 ,sizeof( temp_buf));
							 sprintf( temp_buf," %d", Waardes[Loops]);
							 strcat( rx_buffer, temp_buf);
							 }
						 sprintf ( temp_buf, " %.2f", FLOWT);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %.2f", PRESSURET1);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %.2f", PRESSURET2);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %.2f", PRESSURET3);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %d", TimeOn1GL);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %d", TimeOn2GL);
						 strcat ( rx_buffer, temp_buf);
						 memset( temp_buf, 0, sizeof( temp_buf));
						 sprintf ( temp_buf, " %d&\r", TimeOn3GL);
						 strcat ( rx_buffer, temp_buf);
						 sendRS232( strlen( rx_buffer) ,  &rx_buffer);
					break;
					case 'V' :
					char_p = strchr( char_p, ' ')+1;
					break;
					}
					putimage ( 0, 0, menu, COPY_PUT);
					setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
					Loops = 1000;
					}
				}
			if(dodo++>25)
			{
			display_data();
			dodo=0;
			}
		TcharI = Waardes[17];
		TcharI &= 0x20;
		if( TcharI == 0)
			{
			O_P1 &= 0x80;
			O_P2 &= 0;
			}
		}
}