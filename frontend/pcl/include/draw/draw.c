#include <graphics.h>
#include <stdio.h>

#define MAKS_INT 16
#define RADIUS   9

extern int Waardes[], MaxX, MaxY;
extern char STATION_NAME[];

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

	sprintf( date_buf,"c:\\wcrc\\exe\\%s.val", STATION_NAME);
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
