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
#define MAKS_INT 97

extern int MaxX, MaxY;
extern int Waardes[];
#define STATION_NAME "WITSAND.VAL"



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
		setcolor(LIGHTGREEN);
		setfillstyle ( SOLID_FILL, LIGHTGREEN);
		} else if ( position == 'c')
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
	value /=devider;
	value*=100;
	xpos = (int) value;
	return(xpos);
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

display_data( refresh)
int refresh;
	{
	char date_buf[256], p_pos[]= "ocn";
	int txpos, xpos,test;
	int maks[MAKS_INT];
	float barv_length, value;

	FILE *fp_maks;

	sprintf( date_buf, STATION_NAME);
	fp_maks = fopen( date_buf, "rt");
	if( fp_maks == 0)
		{
		error(2, date_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT; txpos++)
		{
		fscanf (  fp_maks, "%s ", date_buf);
		maks[txpos] = atoi( date_buf);
		}

	for ( txpos =0; txpos< MAKS_INT; txpos++)
		if( Waardes[txpos] >  maks[ txpos])
			 Waardes[txpos] =  maks[ txpos];

	txpos = 373;
	display_level( txpos    , 225       ,  7, 84, 70, 14, val_con(41, 821), 't');
	barv_length = Waardes[41];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH", barv_length);
	Outtextxy (  311 , 208 , date_buf, 6);
	barv_length = Waardes[40];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH",  barv_length);
	Outtextxy (  216 , 208 , date_buf, 6);
	barv_length = Waardes[39];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH",  barv_length);
	Outtextxy (  121 , 208 , date_buf, 6);

	txpos -= 95;
	display_level( txpos    , 225       ,  7, 84, 70, 14, val_con(40, 821), 't');
	txpos -= 95;
	display_level( txpos    , 225       ,  7, 84, 70, 14, val_con(39, 821), 't');
	display_level ( 365     , 356       ,  7, 90, 70, 14, val_con(42, 821), 't');
	barv_length = Waardes[42];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH",  barv_length);
	Outtextxy (  308 , 310 , date_buf, 6);
	display_level ( 98      , (MaxY- 9) ,  7, 90, 70, 14, val_con(44, 821), 't');
	barv_length = Waardes[44];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH",  barv_length);
	Outtextxy (  30 , 460 , date_buf, 6);
	display_level ( 2       , 180       ,  7, 90, 70, 14, val_con(43, 821), 't');
	barv_length = Waardes[43];
	barv_length /= 821;
	barv_length *= 14;
	sprintf ( date_buf, "%3.1f PH",  barv_length);
	Outtextxy (  8 , 75 , date_buf, 6);
	xpos =558;
	if( Waardes[45] > 1023)
		Waardes[45] = 1023;
	if( Waardes[48] > 1023)
		Waardes[48] = 1023;

	display_level ( xpos-1  ,  99+88    , 70, 86, 75,  2, val_con(48, 1023), 'l');
	display_level ( xpos-52 , 333       , 72, 88, 90,  3, val_con(91, 1023), 'l');
	barv_length =(float) 3/1023;
	barv_length *= Waardes[91];
	sprintf ( date_buf, "%3.1f m", barv_length);
	Outtextxy (  xpos-30 , 293 , date_buf, 5);

	barv_length =(float) 2/1024;
	barv_length *= Waardes[48];
	sprintf ( date_buf, "%3.1f m", barv_length);
	Outtextxy (  xpos+5 , 99+ 48 , date_buf, 5);
	if( Waardes[46] > 329)
		Waardes[46] = 329;
	display_level ( 407     , (MaxY-21) ,169, 92, 85, 4, val_con(46, 329), 'l');
	barv_length =(float) 4/329;
	barv_length *= Waardes[46];
	sprintf ( date_buf, "%4.2f m", barv_length);
	Outtextxy (  455 , (MaxY-70) , date_buf, 6);
	display_level ( xpos-308, (MaxY-46) , 59, 79, 98,  3, val_con(45, 1024), 'l');
	barv_length =(float) 3/1024;
	barv_length *= Waardes[45];
	sprintf ( date_buf, "%3.1f m", barv_length);
	Outtextxy (  xpos -300 , (MaxY-76) , date_buf, 5);
	display_level ( xpos-196, (MaxY-66) ,  7, 30, 70,100, val_con(82, 4096), 't');

	if ( refresh ==1)
		{
/** dealk head meter flow **/
		barv_length = flow_rate(33);
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( 400, 98 , date_buf, 10);

/** degas meter flow **/
		barv_length = flow_rate(32);
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( xpos-3, MaxY-255  , date_buf, 10);

/** waste  INLET !! meter flow **/
		barv_length = Waardes [36];
		barv_length *= 100;
		barv_length /= 60;
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( 97, 115  , date_buf, 10);

/** waste outlet meter flow **/
		barv_length = Waardes [37];
		barv_length *= 100;
		barv_length /= 60;
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( 2, 212  , date_buf,  10);

/** HEAD METER OUTLET FLOW **/
		barv_length = flow_rate(34);
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( 144, 467, date_buf,  10);

/** BACK WASH HEAD METER FLOW **/
		barv_length = flow_rate(35);
		barv_length /=10;
		sprintf ( date_buf, "%5.1f L/s", barv_length);
		Outtextxy ( 403 , 165 , date_buf, 10);
		}

	barv_length = Waardes[38];
	barv_length *=( 2500/(1023-204));
	sprintf ( date_buf, "%5.1f L/s", barv_length);
	Outtextxy ( 170, 270, date_buf, 10);

/** MUX pumps **/
	xpos= pump_on ( Waardes[79]);
	pump_position(  20,      120, p_pos[xpos]);
	xpos= pump_on ( Waardes[80]);
	pump_position(  60,      120, p_pos[xpos]);
	xpos= pump_on ( Waardes[81]);
	pump_position( 292, MaxY- 17, p_pos[xpos]);

/** high lift pumps  */
	test = Waardes[88];
	test &= 256;
	xpos= pump_on ( test);
	pump_position( 190, MaxY- 40, p_pos[xpos]);
	test = Waardes[88];
	test &= 512;
	xpos= pump_on ( test);
	pump_position( 190, MaxY- 70, p_pos[xpos]);
	test = Waardes[88] & 0x0400;
	test &=1024;
	xpos= pump_on ( test);
	pump_position( 190, MaxY-100, p_pos[xpos]);

/*treated pumps */
	xpos= pump_on ( Waardes[5]);
	pump_position( 480, MaxY-141, p_pos[xpos]);
	xpos= pump_on ( Waardes[4]);
	pump_position( 480, MaxY-171, p_pos[xpos]);
	xpos= pump_on ( Waardes[3]);
	pump_position( 480, MaxY-201, p_pos[xpos]);

	xpos= pump_on ( Waardes[2]);
	pump_position( 507, MaxY-292, p_pos[xpos]);
	xpos= pump_on ( Waardes[1]);
	pump_position( 507, MaxY-322, p_pos[xpos]);
	xpos= pump_on ( Waardes[0]);
	pump_position( 507, MaxY-352, p_pos[xpos]);
	bore_hole_pos ();
	fclose ( fp_maks);
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

witsand1()
	{

	char kar, text[13], date_buf[128];
	char *boorgat[] =
		{
		"30969","30975","34006","34007","34011","34971","34006","33107","34001",
		"34008","30981","34003","34013","33014","34010","34012","33091","30972",
		"30973","33103","34104","","","","",""
		};

	char *char_p;
	int counts, min, y_half, devider, width, xpos=15, txpos;
	int y_pos, degrees, from ,to, counter;
	float barv_length, barv_pos, tt, travel, vtravel, Degrees;
	struct textsettingstype my_settings;
	struct viewporttype	tmp_type;
	time_t temp_time;
	struct tm *local_time;

	gettextsettings (&my_settings);
	setbkcolor( EGA_BLUE);
	counts = 33;
	setcolor( EGA_WHITE);
	rectangle(0,0,MaxX,MaxY);
	setfillstyle(SOLID_FILL, LIGHTGRAY);
	setcolor( WHITE);

	for ( counts=0;counts < 25; counts++)
		{
		circle( xpos, 55, 5);
		line( xpos,59,xpos,69);
		xpos +=25;
		}

	line( 13,69,xpos-27,69);
	xpos -=47;
	line( xpos,69,xpos,99);
	arrow( xpos, 99, 'd');
	xpos -=35;
	line( xpos, 165, xpos-41, 165);
	draw_tank ( xpos-1, 99, 71, 90);
	draw_tank ( xpos-51, 245, 71, 90);
	draw_tank ( xpos-151, MaxY-114, 170, 94);
	line( xpos-20, 165, xpos-20, 175);
	draw_valve( xpos-27, 175,2, 'v');
	line( xpos-20, 205, xpos-20, 190);
	rectangle ( xpos-25,205, xpos-15, 225);
	line( xpos-20, 225, xpos-20, 245);
	arrow( xpos-20, 245, 'd');
	xpos= 517;
	draw_pumps( xpos, 125, 3);
	line( xpos-PUMP_WIDTH, 125, xpos-85, 125);
	rectangle ( xpos-85,119, xpos-105, 131);
	line( xpos-105, 125, xpos-115, 125);
	line( xpos-PUMP_WIDTH, 155, xpos-85, 155);
	rectangle ( xpos-85,149, xpos-105, 161);
	line( xpos-105, 155, xpos-115, 155);
	draw_valve( xpos-130, 119,1, 'h');
	txpos = xpos-210;
	draw_tank_round( txpos, 140, 70, 85);
	line ( txpos+30, 225, txpos+30, 235);
	line ( txpos+30, 140, txpos+30, 127);
	txpos -= 95;
	draw_tank_round( txpos,140,70,85);
	line ( txpos+30, 140, txpos+30, 127);
	line ( txpos+30, 225, txpos+30, 235);
	txpos -= 95;
	draw_tank_round( txpos,140,70,85);
	draw_tank_round( 7,225,70,85);
	line( 85, 145, 85,235);
	line( 68, 235, 85,235);
	line( 85, 120, 85, 95);
	circle(85, 90, 5);
	rectangle( 80,120,90,145);
	rectangle( 35,180,45,205);
	line( 40,  205, 40, 225);
	line( 42,  120, 42, 115);
	line( 22,  115, 42, 115);
	arrow(22, 115, 'l');
	rectangle( 34,115,37,101);
	line ( 35, 101, 42,  91);
	draw_pumps_vert( 30,120, 2);
	line( 40, 169, 40, 180);
	line( xpos-115, 155, xpos-115, 232);
	line( xpos-115, 239, xpos-115, 243);
	line( 68      , 243, xpos-115, 243);
	txpos +=30;
	line ( txpos, 140, txpos, 127);
	line ( txpos, 225, txpos, 235);
	line ( txpos, 127, xpos-130, 127);
	line ( txpos, 235, xpos, 235);
	line ( xpos, 235, xpos, 245);
	arrow( xpos, 245, 'd');
	xpos = 490;
	draw_pumps( xpos, 276, 3);
	rectangle( xpos-PUMP_WIDTH, 335, xpos-PUMP_WIDTH-25, 338);
	line( xpos-PUMP_WIDTH-25, 336, xpos-50-PUMP_WIDTH, 336);
	arrow( xpos-50-PUMP_WIDTH, 336, 'l');
	line( xpos, 330, xpos+19, 330);
	line( xpos-PUMP_WIDTH, 336, xpos-PUMP_WIDTH, MaxY-115);
	arrow( xpos-PUMP_WIDTH, MaxY-115, 'd');
	draw_pumps( 200, 376, 3);
	line( 200-PUMP_WIDTH, 435, 200-PUMP_WIDTH, 445);
	rectangle( 200-PUMP_WIDTH, 434, 185-PUMP_WIDTH, 437);
	line( 185-PUMP_WIDTH, 435, 172-PUMP_WIDTH, 435);
	arrow( 172-PUMP_WIDTH, 435, 'l');
	rectangle( 195-PUMP_WIDTH, 445, 205-PUMP_WIDTH, 465);
	line( 200-PUMP_WIDTH, 465, 200-PUMP_WIDTH, 475);
	arrow( 200-PUMP_WIDTH, 475, 'd');
	draw_pumps( 300, 460, 1);
	line( 300,460,330,460);
	line( 300-PUMP_WIDTH,402,300-PUMP_WIDTH,460);
	draw_tank ( 330, 425, 20, 40);
	line( xpos-180, 402, xpos-150, 402);
	draw_tank ( xpos-240, 353, 60, 82);
	line( xpos-240, 402, xpos-288, 402);
	draw_valve( xpos-150, 395, 2, 'h');
	line( xpos-135, 402, xpos-110, 402);
	line( xpos-110, 402, xpos-110, 456);
	line( xpos-110, 456, xpos-85, 456);
	y_pos = 348;
	setcolor( EGA_LIGHTGRAY);
	xpos = 100;/* was 40*/
	line ( xpos- 10, y_pos-40, xpos+ 25, y_pos+12);
	line ( xpos- 30, y_pos-40, xpos- 10, y_pos-40);
	line ( xpos    , y_pos+12, xpos+ 65, y_pos+12);
	line ( xpos+  7, y_pos-14, xpos+ 35, y_pos-30);
	line ( xpos+ 10, y_pos-10, xpos+ 45, y_pos-30);
	line ( xpos+ 35, y_pos-30, xpos+ 45, y_pos-30);
	line ( xpos+ 45, y_pos-30, xpos+ 45, y_pos-38);
	line ( xpos+ 35, y_pos-30, xpos+ 35, y_pos-38);
	line ( xpos+ 35, y_pos-38, xpos+ 45, y_pos-38);
	line ( xpos+ 35, y_pos-38, xpos+135, y_pos-88);
	line ( xpos+ 45, y_pos-38, xpos+145, y_pos-88);
	line ( xpos+135, y_pos-88, xpos+145, y_pos-88);
	line ( xpos+145, y_pos-88, xpos+145, y_pos-98);
	xpos  +=155;
	y_pos -=58;
	line ( xpos-130, y_pos+70, xpos+  8, y_pos-14);
	line ( xpos- 10, y_pos-40, xpos+  8, y_pos-14);
	line ( xpos- 50, y_pos-40, xpos- 10, y_pos-40);
	line ( xpos+  8, y_pos-14, xpos+ 25, y_pos-14);
	y_pos +=58;

	setcolor(EGA_LIGHTCYAN);
	outtextxy( xpos-125, 270      , "WEIR");
	outtextxy( xpos-125, 280      , "Ultrasonic flow");
	outtextxy( xpos-125, 290      , "measurement");

	xpos  +=178;

	outtextxy( xpos+128, 87       , "0,25 MEG");
	outtextxy( xpos-25 , 78       , "DEALK HEAD");
	outtextxy( xpos-25 , 88       , "METER");
	outtextxy( xpos+48 , 94       , "Untreated");
	outtextxy( xpos+48 , 104      , PUMP);
	outtextxy( xpos-05 , 245      , "Treated");
	outtextxy( xpos-05 , 255 			, PUMP);
	outtextxy( xpos-20 , 174      , "Back");
	outtextxy( xpos-20 , 184      , "wash");
	outtextxy( xpos-20 , 194      , "Metre");
	outtextxy( xpos+120, MaxY-275 , DEGAS );
	outtextxy( xpos+120, MaxY-265 , HEADMETER);
	outtextxy( 425     , MaxY- 10 , "1 MEG RESERVOIR");
	outtextxy(  95     ,  85      , "G30966");
	outtextxy(  90     ,  95      , "Waste Pond");
	outtextxy(  90     , 105      , "Dilution METER");
	outtextxy(  50     , 188      , "Waste");
	outtextxy(  50     , 198      , "outlet");
	outtextxy( 385     , 290      , "PH");
/*
	outtextxy( 385     , 350      , DEGAS);
	outtextxy( 385     , 360      , SUMP);
*/
	outtextxy( 533     , 340      , DEGAS);
	outtextxy( 533     , 350      , SUMP);
	outtextxy( 230     , MaxY-38  , "Caustic Soda");
	outtextxy( 233     , MaxY-9   , "Dosing Pump");
	outtextxy( xpos-167, MaxY-150 , PUMP);
	outtextxy( xpos-167, MaxY-140 , SUMP);
	outtextxy( xpos-267, MaxY-125 , "Pumps");
	outtextxy( xpos-267, MaxY-135 , HIGHLIFT);
	outtextxy(xpos/2-20, 118      , "DEALK");
	outtextxy(xpos/2+30, 118      , "SOFTNERS");
	outtextxy(  19     , 260      , "WASTE");
	outtextxy(  19     , 270      , "POND");
	outtextxy(  28     , MaxY- 50 , "PH WATER");
	outtextxy(  28     , MaxY- 40 , "Pumped");
	outtextxy(  28     , MaxY- 30 , "away");
	outtextxy( 144     , MaxY- 25 , HEADMETER);
	xpos = 311;
	for( counts = 3; counts>0; counts--)
		{
		outtextxy ( xpos  , 168, "  PH");
		outtextxy ( xpos-1, 178, "SOFTNER");
		sprintf ( date_buf, " No. %d", counts);
		outtextxy ( xpos, 188, date_buf);
		xpos -=95;
		}
	xpos=15;
	gettextsettings (&my_settings);
	settextstyle( my_settings.font, VERT_DIR, 1);

	for ( counts=0;counts < 25; counts++)
		{
		outtextxy ( xpos, 5, boorgat[counts]);
		xpos +=25;
		}
	settextstyle( my_settings.font, HORIZ_DIR, 1);

	setcolor(EGA_LIGHTMAGENTA);
	outtextxy( 220  , 95 , "DEVELOPED BY");
	outtextxy( 220  ,105 , "INFOTRANS CC");

	settextstyle( my_settings.font, HORIZ_DIR, 2);
	outtextxy( 200  , 75      , "WITSAND 1");
	setcolor(EGA_WHITE);
	settextstyle( my_settings.font, HORIZ_DIR, 1);
	display_data(0);
	}

silver2()
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
	xpos+=175;
	draw_tank ( xpos-1, ypos, 150, 150);
	xpos+=175;
	draw_tank ( xpos-1, ypos, 90, 150);
	xpos -= 200;
	ypos +=143;
	setlinestyle( l_settings.linestyle,l_settings.upattern ,THICK_WIDTH);
	line( xpos, ypos, xpos+25, ypos);
	xpos +=175;
	line( xpos, ypos, xpos+25, ypos);
	xpos+=175;
	ypos +=30;

	setlinestyle( l_settings.linestyle,l_settings.upattern ,NORM_WIDTH);

	xpos += 30;
	ypos -= 35;
	draw_pumps( xpos, ypos, 2);
	line( xpos-PUMP_WIDTH, ypos, xpos-(PUMP_WIDTH+29), ypos);
	line( xpos, ypos, xpos, ypos+75);
	rectangle ( xpos+10, ypos+45, xpos+30, ypos+51);
	line( xpos+10, ypos+48, xpos,ypos+48);
	rectangle ( xpos-10, ypos+75, xpos+10, ypos+95);
	line( xpos, ypos+95, xpos, ypos+125);
	arrow(xpos, ypos+125, 'd');
	xpos -=367;
	rectangle ( xpos-10, ypos+75, xpos+10, ypos+95);
	line( xpos, ypos+75, xpos, ypos+4);
	line( xpos, ypos+95, xpos, ypos+125);
	arrow(xpos, ypos+125, 'd');
	setcolor(EGA_LIGHTMAGENTA);
	xpos += 2; ypos = 10;
	outtextxy( xpos+60, ypos+20 , "DEVELOPED BY");
	outtextxy( xpos+60, ypos+30 , "INFOTRANS CC");

	settextstyle( my_settings.font, HORIZ_DIR, 2);
	outtextxy( xpos-20  , ypos      , "HOSPITAL PUMP STATION");
	setcolor(EGA_WHITE);
	settextstyle( my_settings.font, HORIZ_DIR, 1);
	setcolor(EGA_LIGHTCYAN);
	outtextxy( xpos+190, ypos+65 , PUMP);
	outtextxy( xpos+240, ypos+65 , SUMP);
	outtextxy( xpos+200, ypos+75 , "0-4 meter");
	outtextxy( xpos+30 , ypos+65 , "Bulk Reservoir");
	outtextxy( xpos-145, ypos+65 , "Bulk Reservoir");
	outtextxy( xpos+45 , ypos+75 , "0-10 meter");
	outtextxy( xpos-130, ypos+75 , "0-10 meter");
	outtextxy( xpos+285, ypos+195 , HIGHLIFT);
	outtextxy( xpos+285, ypos+205 , PUMP);
	outtextxy( xpos+285, ypos+278 , HIGHLIFT);
	outtextxy( xpos+285, ypos+288 , PUMP);
	outtextxy( xpos+325, ypos+205 , "1");
	outtextxy( xpos+325, ypos+288 , "2");
	outtextxy( xpos+370, ypos+240 , "Pressure");
	outtextxy( xpos+370, ypos+250 , "Tansdcer");
	outtextxy( xpos+285, ypos+318 , "PELLA");
	outtextxy( xpos+285, ypos+328 ,  HEADMETER);
	outtextxy( xpos+15 , ypos+301 , "INDUSTRIAL");
	outtextxy( xpos+15 , ypos+311 ,  HEADMETER);
	getch();
	clearviewport();
	}