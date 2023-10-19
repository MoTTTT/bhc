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
#include "c:\wcrc\pela\pela.h"

#define RADIUS 9
#define MAKS_ALARM_LEV 16

extern int MaxX, MaxY;
extern int Waardes[];
extern int v_error[];
extern int v_min[];
extern int v_maks[];
extern int v_level[];
extern float  SixmeterfGF, EightmeterfGF, OutputfGF;
extern float EightMeterflowGF, SixMeterFlowGF;
extern int SixmeterGI, EightmeterGI, SumpLevelSet6GI, SumpLevelSet8GI;


int alarm_levels[ (MAKS_ALARM_LEV*2)+1];

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
		} else if ( position ==  2 )
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

float flow_rate( waarde)
	{
	float value;
	value = (float)Waardes[waarde];
	value *= 1000;
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

display_data()
	{
	char date_buf[256];
	int txpos, xpos, color, typos=50+100;
	float barv_length, value;
	int maks[MAKS_INT];
	enum { SOF3=0,SOF2=2,SOF1=4,TREAT_PH=6, PUMP_AWAY=8, WASTE_O=10,\
	MEG25=12, DE_GAS=14, MEG1=16, ACUASTI=18};

	FILE *fp_maks, *fp_level;

	fp_maks = fopen( "PELA.VAL", "rt");
	if( fp_maks == 0)
		{
		error(2, date_buf);
		exit(0);
		}

	for ( txpos =0; txpos< MAKS_INT; txpos++)
		fscanf (  fp_maks, "%d", &maks[txpos]);

	for ( txpos =0; txpos< DATA_BYTES; txpos++)
		if( Waardes[txpos] >  maks[ txpos])
			 Waardes[txpos] =  maks[ txpos];
	fclose ( fp_maks);

	fp_maks = fopen( "PELA.LEV", "rt");
	if( fp_maks == 0)
		{
		error(2, date_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_ALARM_LEV*2; txpos++)
		fscanf (  fp_maks, "%d ", &alarm_levels[txpos]);

	barv_length = Waardes[0]*10/6;
	Waardes[0] = (int) barv_length;
	barv_length = (float)Waardes[1]*10/7;
	Waardes[1] = (int) barv_length;
	fclose ( fp_maks);
	txpos = 34;
/**  6 meter level **/
	display_level( txpos    , typos    ,299,100,v_min[0], 6, val_con(0, v_maks[0]), 'l', 1);
	barv_length =(float) 6/MAKS_BITS;
	barv_length *= Waardes[0];
	if ( Waardes[0] < 0)
		color = 1;
	else
		 color=0;
	sprintf ( date_buf, "%4.2f m", barv_length);
	Outtextxy (  txpos+120 , typos-50, date_buf, 6, color);
	txpos += 175;
/** 8 meter level **/
	display_level( MaxX-136    , typos    ,100,100,v_min[1], 7, val_con(1, v_maks[1]), 'l',1);
	barv_length =(float) 7/MAKS_BITS;
	barv_length *= Waardes[1];
	sprintf ( date_buf, "%4.2f m", barv_length);
	if ( Waardes[1] < 0)
			color = 1;
	else
		 color=0;
	Outtextxy (  MaxX-(136-20), typos-50 , date_buf, 6, color);
	typos += 250;
	txpos +=60;
/** sump level **/
	display_level( txpos    , typos    , 150,100,v_min[2],  4, val_con(2, v_maks[2]), 'l',1);
	barv_length =(float) 4/MAKS_BITS;
	barv_length *= Waardes[2];
	if ( Waardes[2] < 0)
			color = 1;
	else
		 color=0;
	sprintf ( date_buf, " %4.2f", barv_length);
	strncpy( &date_buf[5] , " m", 4);
	Outtextxy (  txpos+55 , typos-50 , date_buf, 5, color);

/** MAG 6 meter flow **/
	barv_length =(float) MAKS_FLOW/MAKS_BITS;
	barv_length *=	Waardes[3];
	if ( Waardes[3] < 0)
			color = 1;
	else
		 color=0;
	sprintf ( date_buf, "%6.2f L/s", barv_length);
	SixMeterFlowGF = barv_length;
	Outtextxy ( 192, 220 , date_buf, 12, color);
/** MAG 6 meter flow **/
	barv_length =(float) MAKS_ATLANTIS/MAKS_BITS;
	barv_length *=	Waardes[6];
	if ( Waardes[6] < 0)
			color = 1;
	else
		 color=0;
	sprintf ( date_buf, "%6.2f L/s", barv_length);
/*	SixMeterFlowGF = barv_length;*/
	Outtextxy ( 514, 370, date_buf, 12, color);
	display_level( 274, 270 ,5,50,  10 , 100, SumpLevelSet6GI,'t',0);
	display_level( 281, 270 ,5,50 , 100, 100, SixmeterGI, 't',1);
	sprintf ( date_buf, "%2d %%", SixmeterGI);
	Outtextxy ( 280, 285 , date_buf, 5, 0);

/** MAG 8 meter flow **/
	barv_length =(float) MAKS_FLOW/MAKS_BITS;
	barv_length *= Waardes[4];
	if ( Waardes[4] < 0)
			color = 1;
	else
		 color=0;
	sprintf ( date_buf, "%6.2f L/s", barv_length);
	EightMeterflowGF = barv_length;
	Outtextxy ( 470,220  , date_buf, 10, color);
	display_level( 423, 270 ,5,50,  10, 100, SumpLevelSet8GI, 't',0);
	display_level( 430, 270 ,5,50, 100, 100, EightmeterGI, 't',1);

	sprintf ( date_buf, "%2d %%", EightmeterGI);
	Outtextxy ( 430, 285 , date_buf, 5, color);

/** MAMRE HEAD meter flow **/
	barv_length =(float) MAKS_MAMRE/MAKS_BITS;
	barv_length *= Waardes[5];
	if ( Waardes[5] < 0)
			color = 1;
	else
		 color=0;
	sprintf ( date_buf, "%6.2f L/s", barv_length);
	Outtextxy ( 180, 415  , date_buf, 10, color);
	}

display_text()
	{
	char date_buf[256];
	int txpos, xpos, color;
	float barv_length, value;
	txpos = 34;

/** MAG 6 meter flow **/
		barv_length =(float) MAKS_FLOW/MAKS_BITS;
		barv_length *= 	Waardes[2];
		if ( Waardes[2] < 0)
			color = 1;
		else
		 color=0;
		sprintf ( date_buf, "%6.2f L/s", barv_length);
		Outtextxy ( 192, 220 , date_buf, 10, color);

/** MAG 8 meter flow **/
		barv_length =(float) MAKS_FLOW/MAKS_BITS;
		barv_length *= Waardes[3];
		if ( Waardes[3] < 0)
			color = 1;
		else
		 color=0;
		sprintf ( date_buf, "%6.2f L/s", barv_length);
		Outtextxy ( 475, 220  , date_buf, 10, color);
/** MAMRE HEAD meter flow **/
		barv_length =(float) MAKS_MAMRE/MAKS_BITS;
		barv_length *= Waardes[5];
		if ( Waardes[5] < 0)
			color = 1;
		else
		 color=0;
		sprintf ( date_buf, "%6.2f L/s", barv_length);
		Outtextxy ( 180, 415  , date_buf, 10, color);
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
/*	setbkcolor( LIGHTGRAY);*/
	outtextxy( 1,3, string);
	setbkcolor (bk_color);
	setcolor   (f_color);
	setviewport( tmp_post.left , tmp_post.top   ,\
							 tmp_post.right, tmp_post.bottom, tmp_post.clip );

	}
							 /* xpos, ypos, width, hight, level*/
display_level( bar_left, bar_bottom, width, hight, alarm, maks, level, type, Text)
int bar_left, bar_bottom, width, hight,maks,alarm, level,Text;
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
	if( Text==1)
		{
		setcolor( EGA_LIGHTCYAN);
		barh_right +=3;
		outtextxy( barh_right, bar_bottom-4, "0");
		sprintf( text,"%d", maks);
		outtextxy( barh_right, barh_top, text);
		sprintf( text,"%d", maks/2);
		outtextxy( barh_right, bar_bottom-devider, text);
		}
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
	}