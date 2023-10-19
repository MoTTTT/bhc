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
#include "weer.h"

#define tx_buf_size 1024*2
#define CLICK_POS 36
#define PUMP  0
#define MOTOR 1
#define VALVE 2

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
char   STATION_NAME[32];
int    Waardes[32+17+32+3], LOOPS;
float  Valve_position=50;
double AspectRatio;		/** Aspect ratio of a pixel on the screen*/
int    GraphDriver;		/** The Graphics device driver		*/
int    GraphMode;		  /** The Graphics mode value		*/
int    MaxX, MaxY;		/** The maximum resolution of the screen */
int    MaxColors;		  /** The maximum # of colors available	*/
int    ErrorCode;		  /** Reports any graphics errors		*/
int    Counts, Min_interval, Maks_speed, Display_time, Maks_temp, Min_temp;
int    comport, id, tnc_num, p_mode, poled_loops;
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

reset_val( val_port, data)
int val_port, data;
	{
	char t_buf[53];
	unsigned char junk, t_char, errb;
	int rx_counter=0, loopies;
	unsigned int mw, e_mw, lopies, rx_byte;

	rx_counter =0;
	mw = 0x4000;
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

	t_char = recRS232( &errb);
	if ( errb == 0)
		{
		if (t_char == 6)
			{
			rx_counter = 2;
			t_buf[rx_counter++] = 0x14;
			t_buf[rx_counter++] = 2;
			junk   = data>>8;
			t_char = data;
			t_buf[rx_counter++] =(unsigned char) t_char;
			t_buf[rx_counter++] =(unsigned char) junk;
			t_buf[rx_counter] = crc_calc( 6, t_buf);
			sendRS232( 7, t_buf);
			t_char = recRS232( &errb);
			if ( t_char == 6)
				return (0);
			}
		}
	return(1);
	}


/** pole a station **/
int pole( start_add, words)
int start_add, words;
{

	char buf[tx_buf_size], *b_p, time_buf[19], errb;
	char t_buf[200];
	unsigned char junk, t_char;
	int t_c=0, lops=0, err=0, rx_counter=0, r_char=0, char_r;
	int loopies, loops=0;
	unsigned int mw, e_mw, lopies, rx_byte, value;


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
		t_char = recRS232( &errb);
		if ( errb == 0)
			{
			rx_buffer[ r_char++] = t_char;
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
										Waardes[LOOPS++] = value;
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

error( string)
int string;
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

on_off( xpos, ypos)
int xpos, ypos;
	{
	setcolor( RED);
	setfillstyle ( SOLID_FILL, RED);
	bar(  xpos, ypos, xpos+30, ypos+30);
	setcolor( GREEN);
	setfillstyle ( SOLID_FILL, GREEN);
	bar(  xpos+50, ypos, xpos+80, ypos+30);
	setcolor( WHITE);
	outtextxy ( xpos+10, ypos+10, "ON");
	outtextxy ( xpos+55, ypos+10, "OFF");
	}

execute_cr( xpos,ypos)
int xpos, ypos;
	{
	struct viewporttype t_type;
	struct linesettingstype l_type;
	char kar, tmp_buf[15];
	int test_counter=0, test_val=0, t1=1, t2=1, pstep;
	int maks=1023, level=0, display;
	double percentage=0, step;

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
			if ( CLICK_TYPE[test_counter] == PUMP)
				{
				on_off(10, 10);
				getchar();
				} else if ( CLICK_TYPE[test_counter] == MOTOR)
					{
					on_off(10, 10);
					getchar();
					} else if ( CLICK_TYPE[test_counter] == VALVE)
						{
						step =(double) 108/100;
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
									Waardes[82] = level;
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
FILE	*fp_output;
struct tm *my_time;
struct time new_time;
time_t mytime;
	char kar, *char_p, temp_buf[2048];
	char karakter[16];
	int modifier, Buffers, xpos,ypos, txpos=0, typos;
	int counts, old_pos, write_bytes, kbd_counter;
	unsigned int size;
	int old_min, Loops, old_day, old_hour, size_of_write_buf;
	int start_add[] = { 18646, 18678, 20646, 20846, 20878};
	int words[]     = { 17  , 17  , 18  , 16  ,19};

	xpos=10;
	ypos=50;
	for (counts=0; counts < 21; counts++)
		{
		X_POS_L[txpos]    = xpos;
		X_POS_R[txpos]    = xpos+10;
		Y_POS_L[txpos]    = ypos;
		Y_POS_R[txpos]    = ypos+10;
		CLICK_TYPE[txpos] = PUMP;
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
	strcat ( STATION_NAME, "\r");
/** int port,int speed,int bits,char parity,int stops) **/
	switch ( *argv[2])
	{
		case '1' :
		if (initRS232(COM1,9600,8,'N',2))
		 exit(1);
		comport=COM1;
		break;
		case '2' :
		if (initRS232(COM2,9600,8,'N',2))
		 exit(1);
		comport=COM2;
		break;
	}
	time( &mytime);
	my_time  = localtime(&mytime);
	old_min  = my_time->tm_min;
	old_hour = my_time->tm_hour;
	old_day  = my_time->tm_mday;
	sprintf( karakter, "wit%02d_%02d.dat", my_time->tm_mon+1, my_time->tm_mday);
	if ( (fp_output = fopen( karakter, "at")) ==0)
		if ( (fp_output = fopen( karakter, "wt")) ==0)
			{
			error(2);
			exit(2);
			}
	txpos=xpos-10; typos=ypos-10;
	hospital();
	witsand1();
	getimage( txpos,typos,txpos+20,typos+20, cursor);
	getimage( MaxX-100    ,1       ,MaxX, 50, scr1);
	draw_cursor( xpos, ypos);
	for(;;)
		{
		time( &mytime);
		my_time = localtime(&mytime);
		ctime( &mytime);
		LOOPS=0;
/*
		for ( Loops=0; Loops < 5; Loops++)
			{
			do
				{
				counts = pole( start_add[Loops], words[Loops]);
				if( counts != 0)
					if (poled_loops++ > 10)
					{
					setbkcolor( RED);
					clearviewport();
					error(3);
					} else
						poled_loops =0;
				} while ( counts !=0);
			}
*/
		witsand1();
		size_of_write_buf=0;
		if (old_min != my_time->tm_min)
			{
			sprintf( rx_buffer,"%02d:%02d", old_hour, old_min);
			size_of_write_buf=fwrite( rx_buffer, sizeof( char), strlen( rx_buffer),  fp_output);
			for ( Loops=0; Loops < 114; Loops++)
				{
				sprintf( rx_buffer," %05d", Waardes[Loops]);
				counts = strlen (rx_buffer);
				size_of_write_buf+=write_bytes = fwrite( rx_buffer, sizeof( char), counts, fp_output);
				if ( write_bytes != counts)
					for( counts =0; counts < 20; counts++)
						error(4);
				}
			size_of_write_buf+=fwrite( "\n", sizeof( char), 1,  fp_output);
			memset( Waardes, 0, sizeof(Waardes)-2);
			reset_val( 70, 1);
			old_min = my_time->tm_min;
			}
		if (old_day != my_time->tm_mday)
			{
			fclose( fp_output);
			reset_val( 70, 2);
			old_day = my_time->tm_mday;
			sprintf( karakter, "wit%02d_%02d.dat", my_time->tm_mon+1, my_time->tm_mday);
			if ( (fp_output = fopen( karakter, "at")) ==0)
				if ( (fp_output = fopen( karakter, "wt")) ==0)
					for( counts =0; counts < 20; counts++)
						error(4);
			}
		for( Loops=0; Loops<400; Loops++)
			{
			if ( check_wait_ri() != 0)
				{
				if (rx_packet ( rx_buffer) !=0)
					{
					send_ok();
					switch( rx_buffer[12])
						{
						case 'S':
							old_pos = atoi( &rx_buffer[13]);
							char_p = strchr( &rx_buffer[13], ',');
							Loops  = atoi(char_p);
							reset_val( old_pos, Loops);
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
							mytime = atol( &rx_buffer[13]);
							my_time  = localtime(&mytime);
							new_time.ti_min  = my_time->tm_min;
							new_time.ti_sec  = my_time->tm_sec;
							new_time.ti_hour = my_time->tm_hour;
							settime ( &new_time);
						break;
						}
					Loops=401;
					}
				}			if (kbhit() !=0)
				{
				do
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
							 }

					putimage( txpos      ,typos, cursor, COPY_PUT);
/*					putimage( MaxX-100    ,1    ,scr1   , COPY_PUT);*/
					sprintf( temp_buf, "%d %d", xpos,ypos);
/*					outtextxy( MaxX-100,10, temp_buf);*/
					txpos=xpos-10; typos=ypos-10;
					getimage( txpos,typos,txpos+20,typos+20, cursor);
					draw_cursor( xpos, ypos);
					kbd_counter=0;
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