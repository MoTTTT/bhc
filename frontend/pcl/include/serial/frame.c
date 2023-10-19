#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>

#include "c:\include\c_incl\kbd_char.h"

extern struct text_info old_info;

void frame(int left, int top, int right, int bottom ,char *string ,int kleur)
	{
	char lyne[81];
	int wid = right ;
	int i , strlength, Top=1;

	window( left, top, left+right, (top+bottom));
	textbackground(LIGHTGRAY);
	clrscr();
	window( left, top, left+right, (top+bottom)+1);
	textcolor(BLACK);
	lyne[wid+1] = 0;
	gotoxy(1, Top);
	lyne[0] = 218;
	for (i = 1; i < wid; i++)
		lyne[i] = 196;
	lyne[wid] = 191;
	cputs(lyne);
	gotoxy(1, Top);
	strlength = strlen(string);
	strlength = (wid - strlength)/2;
	strlength++;
	for(i=1; i <= strlength ; i++)
		gotoxy(i,Top);
	textcolor(BLACK);
	textbackground(kleur);
	cprintf("%s",string);
	textcolor( BLACK);
	textbackground(LIGHTGRAY);
	i += strlength;

	while(i <= wid+1)
		gotoxy( i++,Top);
	gotoxy(1, Top++);
	lyne[0] = 179;
	for (i = 1; i < wid; i++)
		lyne[i] = ' ';
	lyne[i] = 179;
	for (i = 0; i < bottom-1; i++)
		{
		gotoxy(1, Top++);
		cputs(lyne);
	  }
	lyne[0] = 192;
	for (i = 1; i < wid; i++)
		lyne[i] = 196;
	lyne[wid] = 217;
	cputs(lyne);
	window( left+1, top+1, left+right-1, top+bottom-1);
  }

void frame_only(int left, int top, int right, int bottom, int agter_kleur, int voor_kleur)
	{
	char lyne[81];
	int wid = right ;
	int i , strlength, Top=1;

	window( left, top, left+right, bottom);
	textbackground(agter_kleur);
	clrscr();
	window( left, top, left+right, bottom+1);
	textcolor(voor_kleur);
	lyne[wid+1] = 0;
	gotoxy(1, Top);
	lyne[0] = 218;
	for (i = 1; i < wid; i++)
		lyne[i] = 196;
	lyne[wid] = 191;
	cputs(lyne);
	while(i <= wid+1)
		gotoxy( i++,Top);
	gotoxy(1, Top++);
	lyne[0] = 179;
	for (i = 1; i < wid; i++)
		lyne[i] = ' ';
	lyne[i] = 179;
	for (i = 0; i < bottom-3; i++)
	  {
		gotoxy(1, Top++);
		cputs(lyne);
	  }
	lyne[0] = 192;
	for (i = 1; i < wid; i++)
		lyne[i] = 196;
	lyne[wid] = 217;
	cputs(lyne);
	window( left+1, top+1, left+right-1, top+bottom-1);
  }

/*
main ()
  {

	int top, left, right,bottom;

	for(;;)
	  {
		left = random ( 39)+1;
		top = random (14)+1;
		right = left+40;
		bottom = top + 10;
		textbackground( BLACK);
		window(1,1,80,25);
		clrscr();
		window( left, top, right, bottom);
		printf ("left - %d top - %d right - %d bottom - %d", left, top, right, bottom);
		frame(left, top, right, bottom , "                TEST FRAME             " ,WHITE );
		gotoxy(2,4);
		textcolor( WHITE);
		textbackground(LIGHTRED);

		cprintf( "Test 4                                 ");
		textbackground(BLUE);
		gotoxy(2,5);
		cprintf( "Test 5                                 ");
		gotoxy(2,6);
		textbackground(LIGHTCYAN);
		cprintf( "Test 6                                 ");
		gotoxy(2,7);
		textbackground(LIGHTMAGENTA);
		cprintf( "Test 7                                 ");
		gotoxy(2,8);
		textbackground(RED);
		cprintf( "Test 8                                 ");
		sleep(1);
	  } ** end of ;; loop**
  } ** end of main **/

display_menu( left, top)
int left, top;
	{
	int counter=0;
	char *menu[] =
		{
		"<F1 > - Build font            \0",
		"<F2 > - Delete font           \0",
		"<F3 > - Edit text             \0",
		"<F4 > - View text             \0",
		"<F5 > - Build script          \0",
		"<F6 > - Display script        \0",
		"<F7 > - Delete Dsiplay script \0"
/*		"<F8 > -                       \0",
		"<F9 > -                       \0",
		"<F10> -                       \0",
		"<F11> -                       \0",
		"<F12> -                        \0",
*/
		};

	while ( counter < 7)
		{
		gotoxy( left,top++);
		clreol();
		cputs(menu[counter++]);
		}
  }

display_row( strings, lines, pos)
char *strings, pos;
int lines;
	{
	char *char_p, *char_p1;
	int lyne;
	struct text_info old_info;

	gettextinfo( &old_info);

	textbackground( LIGHTGRAY);
	if ( pos != 't')
		window( 1,25,80,25);
	else
		window( 1,1,80,1);
	textbackground(LIGHTGRAY);
	clrscr();
	char_p = strings;
	for ( lyne=0; lyne< lines;lyne++)
		{
		textcolor(RED);
		char_p1 = strchr( char_p, ' ');
		*char_p1= 0;
		cprintf( " %s", char_p);
		*char_p1= ' ';
		char_p1++;
		textcolor(WHITE);
		cprintf( "%s", char_p1);
		char_p = strchr( char_p1, 0);
		char_p++;
		}
	window ( old_info.winleft, old_info.wintop,\
			old_info.winright,old_info.winbottom);
	}
check_alt_etc( counts)
int counts;
	{
	int modifier, alt_count = 0, shift_count =0, ctrl_count =0;

	do
		{
		modifier = bioskey(2); /** look for a pressed key**/

		if ( modifier & ALT )
					alt_count++;
		else if ( modifier & SHIFT)
				 shift_count++;
		else if ( modifier & CTRL)
				 ctrl_count++;
		else
			{
			alt_count=0;
			shift_count=0;
			ctrl_count=0;
			}
		if( alt_count > counts)
			if ( alt_count == counts+1)
				{
				display_alt();
				while ( bioskey(2) & ALT); /** look for a pressed key**/
				display_norm_top_row();
				shift_count=0;
				ctrl_count=0;
				}

		if( shift_count > counts)
			if ( shift_count == counts+1)
				{
				display_shift();
				while ( bioskey(2) & SHIFT); /** look for a pressed key**/
				display_norm_top_row();
				alt_count=0;
				ctrl_count=0;
				}

		if( ctrl_count > counts)
			if ( ctrl_count == counts+1)
				{
				display_ctrl();
				while ( bioskey(2) & CTRL); /** look for a pressed key**/
				display_norm_top_row();
				alt_count=0;
				shift_count=0;
				}
		modifier = bioskey(1);
		}while ( modifier != 283 && modifier == 0);
	}