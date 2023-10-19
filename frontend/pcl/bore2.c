#include <bios.h>
#include <dir.h>
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
#include "c:\wcrc\include\weer.h"

#define RADIUS 9
#define MAKS_INT 16
#define MAKS_ALARM_LEV 16

extern int MaxX, MaxY;
extern int Waardes[];
extern int MaksGIA[];
extern int UnitsGIA[ MAKS_ALARM_LEV];

int alarm_levels[ MAKS_ALARM_LEV*2];
char *Lang_strings[]=
	 {
	 " PH",
	 " L/sec",
	 "METER",
	 "%",
	 };

pump_position(xpos, ypos, position)
int xpos, ypos;
char position;
	{
#define x_l 5
#define y_l 5

	setcolor ( WHITE);
	setfillstyle ( SOLID_FILL, LIGHTGRAY);
	if ( position == 'o')
		{
		setcolor(LIGHTRED);
		setfillstyle ( SOLID_FILL, LIGHTRED);
		} else if ( position == 'c')
			{
			setcolor(GREEN);
			setfillstyle ( SOLID_FILL, LIGHTGREEN);
			}
	bar ( xpos, ypos, xpos+x_l, ypos+y_l);
	}

float flow_rate( waarde)
	{
	float value;
	value = (float)Waardes[waarde];
	value *= 980;/*1000*/
	value /=60;
	return(value);
	}

int	pump_on ( position)
	{
	if (position > 0)
			return(1);
		else if ( position < 0)
			return (3);
		else return(0);
	}

valueconfersion(int Units, char *String, char *AddOnCP)
	{
	float TempF;

	TempF = Waardes[Units];
	TempF /= MaksGIA[Units];
	TempF *= UnitsGIA[Units];
	sprintf ( String, "%3.1f%s",  TempF, AddOnCP);
	}
display_data( )
	{
	enum { SOF3=0,SOF2=2,SOF1=4,TREAT_PH=6, PUMP_AWAY=8, WASTE_O=10,\
	MEG25=12, DE_GAS=14, MEG1=16, ACUASTI=18};
	char date_buf[256], p_pos[]= "ocn";
	int txpos, xpos,test;
	int TempI;
	float barv_length, value;

	for ( txpos=0; txpos<MAKS_INT; txpos++)
		{
		if( Waardes[txpos] >  MaksGIA[ txpos])
			 Waardes[txpos] =  MaksGIA[ txpos];
		}

	txpos = 373;

	display_level( MaxX-100      , 220       ,  7, 184, SOF3, 25, val_con(1,MaksGIA[12]), 't');
	valueconfersion( 1, date_buf," M");
	Outtextxy (  MaxX-85 , 140 , date_buf, 7);

/** MUX pumps **/
	TempI = Waardes[8];
	TempI&= 0xf;
	HighliftPumps(  MaxX-350, 155, TempI );

	valueconfersion( 0, date_buf," Kpa");
	Outtextxy ( MaxX-370, 130, date_buf, 10);
	xpos= pump_on ( Waardes[8]&1);
	pump_position( MaxX-275, 160, p_pos[xpos]);
	}

Outtextxy ( xpos, tpos, string, length)
int xpos, tpos, length;
char *string;
	{
	int char_height, char_width, bk_color, f_color;
	struct viewporttype tmp_post;

	getviewsettings( &tmp_post);
	char_height = textheight("H");
	char_width  = textwidth("H");
	setviewport( xpos, tpos, xpos+(length*char_width), tpos+char_height,1);
	bk_color = getbkcolor();
	f_color  = getcolor();
	setcolor(EGA_YELLOW);
	clearviewport();
	outtextxy( 1,1, string);
	setbkcolor (bk_color);
	setcolor   (f_color);
	setviewport( tmp_post.left , tmp_post.top   ,\
							 tmp_post.right, tmp_post.bottom, tmp_post.clip );

	}
							 /* xpos, ypos, width, hight, level*/
display_level( bar_left, bar_bottom, width, hight, alarm, maks, level, type)
int bar_left, bar_bottom, width, hight,maks,alarm, level;
char type;
	{

	char text[100];
	int barh_left = bar_left+1, barh_top = bar_bottom-hight;
	int barh_right = bar_left+width, barh_posb, barh_post;
	float devider= (float) hight /100;

	if ( type == 'l')
		{
		setfillstyle ( SOLID_FILL, EGA_BLUE);
		bar( bar_left+2, barh_top ,barh_right-1, bar_bottom );
		}else
			 {
			 setfillstyle ( SOLID_FILL, WHITE);
			 bar( bar_left, barh_top ,barh_right, bar_bottom );
			 }
	if ( level > alarm_levels[alarm+1] )
		setfillstyle ( SOLID_FILL, LIGHTRED);
	if ( level < alarm_levels[alarm])
		setfillstyle ( SOLID_FILL, LIGHTRED);

	if ( level > alarm_levels[alarm+1] &&  level < alarm_levels[alarm])
		setfillstyle ( SOLID_FILL, LIGHTGREEN);

	barh_post = bar_bottom -(level*devider);
	bar( barh_left+1,bar_bottom, bar_left+width-1, barh_post);
	devider *=50;
	setcolor( EGA_LIGHTCYAN);
	barh_right +=3;
	outtextxy( barh_right, bar_bottom-4, "0");
	sprintf( text,"%d", maks);
	outtextxy( barh_right, barh_top, text);
	sprintf( text,"%d", maks/2);
	outtextxy( barh_right, bar_bottom-devider, text);

	}

arrow( xpos,ypos, direction)
int xpos, ypos;
char direction;
	{
	if ( direction =='d')
		{
		line( xpos-4, ypos-5, xpos  , ypos);
		line( xpos,   ypos  , xpos+4, ypos-5);
		}else if ( direction == 'u')
			{
			line( xpos-4, ypos+5, xpos  , ypos);
			line( xpos,   ypos  , xpos+4, ypos+5);
			} else if ( direction == 'r')
				{
				line( xpos, ypos, xpos-5  , ypos-4);
				line( xpos, ypos, xpos-5, ypos+4);
				} else if ( direction == 'l')
					{
					line( xpos, ypos, xpos+5, ypos-4);
					line( xpos, ypos, xpos+5, ypos+4);
					}
	}

draw_pumps( xpos, ypos, pumps)
	{
	int minus = 30-(2*RADIUS), counter;

	if ( pumps >1)
		{
		line( xpos+5, ypos, xpos+5, (ypos+(pumps-1)*30) +30);
		line( xpos-(minus+(4*RADIUS))-15,\
				ypos, (xpos-(minus+(4*RADIUS)))-15, (ypos+(pumps-1)*30)+30);
		} else
			{
			line( xpos, ypos, xpos, ypos+(pumps-1)*30);
			line( xpos-(minus+(4*RADIUS))-15,\
				ypos, (xpos-(minus+(4*RADIUS)))-15, ypos+(pumps-1)*30);
			}
	for ( counter = 0; counter<pumps; counter++)
		{
		line( xpos-5, ypos, xpos-5, ypos-5);
		rectangle(  xpos-8,ypos-5,xpos-2,ypos-15);
		line( xpos+5, ypos, xpos-minus, ypos);
		circle(  xpos-(minus+RADIUS), ypos, RADIUS);
		circle(  xpos-(minus+(3*RADIUS)), ypos, RADIUS);
		outtextxy ( xpos-(minus+RADIUS)-2, ypos-2, "M");
		outtextxy ( xpos-(minus+(3*RADIUS))-2, ypos-2, "P");
		line( xpos-(minus+(4*RADIUS)), ypos, (xpos-(minus+(4*RADIUS))-15), ypos);
		ypos+=60;
		}
	}

draw_pumps_vert( xpos, ypos, pumps)
	{
	int minus = 24-(2*RADIUS), counter;

	line( xpos, ypos, xpos+((pumps-1)*24), ypos-15);
	line( xpos, ypos+(4*RADIUS)+(2*minus), xpos+(pumps-1)*24,\
				 ypos+(4*RADIUS)+(2*minus));
	for ( counter = 0; counter<pumps; counter++)
		{
		line( xpos, ypos, xpos, ypos+minus);
		line( xpos, ypos-5, xpos-5, ypos-5);
		rectangle(  xpos-5,ypos-8,xpos-20,ypos-2);
		circle(  xpos, ypos+RADIUS+minus, RADIUS);
		circle(  xpos, ypos+(3*RADIUS)+minus, RADIUS);
		line( xpos, ypos+(4*RADIUS)+minus, xpos, ypos+(4*RADIUS)+(2*minus));
		outtextxy ( xpos-3, ypos+RADIUS+minus-1    , "M");
		outtextxy ( xpos-3, ypos+(3*RADIUS)+minus-1, "P");
		xpos+=24;
		}
	}

OuttextXY ( xpos, tpos, string, length, color)
int xpos, tpos, length, color;
char *string;
	{
	int char_height, char_width, bk_color, f_color;
	struct viewporttype tmp_post;

	setcolor(color);
	getviewsettings( &tmp_post);
	char_height = textheight("H");
	char_width  = textwidth("H");
	setviewport( xpos, tpos-2, xpos+(length*char_width)+3, tpos+char_height+4,1);
	bk_color = getbkcolor();
	f_color  = getcolor();
	clearviewport();
/*	setbkcolor( LIGHTGRAY);*/
	outtextxy( 1,3, string);
 setbkcolor (bk_color);
	setcolor   (f_color);
	setviewport( tmp_post.left , tmp_post.top   ,\
							 tmp_post.right, tmp_post.bottom, tmp_post.clip );

	}

HighliftPumps(int XposI,int YposI,int PumpI)
	{
	char PumpPositionCA[][9] = {
		"   ON   ",
		"  FAULT ",
		" MANUAL ",
		"  AUTO  ",
		"   OFF  ",
		" NORMAL ",
		"ABNORMAL"
		};

	XposI -=5;
	setcolor(YELLOW);
	rectangle ( XposI-2,YposI-3,XposI+68, YposI+24);

	 switch( PumpI &0x02)
		 {
		 case 0:
		 OuttextXY (  XposI, YposI, PumpPositionCA[1], 8, LIGHTRED);
		 break;
		 default:
		 OuttextXY (  XposI, YposI, PumpPositionCA[5], 8, GREEN);
		 break;
		 }
	 switch( PumpI & 0x0c)
		 {
		 case 4:
		 OuttextXY (  XposI, YposI+11, PumpPositionCA[2], 8, LIGHTRED);
		 break;
		 case 8:
		 OuttextXY (  XposI, YposI+11, PumpPositionCA[3], 8, GREEN);
		 break;
		 default:
		 OuttextXY (  XposI, YposI+11, PumpPositionCA[6], 8, LIGHTRED);
		 break;
		 }
	}

bore_hole_pos()
	{
	int counter, T_color, xpos=15, pos1, pos2, pos, shift=1;

	T_color = getcolor();
	pos1 = Waardes[ 87];
	pos2 = Waardes[ 88];
	for( counter =1; counter <16; counter++)
		{
		pos =pos1 & shift;
		shift <<=1;
		if ( pos >0)
			{
			setcolor ( LIGHTRED);
			setfillstyle ( SOLID_FILL, LIGHTRED);
			} else if ( pos ==0)
			{
			setcolor( LIGHTGREEN);
			setfillstyle ( SOLID_FILL, LIGHTGREEN);
			} else
				{
				setcolor(LIGHTGRAY);
				setfillstyle ( SOLID_FILL, LIGHTGRAY);
				}
		pieslice( xpos, 55, 0, 360, 5);
		setcolor( WHITE);
		circle( xpos, 55, 5);
		xpos +=25;
		}
	shift =1;
	for( counter =1; counter <7; counter++)
		{
		pos =pos2 & shift;
		shift <<=1;
		if ( pos >0)
			{
			setcolor ( LIGHTRED);
			setfillstyle ( SOLID_FILL, LIGHTRED);
			} else if ( pos ==0)
			{
			setcolor( LIGHTGREEN);
			setfillstyle ( SOLID_FILL, LIGHTGREEN);
			} else
				{
				setcolor(LIGHTGRAY);
				setfillstyle ( SOLID_FILL, LIGHTGRAY);
				}
		pieslice( xpos, 55, 0, 360, 5);
		setcolor( WHITE);
		circle( xpos, 55, 5);
		xpos +=25;
		}
		pieslice( xpos, 55, 0, 360, 5);
		setcolor( WHITE);
		circle( xpos, 55, 5);
		xpos +=25;
	setcolor( T_color);
	}
draw_valve(xpos, ypos, type, dir)
int  xpos, ypos, type;
char dir;
	{
	if ( dir == 'h')
		{
		line( xpos, ypos,  xpos+15, ypos+15);
		line( xpos, ypos+15,  xpos, ypos);
		line( xpos+15, ypos+15,  xpos+15, ypos);
		line( xpos, ypos+15,  xpos+15, ypos);
		line( xpos+7, ypos+7,  xpos+7, ypos-4);
		if ( type ==1)
			line( xpos+2, ypos-4,  xpos+13, ypos-4);
		else
			 rectangle( xpos+2, ypos-4, xpos+13, ypos-7);
		} else
			{
			line( xpos, ypos,  xpos+15, ypos);
			line( xpos, ypos,  xpos+15, ypos+15);
			line( xpos+15, ypos,  xpos, ypos+15);
			line( xpos, ypos+15,  xpos+15, ypos+15);
			line( xpos+7, ypos+7,  xpos+16, ypos+7);
			if ( type ==1)
				line( xpos+16, ypos+2,  xpos+16, ypos+13);
			else
				 rectangle( xpos+16, ypos+2, xpos+20, ypos+13);
			}
	}
