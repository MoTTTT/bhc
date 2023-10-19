*****************************************************************************
**                     PROC.prg : GENERAL SHORT PROCEDURES                 **
**  PART OF MGRAPH.PRG written by G.ENGLAND of M3 ENTERPRISE,  on 26/6/91  **
**                  ( VECTRON DATA GRAPHING DEMONSTRATION )                **
*****************************************************************************

* PROCEDURE TO DISPLAY A PLEASE WAIT MSG ...

PROCEDURE WAITMSG
PARAMETER LINENUMBER

SET COLOR TO GR+/B         && YELLOW ON BLUE
@ LINENUMBER - 1, 30 TO LINENUMBER + 1, 50
@ LINENUMBER, 31 SAY "   PLEASE WAIT !   "
SET COLOR TO &NORMSCRN
RETURN

* -----------------------------------------------------------------------

* PROCEDURE TO PAUSE UNTIL ESC PRESSED 

PROCEDURE PAUSE
DO WHILE INKEY( 0 ) <> ESC
ENDDO

* STOP ANY FUTURE 'LASTKEY=ESC' ABORT CHECKS BEING INCORRECTLY MADE!
KEYBOARD CHR( 32 )
INKEY( 1 )
RETURN

* -----------------------------------------------------------------------

** PROCEDURE TO STUFF A CONTROL-W TO THE KEYBOARD BUFFER

PROCEDURE CTRL_W

KEYBOARD CHR( 23 )
RETURN

* ---------------------------------------------------------------------

** PROCEDURE TO PRINT A WARNING MESSAGE CENTRALISED ON THE SCREEN

PROCEDURE WARNING
PARAMETER WORDING, ATLINE

SET COLOR TO &STANDOUT
IF ( LEN( WORDING ) / 2 ) * 2 = LEN( WORDING )
  WORDING = WORDING + " " 
ENDIF

WORDING = ">>> " + WORDING + "! <<<" 
L = LEN( WORDING ) / 2 

IF L < 13 
  L = 13 
ENDIF

SAVE SCREEN TO TEMPSCR

@ ATLINE - 1,38 - L CLEAR TO ATLINE + 2,41 + L
@ ATLINE - 1,38 - L TO ATLINE + 2,41 + L
@ ATLINE,40 - ( LEN( WORDING ) / 2 ) SAY WORDING
@ ATLINE+1,27 SAY "  Press <Esc> to go back  "

SET COLOR TO &NORMSCRN
SET CURSOR OFF
DO PAUSE
SET CURSOR ON
RESTORE SCREEN FROM TEMPSCR

RETURN

* -----------------------------------------------------------------

* PROCEDURE TO PRINT INITIAL HELP SCREEN

PROCEDURE HELP
SAVE SCREEN TO TEMPSCR
@ 3,8 CLEAR TO 23,70
@ 3,8 TO 23,70
@  5,9 SAY "   VECTRON Graphical Presentation Routine  -  Version "+VER+"   "
@  6,9 SAY "             HOW TO USE THE MENUS IN THIS SYSTEM             "

@  8,9 SAY "   When presented with the menus for this system, you can    "
@  9,9 SAY " select an option by one of two ways.  The simple way is to  "
@ 10,9 SAY " press the first letter of your required choice.  To find    "
@ 11,9 SAY " out more about an option first, use the cursor control keys "
@ 12,9 SAY " (up/down/home/end) to highlight a choice, and you will see  "
@ 13,9 SAY " a short explanation on the bottom line of the screen;       "
@ 14,9 SAY " to select that option press the RETURN (or ENTER) key.      "

@ 16,9 SAY "    In most cases throughout this system, pressing the <Esc> "
@ 17,9 SAY " key will abort what you are doing and return you to the     "
@ 18,9 SAY " previous screen.  Note that if you press <Esc> when you are "
@ 19,9 SAY " entering a screen full of data IT WILL NOT BE SAVED !       "

@ 21,9 SAY "          P R E S S   < E s c >   T O   R E T U R N          "
DO PAUSE
RESTORE SCREEN FROM TEMPSCR

RETURN

* -----------------------------------------------------------------

** PROCEDURE TO CENTRALISE A CHR STRING IN A BOX AT A GIVEN LINE

PROCEDURE BOXTEXT
PARAMETER WORDING, LINENUMBER

WORDING = TRIM( WORDING )
L = LEN( WORDING )
LHS = 40 - ( L / 2 ) 
@ LINENUMBER - 1,LHS - 1 TO LINENUMBER + 1,LHS + LEN( WORDING )
@ LINENUMBER,LHS SAY WORDING
 
RETURN

* -----------------------------------------------------------------

** PROCEDURE TO GIVE HELP (IN GRAPHICS WINDOW) FOR THE MAIN GRAPH
** OF THE VECTRON GRAPHICAL PRESENTATION 

PROCEDURE MGHELP
PARAMETERS BLX, BLY   && BOTTOM LEFT X AND Y CO-ORDINATES

* BLX = xorigin + 30
* BLY = yorigin + GraphHeight - 300
boxfill( BLX, BLY, 640, 290, 0, 4 )
boxfill( BLX + 2, BLY + 2, 636, 286, 0, 8 )
saystring( BLX + 290, BLY + 250, 8, 0, 14, 'KEY' )

DRAWLINE( BLX + 22, BLY + 225, BLX + 100, BLY + 225, 0, 3, 12 )
DRAWLINE( BLX + 22, BLY + 200, BLX + 100, BLY + 200, 0, 0, 12 )
DRAWLINE( BLX + 22, BLY + 175, BLX + 100, BLY + 175, 0, 1, 12 )
DRAWLINE( BLX + 22, BLY + 150, BLX + 100, BLY + 150, 0, 3, 10 )
DRAWLINE( BLX + 22, BLY + 125, BLX + 100, BLY + 125, 0, 0, 10 )
DRAWLINE( BLX + 22, BLY + 100, BLX + 100, BLY + 100, 0, 1, 10 )
DRAWLINE( BLX + 22, BLY +  75, BLX + 100, BLY +  75, 0, 3, 14 )
DRAWLINE( BLX + 22, BLY +  50, BLX + 100, BLY +  50, 0, 0, 14 )
DRAWLINE( BLX + 22, BLY +  25, BLX + 100, BLY +  25, 0, 1, 14 )

saystring( BLX + 120, BLY + 217, 9, 0, 15, 'Lowest Freezer Temperature' )
saystring( BLX + 120, BLY + 192, 9, 0, 15, 'Average Freezer Temperature' )
saystring( BLX + 120, BLY + 167, 9, 0, 15, 'Highest Freezer Temperature' )
saystring( BLX + 120, BLY + 142, 9, 0, 15, 'Lowest Chilled Temperature' )
saystring( BLX + 120, BLY + 117, 9, 0, 15, 'Average Chilled Temperature' )
saystring( BLX + 120, BLY +  92, 9, 0, 15, 'Highest Chilled Temperature' )
saystring( BLX + 120, BLY +  67, 9, 0, 15, 'Lowest Cold Temperature' )
saystring( BLX + 120, BLY +  42, 9, 0, 15, 'Average Cold Temperature' )
saystring( BLX + 120, BLY +  17, 9, 0, 15, 'Lowest Cold Temperature' )

clrline( 0, 0, 18 )
saystring( 0, 0, 4, 0, 14, "Press any key to return" )
INKEY( 0 )

RETURN

* -----------------------------------------------------------------

** PROCEDURE TO DRAW THE POINTER CROSS-HAIRS IN THE MASTER 
** GRAPH OF THE VECTRON GRAPHICAL PRESENTATION 

PROCEDURE POINTER
PARAMETER REMOVE   && IF TRUE, POINTER IS JUST BEING REMOVED FROM SCREEN

IF REMOVE
* REPLACE POINTER WINDOW WITH GRAPH
  SNAPPASTE( PTRWIN_X, PTRWIN_Y, PTRWIN_G )  && CLEAR
  SNAPKILL( PTRWIN_G )
  DRAWLINE( XPOS, YORIGIN, XPOS, YORIGIN + GRAPHHEIGHT, 2, 1, 15 ) 
  DRAWLINE( XORIGIN, YPOS, XORIGIN + GRAPHWIDTH, YPOS, 2, 1, 15 )   

ELSE
* DRAW VERTICAL POINTER LINE 
  XPOS = XORIGIN + ( XPOINTER * ZOOMP[ ZOOM ] )
  DRAWLINE( XPOS, YORIGIN, XPOS, YORIGIN + GRAPHHEIGHT, 2, 1, 15 ) 

* DRAW HORIZONTAL POINTER LINE 
  PTR_POS = MIN( XSTART + XPOINTER, A_COUNT )

  DO CASE
    CASE PRIMARY = 1
      PVAL = ASPEED[ PTR_POS ]
      YPOINTER = PVAL * SYSCALE
    CASE PRIMARY = 2
      PVAL = ARPM[ PTR_POS ]
      YPOINTER = PVAL * RYSCALE
    CASE PRIMARY = 3
      PVAL = AFUEL[ PTR_POS ]
      YPOINTER = PVAL * FYSCALE
  ENDCASE
  YPOS = YORIGIN + YPOINTER
  DRAWLINE( XORIGIN, YPOS, XORIGIN + GRAPHWIDTH, YPOS, 2, 1, 15 )   

* DRAW WINDOW NEAR WHERE POINTER CROSSES, SHOWING PRIMARY VALUE AND TIME
  PTRWIN_X = IF( ( XPOS - XORIGIN ) < ( GRAPHWIDTH / 2 ), ;
             XPOS + 50, XPOS - ( 50 + PTRWIN_W ) )
  PTRWIN_Y = IF( YPOINTER < ( GRAPHHEIGHT / 2 ), ;
             YPOS + 50, YPOS - ( 50 + PTRWIN_H ) ) 
  PTRWIN_G = SNAPCOPY( PTRWIN_X, PTRWIN_Y, ;
                       PTRWIN_X + PTRWIN_W, PTRWIN_Y + PTRWIN_H, 1 )  && CLEAR

  DRAWLINE( PTRWIN_X + 10, PTRWIN_Y + ( PTRWIN_H / 2 ), ;
            PTRWIN_X + ( PTRWIN_W - 10 ), PTRWIN_Y + ( PTRWIN_H / 2 ), ;
            0, 0, 15 )

  SAYSTRING( PTRWIN_X + ( PTRWIN_W / 2 ), PTRWIN_Y + 10, 17, 8, 15, ;
             LTRIM( SHOWTIME( ATIME[ XSTART + XPOINTER ], .T. ) ) )

  TPVAL = ALLTRIM( STR( PVAL, 11, 2 ) )
  SAYSTRING( PTRWIN_X + ( PTRWIN_W / 2 ), PTRWIN_Y + 55, 17, 8, PCOL, ;
             IF( RIGHT( TPVAL, 2 ) = "00", LEFT( TPVAL, LEN( TPVAL ) - 3 ), TPVAL ) )
 
* DRAW DRIVER DETAILS OF GRAPH TITLE (DEPENDANT ON POINTER)
  IF CURR_DR_NO <> ADR_NO[ PTR_POS ]
    CURR_DR_NO = ADR_NO[ PTR_POS ]
    CLRLINE( 0, 930, 73 )
    IF CURR_DR_NO <> 0
      GTITLE2 = "Driver: " + ADR_ID[ CURR_DR_NO ] + "  " + ;
      ADR_NAME[ CURR_DR_NO ] 
      saystring( 1350 / 2, 930, 0, 8, 15, GTITLE2 )
    ENDIF
  ENDIF
ENDIF

RETURN

* -----------------------------------------------------------------

** PROCEDURE TO DRAW THE X AND Y GRIDS IN THE MASTER 
** GRAPH OF THE VECTRON GRAPHICAL PRESENTATION 

PROCEDURE DRAW_GRID

YGRIDDIVS = 10    && DIVIDE Y AXIS BY 10  ( X-DIVS IN ARRAY ZOOMXG )

XYAXES( XORIGIN, YORIGIN, GRAPHWIDTH, GRAPHHEIGHT, ;
        ZOOMXG[ ZOOM ], YGRIDDIVS, 32 + 8, 8 )

* X AXIS LABELLING ...
XGRIDLABEL = 4
TXT = ""
FOR LOOP = 1 TO ZOOMXG[ ZOOM ] STEP XGRIDLABEL   && NUMBER OF X-AXIS DIVISIONS
  TTXT = ADDSECS( ATIME[ XSTART ], ( ( LOOP - 1 ) * ZOOMXGM[ ZOOM ] ) * 60 )
  TTXT = SHOWTIME( TTXT, .F. )
  TXT = TXT + TTXT
NEXT LOOP

LABELX( XORIGIN, YORIGIN - 35, ( GRAPHWIDTH / ZOOMXG[ ZOOM ] ) * XGRIDLABEL, ;
        IF( D_TIME = 12, 7, 5 ), 1, 8, 15, TXT )

RETURN

* -----------------------------------------------------------------

** PROCEDURE TO DRAW THE DATA LINE(S) IN THE MASTER 
** GRAPH OF THE VECTRON GRAPHICAL PRESENTATION 

PROCEDURE PLOT

COLOURS = STR( SPEED_COL, 2 ) + ;
          STR( RPM_COL, 2 ) + ;
          STR( FUEL_COL, 2 )

X_NEW = "XORIGIN + ( ( GRAPHPOINT - XSTART ) * ZOOMP[ ZOOM ] )"

FOR LINE = 1 TO 3
  DO CASE
    CASE LINE = 1
      Y_NEW = "YORIGIN + ( ASPEED[ GRAPHPOINT ] * SYSCALE )"

    CASE LINE = 2
      Y_NEW = "YORIGIN + ( ARPM[ GRAPHPOINT ] * RYSCALE )"

    CASE LINE = 3
      Y_NEW = "YORIGIN + ( AFUEL[ GRAPHPOINT ] * FYSCALE )"
  ENDCASE

  IF DSHOW[ LINE ]
    GRAPHPOINT = XSTART + 1
    TCOL = VAL( SUBSTR( COLOURS, ( LINE * 2 ) - 1, 2 ) )
    DRAWLINE( X1, SY1, &X_NEW, &Y_NEW, 0, 0, TCOL )  && DRAW FIRST LINE

    FOR GRAPHPOINT = XSTART + 2 TO XFINISH
      DRAWLINE( 0, 0, &X_NEW, &Y_NEW, 16, 0, TCOL )  && CONTINUE FROM LAST LINE
    NEXT GRAPHPOINT
  ENDIF
NEXT LINE

* NOW DRAW DRIVING TIME LINES ALONG GRAPH BOTTOM ...
LAST_DR = ADR_NO[ XSTART ]
LAST_POS = XORIGIN
FIRST_POINT = .T.
FOR GRAPHPOINT = XSTART + 1 TO XFINISH
  IF ADR_NO[ GRAPHPOINT ] <> LAST_DR .OR. ( GRAPHPOINT = XFINISH )
    IF LAST_DR <> 0
      TCOL = IF( LAST_DR = 1, D1_COL, D2_COL )
      IF FIRST_POINT
        FIRST_POINT = .F.
      ELSE
        DRAWLINE( LAST_POS, YORIGIN, LAST_POS, YORIGIN + 30, 0, 0, TCOL )
      ENDIF
      BOXFILL( LAST_POS, YORIGIN - 5, &X_NEW - LAST_POS, 4, 0, TCOL )
    ENDIF

    LAST_DR = ADR_NO[ GRAPHPOINT ]
    LAST_POS = &X_NEW
    IF LAST_DR <> 0
      TCOL = IF( LAST_DR = 1, D1_COL, D2_COL )
      DRAWLINE( LAST_POS, YORIGIN, LAST_POS, YORIGIN + 30, 0, 0, TCOL )
    ENDIF
  ENDIF
NEXT GRAPHPOINT

RETURN

* -----------------------------------------------------------------

* PROCEDURE TO DISPLAY MIN AND MAX MARKS ON GRAPH, SET THE
* PRIMARY DATA COLOUR AND DISPLAY MAX Y AXIS VALUE

PROCEDURE MINMAX

DO CASE
  CASE PRIMARY = 1
    SAYSTRING( XORIGIN + 5, YORIGIN + GRAPHHEIGHT + 10, 1, 8, SPEED_COL, LTRIM( STR( MAX_SPEED, 6 ) ) )
    PCOL = SPEED_COL

    IF LMAX_SPEED > 0 .AND. AMAX_SPEED > LMAX_SPEED 
      TYPOS = YORIGIN + ( LMAX_SPEED * SYSCALE )
      BOXFILL( XORIGIN, TYPOS, ;
               GRAPHWIDTH, ( GRAPHHEIGHT + YORIGIN ) - TYPOS, 3 + 128, 4 )
    ENDIF

    IF LMIN_SPEED > 0 
      BOXFILL( XORIGIN, YORIGIN, ;
               GRAPHWIDTH, LMIN_SPEED * SYSCALE, 3 + 128, 4 )
    ENDIF

  CASE PRIMARY = 2
    SAYSTRING( XORIGIN + 5, YORIGIN + GRAPHHEIGHT + 10, 1, 8, RPM_COL, LTRIM( STR( MAX_RPM, 6 ) ) )
    PCOL = RPM_COL

    IF LMAX_RPM > 0 .AND. AMAX_RPM > LMAX_RPM 
      TYPOS = YORIGIN + ( LMAX_RPM * RYSCALE )
      BOXFILL( XORIGIN, TYPOS, ;
               GRAPHWIDTH, ( GRAPHHEIGHT + YORIGIN ) - TYPOS, 3 + 128, 4 )
    ENDIF

    IF LMIN_RPM > 0 
      BOXFILL( XORIGIN, YORIGIN, ;
               GRAPHWIDTH, LMIN_RPM * RYSCALE, 3 + 128, 4 )
    ENDIF

  CASE PRIMARY = 3
    SAYSTRING( XORIGIN + 5, YORIGIN + GRAPHHEIGHT + 10, 1, 8, FUEL_COL, LTRIM( STR( MAX_FUEL, 6 ) ) )
    PCOL = FUEL_COL

    IF LMAX_FUEL > 0 .AND. AMAX_FUEL > LMAX_FUEL 
      TYPOS = YORIGIN + ( LMAX_FUEL * FYSCALE )
      BOXFILL( XORIGIN, TYPOS, ;
               GRAPHWIDTH, ( GRAPHHEIGHT + YORIGIN ) - TYPOS, 3 + 128, 4 )
    ENDIF

    IF LMIN_FUEL > 0 
      BOXFILL( XORIGIN, YORIGIN, ;
               GRAPHWIDTH, LMIN_FUEL * FYSCALE, 3 + 128, 4 )
    ENDIF
ENDCASE
