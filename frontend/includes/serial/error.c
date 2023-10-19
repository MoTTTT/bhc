#include <dos.h>
#include <conio.h>
#include <string.h>

#include "C:\INCLUDE\C_INCL\ERRORSTR.H"

error(num, string)
int num;
char *string;
{
 char *char_p, *char_p1;
 int tone, pos=1;

 frame_only(  1    ,2    ,79    ,24   , RED, LIGHTGRAY);
 if ( strlen ( string) > 0)
 {
	 char_p = string;
	 char_p1 = strchr( string, '\n');
	 *char_p1++=0;
	 if (strlen(string) > strlen(char_p1))
		 tone = strlen(string)+2;
	 else
		 tone = strlen(char_p1);
 } else
		 tone = strlen( errorstr[num])+2;
 frame(  10    ,8    ,tone    ,3   , " ERROR_DISPLAY ", WHITE);
 cprintf("%s", errorstr[num]);
 if ( strlen ( string) > 0)
 {
	 gotoxy( 1,++pos);
	 cprintf("%s", char_p);
 }
 for ( tone = 800; tone > 400; tone -=100)
 {
	sound (tone);
	delay( 90);
 }
	nosound();
	sleep(5);
}