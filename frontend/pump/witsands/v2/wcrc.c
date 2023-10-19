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
#define TES
#define DATA_BYTES 115
int HIGH_LIFT =-1, VALVE_MAN = 2;
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
int    Waardes[32+17+32+33+110], LOOPS;
int    WAARDES[32+17+32+33+110], LOOPS;
float  Valve_position=50;
double AspectRatio;		/** Aspect ratio of a pixel on the screen*/
int    GraphDriver;		/** The Graphics device driver		*/
int    GraphMode;		  /** The Graphics mode value		*/
int    MaxX, MaxY;		/** The maximum resolution of the screen */
int    MaxColors;		  /** The maximum # of colors available	*/
int    ErrorCode;		  /** Reports any graphics errors		*/
int    Counts, Min_interval, Maks_speed, Display_time, Maks_temp, Min_temp;
int    tnc_port, id, tnc_num, p_mode, poled_loops;
int    plc_port, select_port;
void *menu;

struct palettetype palette;		/** Used to read palette info	*/

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

reset_val( val_port, data, type)
int val_port, data;
char type;
	{
	char t_buf[1024];
	unsigned char junk, t_char, errb;
	int rx_counter=0, loopies;
	unsigned int mw, e_mw, lopies, rx_byte;

	select_port = plc_port;
	rx_counter =0;
/** 0x4000;*/
	if ( type == 'R')
		mw = 0x4000;
	else if ( type == 'V')
		mw = 0x51c0;   /** prop valve **/
	else if ( type == 'H')
		mw = 0x51ba;  /**pumps on off **/
	for( loopies = 0; loopies < val_port; loopies++)
		mw += 2;
	junk   = mw>>8;
	t_char = mw;
	e_mw   = mw+1;
/*
	printf( "%0.2x%0.2x %x %x\n", junk, t_char, mw,e_mw);
*/
	memset ( t_buf, 0, sizeof( t_buf));
	t_buf[rx_counter++] = 0x0c;
	t_buf[rx_counter++] = 0x03;
	t_buf[rx_counter++] = 0x83;
	t_buf[rx_counter++] = junk;
	t_buf[rx_counter++] = t_char;
	junk   = e_mw>>8;
	t_char = e_mw;
	t_buf[rx_counter++] = junk;
	t_buf[rx_counter++] = t_char;
	t_buf[rx_counter] = crc_calc( 7, t_buf);
	sendRS232( 8, t_buf);

	t_char = recRS232( &errb, 1);
	if ( errb == 0)
		{
		if (t_char == 6)
			{
			rx_counter = 2;
			t_buf[rx_counter++] = 0x14;
			t_buf[rx_counter++] = 2;
			junk   = data >> 8;
			t_char = data;
			t_buf[rx_counter++] =(unsigned char) t_char;
			t_buf[rx_counter++] =(unsigned char) junk;
			t_buf[rx_counter] = crc_calc( 6, t_buf);
			sendRS232( 7, t_buf);
			t_char = recRS232( &errb, 1);
			if ( t_char == 6)
				return (0);
			}
		}
	return(1);
	}


/** pole a station **/
int pole( start_add, words, type)
int start_add, words, type;
	{

	char *b_p, time_buf[256], errb;
	char t_buf[1024];
	unsigned char junk, t_char;
	int t_c=0, lops=0, err=0, rx_counter=0, r_char=0, char_r;
	int loopies, loops=0, check_bit =2;
	unsigned int mw, e_mw, lopies, rx_byte, value;

	if ( type != 1)
		mw = 0x4200;
/*
	for( loopies = 125; loopies < start_add; loopies++)
		mw += 2;
*/
	e_mw=mw=start_add;
	rx_counter =0;
	junk = mw>>8;
	t_char = mw;
	e_mw = mw+(words*2)+1;
	gotoxy(1,1);
/*
	printf( "%x %x %d", mw, e_mw, e_mw-mw);
*/
	memset( t_buf, 0, sizeof( t_buf));
	t_buf[rx_counter++] = 0x0c;
	t_buf[rx_counter++] = 0x03;
	t_buf[rx_counter++] = 0x82;
	t_buf[rx_counter++] = junk;
	t_buf[rx_counter++] = t_char;
	junk   = e_mw>>8;
	t_char = e_mw;
	t_buf[rx_counter++] = junk;
	t_buf[rx_counter++] = t_char;
	t_buf[rx_counter] = crc_calc( 7, t_buf);
	sendRS232( strlen( t_buf), t_buf);
	do
		{
		t_char = recRS232( &errb, 0);
		if ( errb == 0)
			{
			rx_buffer[ r_char++] = t_char;
			if( r_char > 1000)
				return(1);
			if( lops < 3)
				{
				if ( t_char == 0x0c)
					{
					lops++;
					} else if ( t_char == 0x00)
						lops++;
					else if ( t_char == 0x14)
						lops++;
					} else if ( lops == 3)
						{
						char_r =(int) t_char;
						lops ++;
						} else if ( lops > 3)
							{
							lops++;
							if ( t_c++ == char_r)
								{
								errb = crc_calc( r_char, t_buf);
								if ( errb == t_buf[ r_char])
									{
									rx_byte=4;
									do
										{
										value =( unsigned char)rx_buffer[rx_byte+1];
										value *= 256;
										value +=(unsigned char) rx_buffer[rx_byte++];
										rx_byte++;
										if ( type != 1)
											Waardes[LOOPS++] = value;
										else
											{
											for ( LOOPS=0; LOOPS<6; LOOPS++)
												{
												if (  (value & check_bit) <= 0)
													 Waardes[LOOPS] = 0;
												else
													Waardes[LOOPS] = 1;
												check_bit = check_bit <<1;
												}
											}
/*
					printf("%6d - MW %2d %4X!!\n", value, loops+loopies, mw+loops*2);
*/
										loops++;
										err = 199;
										t_char = 'M';
										} while ( rx_byte < rx_buffer[3]);
										gotoxy(1,1);
									}
								r_char=0;
								}
							}
			} else
				{
				err++;
				if ( err > 5)
					{
					gotoxy(1,24);
					err=rx_counter=lops=r_char=r_char=0;
					t_c=0;
					return(1);
					}
				}
		} while ( (t_char != 'M') );
	return(0);
	}

error( string, add_string)
int string;
char *add_string;
	{
	char *error_string[]=
		{
		"Program aborting",                       /* 0 */
		"Wrong parameters passed to program",     /* 1 */
		"DOS error could not open file",          /* 2 */
		"NO RESPONCE FROM PLC",                   /* 3 */
		"DOS error could not write to the file",  /* 4 */
		};
	int tone;

	setbkcolor (RED);
	clearviewport();
	setcolor( WHITE);
	outtextxy(200,200,error_string[string]);
	outtextxy(200,210, add_string);
	for ( tone = 800; tone > 400; tone -=100)
		{
		sound (tone);
		delay( 90);
		}
	nosound();
	}

get_data();

draw_cursor( xpos, ypos)
	{
	setcolor(EGA_YELLOW);
	line( xpos-10, ypos   , xpos+10, ypos);
	line( xpos   , ypos-10, xpos   , ypos+10);
	}

int on_off( xpos, ypos)
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
			return( level);
			}
		} else
			{
			kar=1;
			if ( kbd_loops++ > 19500)
				kar=0;
			}
		} while( kar != 0x0);
		return(-1);
	}

execute_cr( xpos,ypos)
int xpos, ypos;
	{
	struct viewporttype t_type;
	struct linesettingstype l_type;
	char kar, tmp_buf[15];
	int test_counter=0, test_val=0, t1=1, t2=1, pstep, tt;
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
/*			xpos = ypos = 3;*/
			floodfill( 3,3,LIGHTGRAY);
			getlinesettings( &l_type);
			setlinestyle ( SOLID_LINE, l_type.upattern, THICK_WIDTH);
			setcolor( WHITE);
			rectangle( 1,1, 99,119);
			setlinestyle ( l_type.linestyle, l_type.upattern, l_type.thickness);
			if ( CLICK_TYPE[test_counter] == PuMP)
				{
				tt = on_off(10, 10);
				} else if ( CLICK_TYPE[test_counter] == MOTOR)
					{
				tt = on_off(10, 10);
				if ( tt >= 0)
				 switch( test_counter)
					 {
						case  30 :
						 if ( tt ==0 )
							HIGH_LIFT |=1;
						 else
							HIGH_LIFT &= 0xfe;
					 break;
					 case 31 :
						 if ( tt ==0 )
						HIGH_LIFT |=2;
						 else
							HIGH_LIFT &= 0xfd;
					 break;
					 case 32 :
						 if ( tt ==0 )
							HIGH_LIFT |=4;
						 else
							HIGH_LIFT &= 0xfb;
					 break;
					 }
					VALVE_MAN=4;
					reset_val( 1, 1, 'H');
					reset_val( 1, HIGH_LIFT, 'H');

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
									VALVE_MAN=4;
									reset_val( 1,(int) 1, 'V' );
									Valve_position= percentage;
/*									WAARDES [82] =(int) percentage;*/
									percentage *=4096;
									percentage /= 100;
									tt = ( int) percentage;
									if( tt > 4095)
										tt=4095;
									reset_val( 0, tt, 'V');
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
/*	Begin main function						*/
/*									*/


int main(argc, argv)
int argc;
char *argv[];
	{

void *cursor, *scr1;
FILE	*fp_output, *fp_temp;
struct tm *my_time;
struct time new_time;
struct date new_date;
struct viewporttype t_type;
struct linesettingstype l_type;
time_t mytime;
	char kar, *char_p, temp_buf[2048], *char_b;
	char karakter[256];
	int AND[] = { 0xfeff, 0xfdff, 0xfbff};
	int modifier, Buffers, xpos,ypos, txpos=0, typos, err_tx;
	int counts, old_pos, write_bytes, kbd_counter, tt;
	unsigned int size;
	int old_min, Loops, old_day, old_hour, size_of_write_buf;
/*
	int start_add[] = { 18646, 18678, 20646, 20846, 20878};
	int words[]     = { 17   , 17   , 18  , 2   ,18}; *16 29*
*/
	int reset_set =1, ttxpos,ttypos;
	int end = 0;
	int start_add[] = { 18646, 18678, 20646, 20846, 20878};
	int words[]     = { 17    , 17    , 18    , 16   ,29}; /*16 29*/

	xpos=10;
	ypos=50;
	for (counts=0; counts < 21; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+10;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+10;
		CLICK_TYPE[txpos] = PuMP;
		txpos++;
		xpos+=25;
		}

	xpos=506;
	ypos=126;
	for (counts=0; counts < 3; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+7;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+7;
		CLICK_TYPE[txpos] = MOTOR;
		txpos++;
		ypos+=30;
		}

	xpos=479;
	ypos=277;
	for (counts=0; counts < 3; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+7;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+7;
		CLICK_TYPE[txpos] = MOTOR;
		txpos++;
		ypos+=30;
		}

	xpos=378;
	ypos=189;
	for (counts=0; counts < 3; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+7;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+7;
		CLICK_TYPE[txpos] = MOTOR;
		txpos++;
		ypos+=30;
		}

	xpos=189;
	ypos=379;
	for (counts=0; counts < 3; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+7;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+7;
		CLICK_TYPE[txpos] = MOTOR;
		txpos++;
		ypos+=30;
		}

	X_POS_L[txpos]    = 292;
	X_POS_R[txpos]    = 299;
	Y_POS_L[txpos]    = 462;
	Y_POS_R[txpos]    = 469;
	CLICK_TYPE[txpos] = MOTOR;
	txpos++;

	X_POS_L[txpos]    = 20;
	X_POS_R[txpos]    = 26;
	Y_POS_L[txpos]    = 119;
	Y_POS_R[txpos]    = 126;
	CLICK_TYPE[txpos] = MOTOR;
	txpos++;

	X_POS_L[txpos]    = 59;
	X_POS_R[txpos]    = 66;
	Y_POS_L[txpos]    = 119;
	Y_POS_R[txpos]    = 126;
	CLICK_TYPE[txpos] = MOTOR;
	txpos++;

	X_POS_L[txpos]    = 362;
	X_POS_R[txpos]    = 372;
	Y_POS_L[txpos]    = 382;
	Y_POS_R[txpos]    = 413;
	CLICK_TYPE[txpos] = VALVE;

	clrscr();
	memset( Waardes, 0, sizeof(Waardes));
	Initialize();
	size   = imagesize(1,1,100,120);
	menu   = malloc( size);
	size   = imagesize(1,1,20,20);
	cursor = malloc( size);
	size   = imagesize(1,1,100,50);
	scr1   = malloc( size);
	xpos = MaxX/2; ypos = MaxY/2;
	if ( argc < 3)
	{
		error(0);
		sleep(1);
		error(1);
		exit(1);
	}
	strcpy ( STATION_NAME, argv[1]);
/** int port,int speed,int bits,char parity,int stops) **/
	if (initRS232(COM1,9600,8,'N',1))
	 exit(1);
	tnc_port=select_port;
	if (initRS232(COM2,9600,8,'N',1))
	 exit(1);
/*	select_port = tnc_port;*/
/*	break_comms();*/
	plc_port=select_port;
	if( *argv[2] == '2')
		{
		plc_port = tnc_port;
		tnc_port = select_port;
		}


	time( &mytime);
	my_time  = localtime(&mytime);
	old_min  = my_time->tm_min;
	old_hour = my_time->tm_hour;
	old_day  = my_time->tm_mday;
	txpos=xpos-10; typos=ypos-10;
	witsand1();
	getimage( txpos,typos,txpos+20,typos+20, cursor);
	getimage( MaxX-100    ,1       ,MaxX, 50, scr1);
	draw_cursor( xpos, ypos);
	reset_val( 1, 0, 'V'); /** return valve to plc!!*/
	reset_val( 70, 1, 'R');
	reset_val( 70, 2, 'R');
	for(;;)
		{
		select_port = plc_port;

		getviewsettings(&t_type);
		ttxpos=5;ttypos=310;
		getimage ( ttxpos, ttypos, ttxpos+100, ttypos+120, menu);
		setviewport( ttxpos, ttypos, ttxpos+100, ttypos+ 60, 1);
		setfillstyle ( SOLID_FILL, MAGENTA);
		floodfill( 3,3,RED);
		getlinesettings( &l_type);
		setlinestyle ( SOLID_LINE, l_type.upattern, THICK_WIDTH);
		setlinestyle ( SOLID_LINE, l_type.upattern, NORM_WIDTH);
		setcolor( WHITE);
		rectangle( 1,1, 99, 59);
		setcolor( LIGHTGRAY);
		outtextxy (2, 10, "POLLING PLC");
		for ( Loops=0; Loops < 5; Loops++)
			{
			do
				{
				recRS232(&kar, -1);
				} while ( kar ==0);
			do
				{
			delay(100);
				counts = pole( start_add[Loops], words[Loops], 0);
				if( counts != 0)
					{
					if (poled_loops++ > 3)
						{
						setbkcolor( RED);
						clearviewport();
						error(3, "PLC PROBLEM!!");
						}
					} else
						poled_loops =0;
				} while ( counts !=0);
			}
		do
			{
			delay(100);
			counts = pole(  0x40d4, 2, 1);
			if( counts != 0)
				{
				if (poled_loops++ > 10)
					{
					setbkcolor( RED);
					clearviewport();
					error(3, "PLC PROBLEM!!");
					}
				} else
					poled_loops =0;
			} while ( counts !=0);

		setlinestyle ( SOLID_LINE, l_type.upattern, THICK_WIDTH);
		setlinestyle ( SOLID_LINE, l_type.upattern, NORM_WIDTH);
		setcolor( WHITE);
		rectangle( 1,1, 99, 59);
		setcolor( LIGHTGRAY);
		outtextxy (2, 10, "DONE POLLING PLC");
		putimage ( 0, 0, menu, COPY_PUT);
		setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);


		if( HIGH_LIFT < 0)
		 HIGH_LIFT = Waardes[88] >= 4;

		Waardes[ 87] = Waardes[ 84];
		Waardes[ 88] = Waardes[ 85];
		time( &mytime);
		my_time = localtime(&mytime);
		char_p = ctime( &mytime);
		LOOPS=0;
		char_b = strchr( char_p, ' ') +1;
		char_b = strchr( char_b, ' ') +1;
		char_b = strchr( char_b, ' ');
		*char_b =0;
		Outtextxy( 15, 325, char_p, strlen( char_p));
		witsand1();
		if (old_min != my_time->tm_min)
			{
			sprintf( karakter, "WIT_%02d.DOC", my_time->tm_mday);
			fp_output = fopen( karakter, "at");
			if( fp_output == 0)
				 fp_output = fopen( karakter, "wt");
			if (fp_output ==0)
				for (;;)
				error(2, karakter);
			if ( VALVE_MAN > 0)
				if ( VALVE_MAN-- ==1)
					reset_val( 1, 0, 'V'); /** return valve to plc!!*/
/*	mw = 0x51c0;   prop valve **/
			reset_set =0;
			select_port = plc_port;
			display_data(1);
			sprintf( rx_buffer,"%02d:%02d", old_hour, old_min);
			fwrite( rx_buffer, sizeof( char), strlen( rx_buffer),  fp_output);
			for ( Loops=0; Loops < DATA_BYTES+1; Loops++)
				{
				WAARDES[Loops] = Waardes[Loops];
				sprintf( rx_buffer," %d", Waardes[Loops]);
				counts = strlen (rx_buffer);
				write_bytes = fwrite( rx_buffer, sizeof( char), counts, fp_output);
				if ( write_bytes != counts)
					for( counts =0; counts < 20; counts++)
						error(4);
				}
			fwrite( "\n", sizeof( char), 1,  fp_output);
			memset( Waardes, 0, sizeof(Waardes));
			old_min = my_time->tm_min;
			fflush( fp_output);
			fclose ( fp_output);
			select_port = plc_port;
			reset_val( 70, 2, 'R');
			reset_val( 70, 1, 'R');
			}
		if (old_day != my_time->tm_mday)
			{
			select_port = plc_port;
			reset_val( 70, 2, 'R');
			old_day = my_time->tm_mday;
			sprintf( karakter, "WIT_%02d.DOC", my_time->tm_mday);
			if ( (fp_output = fopen( karakter, "wt")) ==0)
				{
				for( counts =0; counts < 20; counts++)
						error(4);
					exit(0);
				}
			fclose ( fp_output);
			if ( reset_set == 0)
				{
				reset_val( 70, 1, 'R');
				reset_set =1;
				}
			}
/*
		reset_val( 1, 1);
		reset_val( 0, 0);
		sleep(5);
		reset_val( 0, 0);
		reset_val( 1, 0);
*/
		size_of_write_buf=0;
/*		setport();*/
		select_port = tnc_port;
		for( Loops=0; Loops<50; Loops++)
			{
			time( &mytime);
			my_time = localtime(&mytime);
			ctime( &mytime);
			LOOPS=0;
			if (old_min != my_time->tm_min)
				{
				Loops = 10000;
				clearviewport();
				witsand1();
				break;
				}
			if ( rx_packet ( rx_buffer) ==0)
				{
				char_p = rx_buffer;
				while ( *char_p == 'U')
					char_p++;
/*				char_p = strchr( rx_buffer, 'O');*/
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
								HIGH_LIFT &= tt;
								} else
									 {
									 tt = 1<<tt;
									 HIGH_LIFT |= tt <<8;
									 }
							VALVE_MAN=4;
							reset_val( 1, 1, 'H');
							reset_val( 0, HIGH_LIFT, 'H');
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
/*											size_of_write_buf = sizeof( tx_buffer)+1;*/
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
/*												if( rx_buffer[counts] == 'U')*/
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
							old_pos = atoi( &rx_buffer[13]);
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
					for ( Loops=0; Loops < DATA_BYTES; Loops++)
						{
						memset( temp_buf,0 ,sizeof( temp_buf));
						sprintf( temp_buf," %d", WAARDES[Loops]);
						strcat( rx_buffer, temp_buf);
/*if( strlen( rx_buffer) > 250)
							{
							tx_packets( &rx_buffer , modifier, &tx_buffer);
							memset ( rx_buffer, 0, sizeof( rx_buffer));
							sleep(1);
							}*/
						}
						strcat (rx_buffer, "&\r");
						modifier = strlen( rx_buffer);
						tx_packets( &rx_buffer , modifier, &tx_buffer);
						end++;
						if( end < 0)
							end =0;
						break;
					case 'V' :
						char_p = strchr( char_p, ' ')+1;
						reset_val( 1, 1, 'V');  /* change to remote position **/
						reset_val( 0, atoi( char_p), 'V');  /* change to remote position **/
						VALVE_MAN = 4;
						break;
						}
					putimage ( 0, 0, menu, COPY_PUT);
					setviewport( t_type.left,t_type.top,t_type.right,t_type.bottom,t_type.clip);
					}
				}
		my_time = localtime(&mytime);
		char_p = ctime( &mytime);
		LOOPS=0;
		char_p = strchr( char_p, ' ') +1;
		char_p = strchr( char_p, ' ') +1;
		char_p = strchr( char_p, ' ') +1;
		Outtextxy( 15, 335, char_p, strlen( char_p)-6);
				if (kbhit() !=0)
				{
				do
					{
					if (kbhit() !=0)
					{
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
							 putimage( txpos      ,typos, cursor, COPY_PUT);
							 execute_cr( xpos, ypos);
							 kbd_counter = 10000;
							 }

					putimage( txpos      ,typos, cursor, COPY_PUT);
					sprintf( temp_buf, "%d %d", xpos,ypos);
					txpos=xpos-10; typos=ypos-10;
					getimage( txpos,typos,txpos+20,typos+20, cursor);
					draw_cursor( xpos, ypos);
					kbd_counter=0;
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
		}
 }