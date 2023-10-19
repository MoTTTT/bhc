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
#include "bore.h"


unsigned char GainGUCA[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int ErrorGIA[]           = { 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000};
int MinGIA[]             = { 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000};
int OffsetGIA[]          = { 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000};
int MaksGIA[]            = { 2042, 2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042,2042};
int AlarmSetPointsGIA[]  = {95,35,95,35,95,35,95,35,95,35,95,35,95,35,95,35,95,35,75,20,95,35,95,35,95,35,95,35,95,35,95,35,95,35 };
int AlarmLevelsGIA[]     = {95,35,95,35,95,35,95,35,95,35,95,35,95,35,95,35,95,35,75,20,95,35,95,35,95,35,95,35,95,35,95,35,95,35 };
int UnitsGIA[]           = { 5,2,41,41,41,100,1000,1000,1000,1000,1000,1000,2,0,0,0};
int ModeGI=0;
float LitersPerMinuteGF;
unsigned char OutputByte1GUC=0, OutputByte2GUC=0;

char *BOX_NO[] = { "hadc1278"};
FILE	*fp_output;
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
int BASE=0x220,PumpsRunningGI=0; PumpsRunningGIA[] = {1,2,3};
int Pump1RunningGI=0, Pump2RunningGI=0, Pump3RunningGI=0;
int	X_POS_L[CLICK_POS+3];
int	X_POS_R[CLICK_POS+3];
int	Y_POS_L[CLICK_POS+3];
int	Y_POS_R[CLICK_POS+3];
int CLICK_TYPE[ CLICK_POS+3], DelayTimeGI=1600;
int VleiSilvGI=0, VleiResGI=0, ResSilvGI=0, FlowTGI=0;
float FlowMinF=0;
int AND[17];

char rx_buffer[tx_buf_size];
char tx_buffer[tx_buf_size];
char   STATION_NAME[32];
int LOOPS, old_minute=71;
float  Valve_position=50;
float FlowVleiSilvGF=0, FlowVleiResGF=0 ,FlowResSilvGF=0 ,FlowGF=0;
float FlowTGF;

double AspectRatio;		/** Aspect ratio of a pixel on the screen*/
int    GraphDriver;		/** The Graphics device driver		*/
int    GraphMode;		  /** The Graphics mode value		*/
int    MaxX, MaxY;		/** The maximum resolution of the screen */
int    MaxColors;		  /** The maximum # of colors available	*/
int    ErrorCode;		  /** Reports any graphics errors		*/
int    Counts, Min_interval, Maks_speed, Display_time, Maks_temp, Min_temp;
int    tnc_port, id, tnc_num, p_mode, poled_loops;
int    plc_port, select_port, old_day, DAY, MIN;
void *menu;
float XtravelGF, YtravelGF;
float FlowCounterGF=0, FlowsGFA[120];
float XLtravelGF, YLtravelGF;
float LevelCounterGF=0, LevelsGFA[120], PressureGFA[120];

int    Waardes[DATA_BYTES], LoopsGI=0;
unsigned char O_P1=0, O_P2=0;
int Switch[]  = { 0,0,0,0};

struct palettetype palette;		/** Used to read palette info	*/
struct tm *my_time;

int read_adc( adc)
	int adc;
	{
	int address, val, loop_counter;
	unsigned char data;

	outportb( BASE+10, adc);
	outportb ( BASE+9,GainGUCA[adc]);
	outportb( BASE+11, 1);  /* trigger adc */
	outportb( BASE+12, 0);  /* trigger adc */

/*	do
		{
		data = inportb (BASE+5);
		data &= 0x10;
		}while ( data >0);
*/	data =inportb (BASE+5);
	data &= 0x0f; /** mask off un wanted bits **/
	val =(int) data <<8;
	val +=inportb (BASE+4);
	return (val);
	}

file_open()
	{

	char karakter[256];
	time_t mytime;
	time( &mytime);

	my_time  = localtime(&mytime);
	sprintf( karakter, "%ssil%02d_%02d.doc", getenv( "WCR_DAT")\
													, my_time->tm_mon+1, my_time->tm_mday);
	if ( (fp_output = fopen( karakter, "at")) ==0)
		if ( (fp_output = fopen( karakter, "wt")) ==0)
			{
			error(2);
			exit(2);
			}
	 }

boretroom()
	{

	char kar, text[13], date_buf[128];

	char *char_p;
	int min, y_half, devider, width, xpos=15, txpos;
	int ypos, degrees, from ,to, counter;
	float barv_length, barv_pos, travel, vtravel, Degrees;
	struct textsettingstype my_settings;
	struct viewporttype	tmp_type;
	time_t temp_time;
	struct tm *local_time;

	gettextsettings (&my_settings);
	setbkcolor( EGA_BLUE);
	setcolor( EGA_WHITE);
	rectangle(0,0,MaxX,MaxY);
	setfillstyle(SOLID_FILL, LIGHTGRAY);

	xpos = 380;/* was 40*/
	ypos = 140;

	line (  xpos, ypos-15,xpos+70, ypos-15);
	draw_valve(xpos+70, ypos-22, 0, 'h');
	line (  xpos+85, ypos-15,xpos+120, ypos-15);
	arrow (  xpos+120, ypos-15, 'r' );
	draw_pumps_vert( xpos, ypos, 1 );
	line (  xpos, ypos+65,xpos, ypos+48);
	arrow (  xpos, ypos+55, 'u' );
	circle(xpos,ypos+70, 5 );
	line (95, MaxY-20,MaxX-5, MaxY-20);
	line (95, MaxY/2 ,95, MaxY-20);
	ypos = (MaxY/2)+9;
	YtravelGF = (MaxY/2)-29;
	barv_length = ( (MaxY/2)-20) /10;
	memset( text,0, sizeof(text));
	setcolor(GREEN);
	for( min=1000; min>0; min-=100)
		{
		line (95, ypos ,85, ypos);
		sprintf( text, "%2d", min);
		outtextxy( 5, ypos, text);
		ypos += (int) barv_length;
		}
	setcolor(YELLOW);
	ypos =(MaxY/2)+9;
	YtravelGF = (MaxY/2)-20;
	barv_length = ( (MaxY/2)-20) /5;
	memset( text,0, sizeof(text));
	for( min=25; min>0; min-=5)
		{
		line (95, ypos ,85, ypos);
		sprintf( text, "%2d", min);
		outtextxy( 40, ypos, text);
		ypos += (int) barv_length;
		}

	xpos =MaxX - 95;
	XtravelGF = 95;
	barv_length =(float) xpos/(2*60);
	xpos=95;
	setcolor(WHITE);
	memset( text,0, sizeof(text));
	for( min=120; min>0; min--)
		{
		line ( xpos,MaxY-20, xpos, MaxY-15);
		XtravelGF+= barv_length;
		xpos =(int ) XtravelGF;
		}
	XtravelGF = barv_length;
	outtextxy( 200, MaxY-10, "Last 120 Min");
	setcolor( EGA_LIGHTMAGENTA);
	ypos =(MaxY/2)+9;
	YtravelGF = (MaxY/2)-20;
	barv_length = ( (MaxY/2)-20) /10;
	memset( text,0, sizeof(text));
	for( min=20; min>0; min-=2)
		{
		line (95, ypos ,85, ypos);
		sprintf( text, "%2d", min);
		outtextxy(60, ypos, text);
		ypos += (int) barv_length;
		}
	outtextxy( 265  , 40 , "DEVELOPED BY");
	outtextxy( 265  , 50 , "INFOTRANS CC");

	settextstyle( my_settings.font, HORIZ_DIR, 2);
	outtextxy( 220  , 10      , "SILVERSTROOM");
	setcolor(EGA_WHITE);
	settextstyle( my_settings.font, HORIZ_DIR, 1);
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

	t_char = recRS232( &errb, 0);
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
			t_char = recRS232( &errb,0);
			if ( t_char == 6)
				return (0);
			}
		}
	return(1);
	}


/** pole a station **/
int pole( )
	{

	char *pchar, kar;
	int LOOPS,	loops=0, err=0, counts, SlaapI=1800;

	 send_ok();
	 delay(SlaapI);
	 pchar = rx_buffer;
	 memset( rx_buffer, 0 , sizeof( rx_buffer));
	 if ( rx_packet( rx_buffer) ==0)
		 {
		 do
			 {
			 if ( *pchar == ' ')
				 loops++;
			 } while ( *pchar++ != '&');
		 if( loops== MAKS_INT && *--pchar == '&')
			 {
			 *pchar = '\n';
			 LOOPS=0;
			 pchar = strchr( rx_buffer, ' ');
			 pchar++;
			 do
				{
				Waardes[LOOPS] = atoi( pchar);
				pchar = strchr( pchar, ' ');
				pchar++;
				} while ( LOOPS++ < MAKS_INT);
				pchar = rx_buffer;
			 } else
				 {
				 err++;
				 setbkcolor(LIGHTRED);
				 return(1);
				 }
		 }else
			 return(1);
	if( old_day != DAY)
		{
		fclose ( fp_output);
		old_day = DAY;
		}
	return(0);
}

error( string, name)
int string;
char *name;
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
	outtextxy(200,210, name );
	for ( tone = 800; tone > 400; tone -=100)
		{
		sound (tone);
		delay( 90);
		}
	nosound();
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

/*
void get_data( )
	{

	char *pchar;
	int LOOPS,	loops;

	for( loops=0; loops < 16; loops++)
		{
		Waardes[loops] = read_adc(loops);
		Waardes[loops] -= OffsetGIA[loops];
		}
	for( loops=0; loops < 16; loops++)
		{
		if (Waardes[ loops] < MinGIA[loops])
			ErrorGIA[loops] =1;
		else
			ErrorGIA[loops] =0;
		if( Waardes[loops] > -15  && Waardes[loops] < 0)
			Waardes[loops] = 0;
		}
	if( Waardes[2] >0)
		{
		FlowVleiSilvGF +=Waardes[2];
		VleiSilvGI++;
		}

	if( Waardes[3] >0)
		{
		FlowVleiResGF +=Waardes[3];
		VleiResGI++;
		}
	if( Waardes[4] >0)
		{
		FlowResSilvGF +=Waardes[4];
		ResSilvGI++;
		}
	if( Waardes[5] >0)
		{
		FlowGF +=Waardes[5];
		FlowTGI++;
		}
	Waardes[loops++] = inportb( BASE+6); /** read dig i/p 0 - 7 **/
	Waardes[loops++] = inportb( BASE+7); /** read dig i/p 8 - 15 **/
	Waardes[loops++] = inportb( BASE+6); /** read dig i/p 0 - 7 **/
	Waardes[loops]   = inportb( BASE+7); /** read dig i/p 8 - 15 **/
	if( ((Waardes[loops] & 0x80) >> 8) > 0)
		O_P1 &= 0x7f;
	else
		O_P1 |=80;
 }
*/
int PumpsOK( int pump)
	{
	int ConditionI;

	switch ( pump)
		{
		case 1:
			ConditionI = Waardes[16] &0xf;
		break;
		case 2:
			ConditionI = Waardes[16] >>4;
		break;
		case 3:
			ConditionI = Waardes[17] &0xf;
		break;
		case 4:
			ConditionI = Waardes[17] >>4;
		break;
		case 5:
			ConditionI = Waardes[18] &0xf;
		break;
		}

/*		if ( ConditionI &
		ConditionI
*/
	}

int Percentage( int OffsetI)
	{
	float Temp1F, Temp2F;
	int ReturnI;

	Temp1F = Waardes[OffsetI];
	Temp2F = MaksGIA[OffsetI];
	Temp1F /= Temp2F;
	Temp1F *=100;
	ReturnI = (int) Temp1F;
	return( ReturnI);
	}

void Controle()
	{
	int Vlei1I, Vlei2I, ResortI, HighLift1I, HighLift2I, PercentageI;

	HighLift1I = PumpsOK(1);
	HighLift2I = PumpsOK(2);
	ResortI    = PumpsOK(3);
	Vlei1I     = PumpsOK(4);
	Vlei2I     = PumpsOK(5);

	/***  treat the Resort Resevoir low level ***/
	PercentageI = Percentage(1);
	if ( PercentageI > AlarmLevelsGIA[1*2] )  /** check level of Resort Resevoir  **/
		{
		OutputByte1GUC &= ~(0x4 + 0x16);
		} else if ( PercentageI < AlarmLevelsGIA[(1*2)+1] )
			{    /** check level of the vlei**/
			 if ( (OutputByte1GUC & 0x8) ==0  && Vlei2I ==12)
				 OutputByte1GUC |= 0x16;
			 else if ( ResortI ==12)
				 OutputByte1GUC |= 0x4;
			}
	}



/*	Begin main function						*/
/*									*/


int main(argc, argv)
int argc;
char *argv[];
	{

	void *cursor, *scr1;
	struct time new_time;
	struct viewporttype t_type;
	struct linesettingstype l_type;
	time_t mytime;
	FILE *fp_temp;

	float TempF,TempXF, CalcF;

	char kar, *char_p, temp_buf[1048], *char_b, *pchar;
	char connect[] = { "c XXXXXX\r"};
	char karakter[16], KbdCharC;
	int modifier, Buffers, xpos,ypos, txpos=0, typos, tt, counter;
	int counts, old_pos, write_bytes, kbd_counter, char_send, TogleI=1;
	int loopsI;
	float PumpChangeHighI=20, PumpChangeLowI=0;
	float PumpChangeHighOld=0, PumpChangeLowOld=0;
	unsigned int size;
	long TotalLoops=0;
	int Loops=0, size_of_write_buf, ttxpos, ttypos, err_tx;
	int OldValueI=-1, TxposI;

	outportb(BASE+13, 0);
	fp_temp = fopen( "bore.gan", "rt");
	if( fp_temp == 0)
		{
		error(2, temp_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT/2; txpos++)
		{
		fscanf (  fp_temp, "%c ", &GainGUCA[txpos]);
		GainGUCA[txpos] -= '0';
		}
	fclose ( fp_temp);

	fp_temp = fopen( "bore.min", "rt");
	if( fp_temp == 0)
		{
		error(2, temp_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT/2; txpos++)
		{
		fscanf (  fp_temp, "%d ", &OffsetGIA[txpos]);
		}
	fclose ( fp_temp);

	fp_temp = fopen( "bore.maks", "rt");
	if( fp_temp == 0)
		{
		error(2, temp_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT; txpos++)
		{
		fscanf (  fp_temp, "%d ", &MaksGIA[txpos]);
/*		if( Waardes[txpos] >  MaksGIA[ txpos])
			 Waardes[txpos] =  MaksGIA[ txpos];*/
		}
	fclose ( fp_temp);

	fp_temp = fopen( "bore.LEV", "rt");
	if( fp_temp == 0)
		{
		error(2, temp_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT; txpos++)
		fscanf (  fp_temp, "%d ", &AlarmSetPointsGIA[txpos]);
	fclose ( fp_temp);

	fp_temp = fopen( "bore.unt", "rt");
	if( fp_temp == 0)
		{
		error(2, temp_buf);
		exit(0);
		}
	for ( txpos =0; txpos< MAKS_INT; txpos++)
		fscanf (  fp_temp, "%d ", &UnitsGIA[txpos]);
	fclose ( fp_temp);

	clrscr();
	memset( Waardes, -1, sizeof(Waardes));
	Initialize();
	size   = imagesize(1,1,100,120);
	menu   = malloc( size);
	size   = imagesize(1,1,25,25);
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
	strncpy ( &connect[2], argv[1], 6);
	strcpy ( STATION_NAME, connect);
/** int port,int speed,int bits,char parity,int stops) **/

	FlowMinF=LitersPerMinuteGF = atof( argv[3]);
	if( *argv[2] == '1')
		{
		if (initRS232(COM1,9600,8,'N',2))
				exit(1);
		} else
			{
			if (initRS232(COM2,9600,8,'N',2))
				exit(1);
			}

	time( &mytime);
	my_time  = localtime(&mytime);
	old_day  = my_time->tm_mday;

	txpos=xpos-10; typos=ypos-10;
	boretroom();
	getimage( txpos,typos,txpos+20,typos+20, cursor);
	getimage( MaxX-100    ,1       ,MaxX, 50, scr1);
	time( &mytime);
	my_time = localtime(&mytime);
	ctime( &mytime);
	old_minute = MIN = my_time->tm_min;
	break_comms();
	for(;;)
		{
		DAY = my_time->tm_mday;
		LOOPS=0;
		time( &mytime);
		my_time = localtime(&mytime);
		while (get_data( &rx_buffer[0], 9, &Waardes[0]) !=0);
		char_b = strchr( rx_buffer, ' ')+1;
		for ( Loops =0; Loops < 9;Loops++)
			{
			Waardes[Loops] = atoi( char_b);
			char_b = strchr( char_b, ' ')+1;
			}
		char_b = strchr( char_b, ' ')+1;
		FlowMinF = atof(char_b);
		if( FlowMinF >11)
			FlowMinF=10.9;
		char_b = strchr( char_b, ' ')+1;
		FlowTGF= atof(char_b);
		clearviewport();
		boretroom();
		display_data(0);
		Loops=0;
		DAY = my_time->tm_mday;
		MIN = my_time->tm_min;

		TotalLoops++;
		for ( FlowCounterGF=0; FlowCounterGF<119; FlowCounterGF++)
			{
			FlowsGFA[FlowCounterGF]    = FlowsGFA[FlowCounterGF+1];
			LevelsGFA[FlowCounterGF]   = LevelsGFA[FlowCounterGF+1];
			PressureGFA[FlowCounterGF] = PressureGFA[FlowCounterGF+1];
			}
		FlowsGFA[FlowCounterGF] = FlowMinF;

		CalcF = YtravelGF/UnitsGIA[0];
		TempF = UnitsGIA[0];
		TempF /= 1638;
		TempF *= Waardes[0];
		PressureGFA[FlowCounterGF] = TempF;

		CalcF = YtravelGF/UnitsGIA[1];
		TempF = UnitsGIA[1];
		TempF /= 1638;
		TempF *= Waardes[1];
		LevelsGFA[FlowCounterGF] = TempF;
		ypos=MaxY-20;
		xpos  =95;
		TempXF=95;
		for( tt =0; tt < 119; tt++)
			{
			TempF = (float)PressureGFA[tt-1];
			TempF *= YtravelGF/UnitsGIA[0];
			TxposI = (int) TempF;
			TempF = (float)PressureGFA[tt];
			TempF *= YtravelGF/UnitsGIA[0];
			setcolor( GREEN);
			if ( tt==0)
				line ( xpos, ypos-(int)TempF, (xpos + (int)XtravelGF), ypos-(int)TempF);
			else
				line ( xpos, ypos-TxposI, (xpos + (int)XtravelGF), ypos-(int)TempF);
			setcolor(YELLOW);
			TempF = (float)LevelsGFA[tt-1];
			TempF *= YtravelGF/UnitsGIA[1];
			TxposI = (int) TempF;
			TempF = (float)LevelsGFA[tt];
			TempF *= YtravelGF/UnitsGIA[1];
			if ( tt==0)
				line ( xpos, ypos-(int)TempF, (xpos + (int)XtravelGF), ypos-(int)TempF);
			else
				line ( xpos, ypos-TxposI, (xpos + (int)XtravelGF), ypos-(int)TempF);

			TempF = (float)FlowsGFA[tt-1];
			TempF *= YtravelGF/UnitsGIA[2];
			TxposI = (int) TempF;
			TempF = (float)FlowsGFA[tt];
			TempF *= YtravelGF/UnitsGIA[2];
			setcolor(MAGENTA);
			if ( tt==0)
				line ( xpos, ypos-(int)TempF, (xpos + (int)XtravelGF), ypos-(int)TempF);
			else
				line ( xpos, ypos-TxposI, (xpos + (int)XtravelGF), ypos-(int)TempF);
			TempXF += XtravelGF;
			xpos = (int) TempXF;
			}
		memset( rx_buffer, 0, sizeof(rx_buffer));
		file_open(0);
		sprintf ( rx_buffer, "%02d:%02d ", my_time->tm_hour, my_time->tm_min);
		for ( counter =0; counter < 9; counter++)
			{
			memset( temp_buf, 0, sizeof( temp_buf));
			sprintf ( temp_buf, " %d", Waardes[counter]);
			strcat ( rx_buffer, temp_buf);
			}
		memset( temp_buf, 0, sizeof( temp_buf));
		sprintf ( temp_buf, "%.1f ", FlowMinF);
		strcat ( rx_buffer, temp_buf);
		OuttextXY(438,  88, temp_buf, strlen(temp_buf)-1, YELLOW);
		sprintf ( temp_buf, "%.1f\n", FlowTGF/1000);
		strcat ( rx_buffer, temp_buf);
		rectangle( 415, 165, 528, 191);
		OuttextXY(418, 168, "Total Flow Kl", 13, YELLOW);
		OuttextXY(418, 178, temp_buf, strlen(temp_buf)-1, YELLOW);
		old_minute = strlen( rx_buffer);
		old_minute = fwrite( rx_buffer, sizeof( char), old_minute, fp_output);
		memset( rx_buffer, 0, sizeof( rx_buffer));
		fflush( fp_output);
		fclose ( fp_output);
		time( &mytime);
		my_time = localtime(&mytime);
		MIN = my_time->tm_min;
		old_minute = MIN;
		loopsI=120;
		do
			{
			time( &mytime);
			my_time = localtime(&mytime);
			MIN = my_time->tm_min;
			char_p  = ctime( &mytime);
			char_b  = strchr( char_p, ' ')+1;
			char_b  = strchr( char_b, ' ')+1;
			char_b  = strchr( char_b, ' ') ;
			*char_b =0;
			Outtextxy( 80 , 90, char_p, strlen( char_p));
			char_p = char_b+1;
			char_b = strchr( char_p, ' ');
			*char_b =0;
			Outtextxy( 80 , 100, char_p, strlen( char_p));
			if ( kbhit())
				{
				KbdCharC = getch();
				if ( KbdCharC == 0x1b )
					{
					closegraph();
					system("cls");
					exit(1);
					}
				}
			} while ( (old_minute == MIN) || (loopsI-- >0));
		old_minute = MIN;
		}
 }