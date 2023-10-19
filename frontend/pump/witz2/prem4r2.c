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
#include "prem4r.h"

#define RADIUS 9

extern int MaxX, MaxY;
extern int Waardes[DATA_BYTES];
extern int v_error[];
extern int v_min[];
extern int v_maks[];
extern int v_level[];
extern float	PELLA, INDUS, INDUS_C;
extern int 	peela_c, indus_c;


char *Lang_strings[]=
	 {
	 " PH",
	 " L/sec",
	 "METER",
	 "%",
	 };

pump_position(xpos, ypos, position)
int xpos, ypos, position;
	{
#define x_l 5
#define y_l 5

	setcolor ( WHITE);
	setfillstyle ( SOLID_FILL, LIGHTGRAY);
	if ( position == 1)
		{
		setcolor(LIGHTGREEN);
		setfillstyle ( SOLID_FILL, LIGHTGREEN);
		} else if ( position ==  0 )
			{
			setcolor(RED);
			setfillstyle ( SOLID_FILL, LIGHTRED);
			}
	bar ( xpos, ypos, xpos+x_l, ypos+y_l);
	}

int val_con( waarde, devider)
int waarde, devider;
	{
	float value;
	int xpos;

	value = (float)Waardes[waarde];
	if ( value < 0)
		value =0;
	value /=devider;
	value*=100;
	xpos = (int) value;
	return(xpos);
	}

int	pump_on ( position)
	{
	if (position > 0)
			return(1);
		else if ( position < 0)
			return (3);
		else return(0);
	}

display_data()
	{
	char date_buf[256];
	int txpos, xpos, color;
	float barv_length, value;

	txpos = 34;
	display_level( txpos    , 150+98    ,149,149,v_min[0], 3, val_con(0, v_maks[0]), 'l');
	barv_length =(float) 3/v_maks[0];
	barv_length *= Waardes[0];
	if ( Waardes[0] < 0)
		color = 1;
	else
	 color=0;
	sprintf ( date_buf, "%4.2f m", barv_length);
	Outtextxy (  txpos+55 , 99+75 , date_buf, 6, color);
	txpos += 175;
	txpos += 175;
	pump_position( 15,  MaxY-10, (Waardes[17] & 0x40) >> 6);
	pump_position( 55,  MaxY-10, (Waardes[17] & 0x80) >> 7);
	pump_position( 340, 115, (Waardes[16] & 0x01));
	pump_position( 340, 185, (Waardes[16] & 0x10) >> 4);
	pump_position( 340, 255, (Waardes[17] & 0x01));
	}

Outtextxy ( xpos, tpos, string, length, color)
int xpos, tpos, length, color;
char *string;
	{
	int char_height, char_width, bk_color, f_color;
	struct viewporttype tmp_post;

	if ( color)
		{
		strncpy( string, "XXXXXXXXXXXXXXX", length);
		setcolor(EGA_LIGHTRED);
		}	else
			 setcolor(EGA_YELLOW);
	getviewsettings( &tmp_post);
	char_height = textheight("H");
	char_width  = textwidth("H");
	setviewport( xpos, tpos-2, xpos+(length*char_width)+3, tpos+char_height+4,1);
	bk_color = getbkcolor();
	f_color  = getcolor();
	clearviewport();
	outtextxy( 1,3, string);
	setbkcolor (bk_color);
	setcolor   (f_color);
	setviewport( tmp_post.left , tmp_post.top   ,\
							 tmp_post.right, tmp_post.bottom, tmp_post.clip );

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
		bar( bar_left+2, barh_top-1, barh_right-1, bar_bottom );
		}else
			 {
			 setfillstyle ( SOLID_FILL, WHITE);
			 bar( bar_left, barh_top ,barh_right, bar_bottom );
			 }
	if ( level > alarm)
		setfillstyle ( SOLID_FILL, LIGHTGREEN);
	else
		setfillstyle ( SOLID_FILL, LIGHTRED);
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

	line( xpos, ypos, xpos, ypos+(pumps-1)*30);
	line( xpos-(minus+(4*RADIUS))-15,\
				ypos, (xpos-(minus+(4*RADIUS)))-15, ypos+(pumps-1)*30);

	for ( counter = 0; counter<pumps; counter++)
		{
		line( xpos, ypos, xpos-minus, ypos);
		circle(  xpos-(minus+RADIUS), ypos, RADIUS);
		circle(  xpos-(minus+(3*RADIUS)), ypos, RADIUS);
		outtextxy ( xpos-(minus+RADIUS)-2, ypos-2, "M");
		outtextxy ( xpos-(minus+(3*RADIUS))-2, ypos-2, "P");
		line( xpos-(minus+(4*RADIUS)), ypos, (xpos-(minus+(4*RADIUS))-15), ypos);
		ypos+=30;
		}
	}

draw_pumps_vert( xpos, ypos, pumps)
	{
	int minus = 24-(2*RADIUS), counter;

	line( xpos, ypos, xpos+((pumps-1)*24), ypos);
	line( xpos, ypos+(4*RADIUS)+(2*minus), xpos+(pumps-1)*24,\
				 ypos+(4*RADIUS)+(2*minus));
	for ( counter = 0; counter<pumps; counter++)
		{
		line( xpos, ypos, xpos, ypos+minus);
		circle(  xpos, ypos+RADIUS+minus, RADIUS);
		circle(  xpos, ypos+(3*RADIUS)+minus, RADIUS);
		line( xpos, ypos+(4*RADIUS)+minus, xpos, ypos+(4*RADIUS)+(2*minus));
		outtextxy ( xpos-3, ypos+RADIUS+minus-1    , "M");
		outtextxy ( xpos-3, ypos+(3*RADIUS)+minus-1, "P");
		xpos+=24;
		}
	}

draw_tank( xpos, ypos, xl, yl)
int xpos, ypos, xl,yl;
	{

	struct linesettingstype my_settings;

	getlinesettings (&my_settings);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,THICK_WIDTH);
	line( xpos, ypos,xpos, ypos+yl);
	line( xpos,ypos+yl,xpos+xl,ypos+yl);
	xpos +=xl;
	line( xpos, ypos, xpos, ypos+yl);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,my_settings.thickness );
	}

draw_tank_round( xpos, ypos, xl, yl)
int xpos, ypos, xl,yl;
	{

	int rr =9;
	struct linesettingstype my_settings;

	getlinesettings (&my_settings);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,THICK_WIDTH);
	line( xpos, ypos+rr,xpos, ypos+(yl-rr));
	arc( xpos+rr, ypos+rr, 90, 180, rr);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,THICK_WIDTH);
	line( xpos+rr,ypos+1,xpos+(xl-(2*rr)),ypos+1);
	arc( (xpos+xl)-(2*rr), ypos+rr, 0, 90, rr);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,THICK_WIDTH);
	line( xpos+xl-rr-1, ypos+rr, xpos+xl-rr-1, (ypos+yl) -rr);
	line( xpos+rr,ypos+yl,xpos+(xl-(2*rr)),ypos+yl);
	arc( xpos+rr, ypos+yl-rr, 180, 270, rr);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,THICK_WIDTH);
	arc( (xpos+xl)-2*rr, ypos+yl-rr, 270, 360, rr);
	setlinestyle( my_settings.linestyle,my_settings.upattern ,my_settings.thickness );
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
	int TempI;

	XposI -=5;
	setcolor(WHITE);
	rectangle ( XposI-2,YposI-3,XposI+68, YposI+31);
	if( PumpI ==1)
		TempI= Waardes[16] >> 4;
	else if ( PumpI == 2)
		TempI= Waardes[17] & 0xf;
	else
		TempI= Waardes[16] & 0xf;

	 switch( TempI &0x02)
		 {
		 case 0:
		 OuttextXY (  XposI, YposI, PumpPositionCA[1], 8, LIGHTRED);
		 break;
		 default:
		 OuttextXY (  XposI, YposI, PumpPositionCA[5], 8, GREEN);
		 break;
		 }
	 switch( TempI & 0x0c)
		 {
		 case 4:
		 OuttextXY (  XposI, YposI+18, PumpPositionCA[2], 8, LIGHTRED);
		 break;
		 case 8:
		 OuttextXY (  XposI, YposI+18, PumpPositionCA[3], 8, GREEN);
		 break;
		 default:
		 OuttextXY (  XposI, YposI+18, PumpPositionCA[6], 8, LIGHTRED);
		 break;
		 }
	}