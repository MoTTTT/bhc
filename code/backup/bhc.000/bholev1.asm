                      ;WCRSC BOREHOLE CONTROL
                      ;**********************
        ;BYTE ALLOWCATION
        ;****************
        ;ADC0----71,70   LEVEL
        ;ADC1----73,72   VALVE POSITION
        ;ADC2----75,74   PRESSURE
        ;ADC3----77,76
        ;ADC4----79,78
        ;ADC5----7B,7A
        ;ADC6----7D,7C
        ;ADC7----7F,7E
        ;TOTAL FLOW----63,62,61,60
        ;FLOW/SEC----64
        ;TIME CLOCK----67,66,65
        ;MINUTE COUNTER----69,68
        ;RAM ADRESSING----6B,6A
        ;FLOW RATE----6C   DEFAULT
        ;VALVE CLOSE POS.----6D     15 DIG. COUNTS
        ;VALVE OPEN POS.----6E         DIG.FOR APPROX. FLOW RATE
        ;LOW LEVEL VALUE------6F       52 BCD DEFAULT

        INPUT EQU 26H   ;INPUT BYTE
        OUTPUT EQU 24H  ;OUTPUT BYTE
        CONTR1 EQU 28H  ;1st CONTROL BYTE
        CONTR2 EQU 29H  ;2nd CONTROL BYTE                                  
        
        ;ADDRESS IN ASCII
        ;****************
        DIG1 EQU 33H
        DIG2 EQU 34H
        DIG3 EQU 30H
        DIG4 EQU 31H
        DIG5 EQU 39H

        ;DIGITAL INPUTS
        ;**************
        PHFAIL BIT 30H
        AUTO BIT 31H
        MAN BIT 32H
        S_START BIT 33H
        FAULT BIT 34H
        RUN BIT 35H

        ;DIGITAL OUTPUTS
        ;***************
        L_LEV BIT 20H
        P_START BIT 21H
        V_CLOSE BIT 22H
        V_OPEN BIT 23H
        DOG BIT 27H

        ;CONTR_1 BITS
        ;************
        WEDOG BIT 40H     ;EXTERNAL W/DOG SUBR. ENABLE
        CLOCK BIT 41H           
        ANALOG BIT 42H             
        DELAY1 BIT 43H    ;VALVE PULSE TIME START REG 4                                   
        DELAY2 BIT 44H    ;L/LEVEL LOCKOUT DELAY START REG 3
        FLOW BIT 45H      ;FLOW CONTROL ENABLE       
        AUT_ON BIT 46H    ;SET OR CLEARED BY COMMS
        COMMS BIT 47H     ;SERIAL COMMS HAS BEEN MADE

        ;CONTR_2 BITS
        ;************
        WIDOG BIT 48H     ;INTERNAL W/DOG ENABLE
        DELAY1A BIT 49H   ;VALVE PULSE TIME OUT
        DELAY2A BIT 4AH   ;L/LEVEL TIME OUT
        V_POS BIT 4BH     ;APPROX VALVE POSITION REACHED
        COMM1 BIT 4CH     ;ENABLE DATA FOR KEY BOARD DISPLAY
        

        ;I/O ADDRESSING CONTROL WORD 82H
        ;*******************************
        PCONTROL EQU 000BH
        PORT_A EQU 0008H   ;OUTPUT
        PORT_B EQU 0009H   ;INPUT
        PORT_C EQU 000AH   ;OUTPUT

$INCLUDE(REG552.PDF)

        LJMP SETUP

ORG 23H
        NOP
        LJMP RECEIVE
        
ORG 5BH
        NOP         ;TIMER2 INTERRUPT ROUTINE EVERY 50mS
        CLR ES      ;DISABLE SERIAL INTERRUPT
        PUSH ACC
        PUSH PSW
        SETB P1.5        ;RESET T2
        SETB WEDOG       ;ENABLE EXT W/DOG SUBROUTINE
        CLR P1.5
        NOP
        INC R6
        CJNE R6,#20,NEXT  ;ENABLE A TO D EVERY 1 SEC    
        MOV R6,#00H       ;CLEAR REG 6
        SETB ANALOG
NEXT:   NOP
        INC R7
        CJNE R7,#200,NEXT1 ;ENABLE CLOCK SUBROUTINE EVERY 10 SEC's
        MOV R7,#00H        ;CLEAR REG 7
        SETB CLOCK 
NEXT1:  NOP
        INC R5
        CJNE R5,#100,NEXT2 ;ENABLE FLOW CONTROL EVERY 5SEC's
        CLR TR0            ;STOP COUNTER
        MOV B,#05H
        MOV A,TL0          ;COUNTER VALUE TO ACC
        DIV AB            ;DIV COUNTER VALUE BY 5
        MOV B,#0AH         ;BIN TO BCD CONVERSION
        DIV AB
        SWAP A
        ADD A,B
        MOV R1,#64H
        MOV @R1,A          ;STORE FLOW/SEC IN BYTE 64
        NOP                ;UPDATE TOTAL FLOW
        MOV R2,#05H
COUNT:  NOP
        MOV R1,#64H
        MOV A,@R1          ;FLOW/SEC TO ACC
        MOV R1,#60H
        CLR C
        ADD A,@R1
        DA A
        MOV @R1,A          ;STORE IN BYTE 60
        INC R1
        CLR A
        ADDC A,@R1
        DA A
        MOV @R1,A          ;STORE IN BYTE 61
        INC R1
        CLR A
        ADDC A,@R1
        DA A
        MOV @R1,A          ;STORE IN BYTE 62
        INC R1
        CLR A
        ADDC A,@R1
        DA A
        MOV @R1,A          ;STORE IN BYTE 63
        DJNZ R2,COUNT
        NOP
        MOV TL0,#00H       ;CLEAR COUNTER
        SETB TR0           ;START COUNTER
        MOV R5,#00H        ;CLEAR REG 5
        SETB FLOW  
NEXT2:  NOP
        JNB DELAY1,NEXT3   ;VALVE PULSE START
        INC R4
        CJNE R4,#04,NEXT3  ;200mS
        MOV R4,#00H        ;CLEAR REG 4
        SETB DELAY1A       ;VALVE PULSE TIME OUT
NEXT3:  NOP
        JNB DELAY2,NEXT4   ;LOW LEVEL LOCKOUT DELAY START
        INC R3
        CJNE R3,#60,NEXT4  ;3 SEC's
        MOV R3,#00H        ;CLEAR REG 3
        SETB DELAY2A       ;LOW LEVEL LOCKOUT TIME OUT
NEXT4:  NOP
        CLR CMI0
        POP PSW
        POP ACC
        SETB ES           ;ENABLE SERIAL INTERRUPT
        RETI

SETUP:  NOP          
        MOV P1,#0FFH     ;SET PORT1 ON MICRO ALL HIGH
        MOV DPTR,#PCONTROL   ;ALL OUTPUTS OFF
        MOV A,#82H           ;***************
        CLR P1.1
        MOVX @DPTR,A
        MOV DPTR,#PORT_A
        MOV A,#00H
        MOVX @DPTR,A
        MOV DPTR,#PORT_C
        MOV A,#00H
        MOVX @DPTR,A
        NOP
        SETB P1.1
        NOP              ;CLEAR BYTES 50 TO 7F
        MOV R0,#50H      ;********************
        MOV R1,#48
LOOP:   NOP
        MOV @R0,#00H
        INC R0
        DJNZ R1,LOOP
        NOP
        MOV SP,#0FH       ;SET STACK POINTER TO START AT 3rd REG. BANK
        MOV R0,#OUTPUT    ;CLEAR OUTPUT,INPUT &CONTROL BYTES
        MOV @R0,#00H      ;*********************************
        MOV R0,#INPUT
        MOV @R0,#00H
        MOV R0,#CONTR1
        MOV @R0,#00H
        MOV R0,#CONTR2
        MOV @R0,#00H
        NOP
        ;**************DEFAULT SETTINGS*******************
        ;*************************************************
        MOV R0,#6CH       ;SET DEFAULT FLOW RATE
        MOV @R0,#12H      ;22 L/SEC **********************
        MOV R0,#6DH
        MOV @R0,#15       ;VALVE CLOSE POSITION *******************
        MOV R0,#6EH
        MOV @R0,#65       ;VALVE OPEN POS. FOR APPROX. FLOW RATE ***********
        NOP               ;************************************
        MOV R0,#6FH
        MOV @R0,#52H      ;DEFAULT LOW LEVEL****************************
        MOV R3,#00H       ;CLEAR REG FOR COUNTERS
        MOV R4,#00H
        MOV R5,#00H
        MOV R6,#00H
        MOV R7,#00H
        MOV CMH0,#0B4H    ;SET FOR 50mSEC INTERRUPT
        MOV CML0,#00H
        MOV TMOD,#25H     ;SET T0 AS 16 BIT COUNTER,T1 AS AUTO RELOAD
        MOV TH1,#250      ;4800 BAUD
        MOV TL1,#250
        MOV SCON,#50H     ;SET COMMS MODE 1
        SETB EA           ;ENABLE ALL INTERRUPTS
        SETB ECM0         ;ENABLE T2 COMPARE INTERRUPT 
        SETB ES          ;ENABLE SERIAL INTERRUPT
        SETB TR0         ;START COUNTER T0
        SETB TR1         ;START T1
        CLR P1.5
        MOV TM2CON,#21H   ;START T2   ENABLE EXT CLEAR
BEGIN:  NOP               ;READ INPUTS
        MOV C,AUT_ON
        MOV P_START,C
        MOV DPTR,#PORT_B
        MOV R0,#INPUT
        CLR P1.1          ;ENABLE I/O
        NOP
        MOVX A,@DPTR
        MOV @R0,A
        SETB P1.1         ;DISABLE I/O
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1    ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1:   NOP
        JNB ANALOG,LAB2   ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2:   NOP
        JNB CLOCK,LAB3    ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3:   NOP
        JNB WIDOG,LAB4    ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4:   NOP
        JNB COMMS,LAB5    ;XMIT SUBROUTINE
        LCALL SUB5
LAB5:   NOP
        JNB COMM1,LAB6    ;XMIT SUBROUTINE TO KEYBOARD
        LCALL SUB6
LAB6:   NOP               ;CHECK IF RUN SIGNAL=1
        JNB RUN,LAB7
        LJMP CONTRO1
LAB7:   NOP               ;CHECK FOR HIGH PRESSURE IE.>8bar  217 DIGITAL
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#02H  ;CHANNEL 2
        ORL ADCON,#08H  ;START ADC
LAB8:   NOP
        MOV A,ADCON
        JNB 0E4H,LAB8 
        CLR C
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,#217
        JC CLOSE        ;GOTO CLOSE VALVE
        LJMP CONTRO1
CLOSE:  NOP
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#01H  ;CHANNEL 1
        ORL ADCON,#08H  ;START ADC
LAB9:   NOP
        MOV A,ADCON
        JNB 0E4H,LAB9 
        CLR C
        MOV R0,#6DH
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,@R0      ;SUBTRACT DEFINED CLOSE POSITION
        JNC LAB10
        CLR V_CLOSE     ;VALVE CLOSE OFF
        CLR V_OPEN
        MOV DPTR,#PORT_A
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP BEGIN
LAB10:  NOP
        SETB V_CLOSE     ;VALVE CLOSE ON  
        CLR V_OPEN       ;VALVE OPEN OFF
        MOV DPTR,#PORT_A
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP BEGIN
CONTRO1:NOP             ;READ INPUTS
        MOV C,AUT_ON
        MOV P_START,C
        MOV DPTR,#PORT_B
        MOV R0,#INPUT
        CLR P1.1          ;ENABLE I/O
        NOP
        MOVX A,@DPTR
        MOV @R0,A
        SETB P1.1         ;DISABLE I/O
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1A   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1A:  NOP
        JNB ANALOG,LAB2A  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2A:  NOP
        JNB CLOCK,LAB3A   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3A:  NOP
        JNB WIDOG,LAB4A   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4A:  NOP
        JNB COMMS,LAB5AZ  ;XMIT SUBROUTINE
        LCALL SUB5
LAB5AZ: NOP
        JNB COMM1,LAB5A   ;XMIT SUBROUTINE
        LCALL SUB6
LAB5A:  NOP                                   
        JNB V_POS,LAB23
        CLR V_CLOSE
        CLR V_OPEN
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP CONTRO2
LAB23:  NOP               ;CHECK VALVE POSITION
        CLR FLOW
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#01H  ;CHANNEL 1
        ORL ADCON,#08H  ;START ADC
LAB24:  NOP
        MOV A,ADCON
        JNB 0E4H,LAB24
        CLR C
        MOV R0,#6EH
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,@R0      ;SUBTRACT DEFINED OPEN POSITION
        JC LAB25
        SETB V_POS      ;SET POSITION REACHED BIT
        LJMP CONTRO1
LAB25:  NOP             ;OPEN VALVE
        SETB V_OPEN
        CLR V_CLOSE
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        JNB RUN,LAB35   ;CHECK IF RUN SIGNAL=1
        LJMP LEVEL      ;JUMP TO CHECK LEVEL
LAB35:  NOP             ;CHECK FOR HIGH PRESSURE IE.>4bar  135 DIGITAL
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#02H  ;CHANNEL 2
        ORL ADCON,#08H  ;START ADC
LAB26:  NOP
        MOV A,ADCON
        JNB 0E4H,LAB26
        CLR C
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,#135
        JNC LEVEL      
        CLR V_OPEN   
        CLR V_CLOSE
        CLR V_POS
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP CLOSE
LEVEL:  NOP             ;CHECK LEVEL
        CLR C
        MOV R0,#70H     ;ADC LOW BYTE
        MOV A,@R0
        MOV R0,#6FH     ;DEFAULT LEVEL TO ACC
        SUBB A,@R0
        MOV R0,#71H     ;ADC HIGH BYTE TO ACC
        MOV A,@R0
        SUBB A,#00H
        JNC LAB28       ;LEVEL OK!
LAB36:  NOP             ;START OF LOW LEVEL LOCKOUT
        JB DELAY2,LAB29 ;CHECK IF TIMER HAS BEEN STARTED
        MOV R3,#00H     ;CLEAR TIMER
        SETB DELAY2     ;START TIMER
LAB29:  NOP             ;CHECK FOR TIME OUT
        JB DELAY2A,LAB31
        JNB DELAY2A,LAB30
LAB28:  NOP
        CLR DELAY2
        CLR DELAY2A
        MOV R3,#00H
LAB30:  NOP
        LJMP CONTRO1
LAB31:  NOP              ;TIMER RUN OUT
        CLR V_OPEN
        CLR V_CLOSE
        SETB L_LEV
        CLR V_POS
        CLR AUT_ON
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        CLR DELAY2
        CLR DELAY2A
        MOV R3,#00H
LAB32:  NOP
        MOV DPTR,#PORT_B
        MOV R0,#INPUT
        CLR P1.1          ;ENABLE I/O
        NOP
        MOVX A,@DPTR
        MOV @R0,A
        SETB P1.1         ;DISABLE I/O
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1B   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1B:  NOP
        JNB ANALOG,LAB2B  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2B:  NOP
        JNB CLOCK,LAB3B   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3B:  NOP
        JNB WIDOG,LAB4B   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4B:  NOP
        JNB COMMS,LAB5BZ  ;XMIT SUBROUTINE
        LCALL SUB5
LAB5BZ: NOP
        JNB COMM1,LAB5B   ;XMIT SUBROUTINE
        LCALL SUB6
LAB5B:  NOP                                   
        JNB RUN,LAB33   ;CHECK IF RUN SIGNAL=1
        LJMP LAB32      ;JUMP TO START OF LOOP
LAB33:  NOP             ;CHECK FOR HIGH PRESSURE IE.>4bar  135 DIGITAL
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#02H  ;CHANNEL 2
        ORL ADCON,#08H  ;START ADC
LAB34:  NOP
        MOV A,ADCON
        JNB 0E4H,LAB34
        CLR C
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,#135
        JNC LAB32
        CLR L_LEV
        CLR V_CLOSE
        CLR V_OPEN
        CLR P_START
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP CLOSE
CONTRO2:NOP               ;READ INPUTS
        MOV C,AUT_ON
        MOV P_START,C
        MOV DPTR,#PORT_B
        MOV R0,#INPUT
        CLR P1.1          ;ENABLE I/O
        NOP
        MOVX A,@DPTR
        MOV @R0,A
        SETB P1.1         ;DISABLE I/O
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1C   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1C:  NOP
        JNB ANALOG,LAB2C  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2C:  NOP
        JNB CLOCK,LAB3C   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3C:  NOP
        JNB WIDOG,LAB4C   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4C:  NOP
        JNB COMMS,LAB5CZ  ;XMIT SUBROUTINE
        LCALL SUB5
LAB5CZ: NOP
        JNB COMM1,LAB5C   ;XMIT SUBROUTINE
        LCALL SUB6
LAB5C:  NOP             ;CHECK LEVEL          
        CLR C
        MOV R0,#70H     ;ADC LOW BYTE
        MOV A,@R0
        MOV R0,#6FH     ;DEFAULT LEVEL TO ACC
        SUBB A,@R0
        MOV R0,#71H     ;ADC HIGH BYTE TO ACC
        MOV A,@R0
        SUBB A,#00H
        JNC LAB38       ;LEVEL OK!
        LJMP LAB36      ;JUMP TO LOW LEVEL LOCKOUT ROUTINE
LAB38:  NOP
        CLR DELAY2
        CLR DELAY2A
        MOV R3,#00H
        JNB RUN,LAB39   ;CHECK IF RUN SIGNAL=1
        LJMP LAB41      ;JUMP TO DO FLOW CONTROL
LAB39:  NOP             ;CHECK FOR HIGH PRESSURE IE.>4bar  135 DIGITAL
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#02H  ;CHANNEL 2
        ORL ADCON,#08H  ;START ADC
LAB40:  NOP
        MOV A,ADCON
        JNB 0E4H,LAB40
        CLR C
        MOV A,ADCH      ;ADC RESULT TO ACC
        SUBB A,#135
        JNC LAB41
        CLR V_OPEN   
        CLR V_CLOSE
        CLR V_POS
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        LJMP BEGIN
LAB41:  NOP             ;FLOW CONTROL        
        JB FLOW,LAB42
        LJMP CONTRO2
LAB42:  NOP             ;CHECK FOR NO FLOW
        JB DELAY1,LAB45
        MOV R0,#64H
        MOV A,@R0
        CJNE A,#00H,LAB43
        CLR V_POS
        CLR FLOW
        LJMP CONTRO1
LAB43:  NOP             ;CHECK IF FLOW > DEFAULT
        MOV R0,#6CH     ;DEFAULT FLOW
        MOV A,@R0
        MOV R0,#64H     ;ACTUAL FLOW
        CLR C
        SUBB A,@R0      ;DEFAULT FLOW - ACTUAL FLOW
        JC LAB44        ;ACT > DEFAULT
        LJMP LAB60      ;JUMP TO CHECK FLOW < (DEFAULT-2)
LAB44:  NOP             ;PULSE CLOSE VALVE
        SETB V_CLOSE
        CLR V_OPEN
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        JB DELAY1,LAB45 ;CHECK IF TIMER HAS BEEN STARTED
        MOV R4,#00H     ;CLEAR TIMER
        SETB DELAY1     ;START TIMER
LAB45:  NOP             ;CHECK FOR TIME OUT
        JB DELAY1A,LAB46
        LJMP CONTRO2
LAB46:  NOP              ;TIMER RUN OUT
        CLR V_OPEN
        CLR V_CLOSE
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        CLR DELAY1
        CLR DELAY1A
        MOV R4,#00H
        CLR FLOW
        LJMP CONTRO2
LAB60:  NOP                                     
        JNB WEDOG,LAB1D   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1D:  NOP
        JNB ANALOG,LAB2D  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2D:  NOP
        JNB CLOCK,LAB3D   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3D:  NOP
        JNB WIDOG,LAB4D   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4D:  NOP
        JNB COMMS,LAB5DZ  ;XMIT SUBROUTINE
        LCALL SUB5
LAB5DZ: NOP
        JNB COMM1,LAB5D   ;XMIT SUBROUTINE
        LCALL SUB6
LAB5D:  NOP             ;CHECK FLOW < (DEFAULT -2)
        JB DELAY1,LAB48 ;CHECK IF TIMER HAS BEEN STARTED
        MOV R0,#6CH     ;DEFAULT FLOW
        MOV A,@R0
        ADD A,#99H
        DA A
        ADD A,#99H
        DA A            ;(DEFAULT -2)
        MOV R0,#44H     ;(DEFAULT-2) TO BYTE 44
        MOV @R0,A
        MOV R0,#64H     ;ACTUAL FLOW
        MOV A,@R0
        MOV R0,#44H
        CLR C
        SUBB A,@R0      ;ACT.FLOW -(DEFAULT -2)    
        JC LAB47        ;ACT < (DEFAULT-2)
        CLR FLOW
        CLR DELAY1
        CLR DELAY1A
        MOV R4,#00H
        LJMP CONTRO2    ;DEFAULT >= ACT FLOW =< (DEFAULT-2)
LAB47:  NOP             ;PULSE OPEN VALVE
        CLR V_CLOSE
        SETB V_OPEN
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        JB DELAY1,LAB48 ;CHECK IF TIMER HAS BEEN STARTED
        MOV R4,#00H     ;CLEAR TIMER
        SETB DELAY1     ;START TIMER
LAB48:  NOP             ;CHECK FOR TIME OUT
        JB DELAY1A,LAB49
        LJMP LAB60  
LAB49:  NOP              ;TIMER RUN OUT
        CLR V_OPEN
        CLR V_CLOSE
        MOV DPTR,#PORT_A    ;OUTPUT ROUTINE
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        CLR DELAY1
        CLR DELAY1A
        MOV R4,#00H
        CLR FLOW
        LJMP CONTRO2
  ;********************************************************************      
       
SUB1:   NOP         ;EXT WATCH DOG SUBROUTINE
        MOV C,WEDOG
        ANL C,/DOG
        MOV DOG,C
        CLR WEDOG   
        SETB WIDOG  ;ENABLE INTERNAL WATCH DOG ROUTINE
        MOV DPTR,#PORT_A
        MOV R0,#OUTPUT
        CLR P1.1
        MOV A,@R0
        MOVX @DPTR,A
        SETB P1.1
        RET
SUB2:   NOP         ;ANALOG CONVERSION SUBROUTINE
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#00H  ;CHANNEL 0
        ORL ADCON,#08H  ;START ADC
LOOP1:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP1
        MOV R0,#71H
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 71
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 70
        NOP         ;*************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#01H  ;CHANNEL 1
        ORL ADCON,#08H  ;START ADC
LOOP2:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP2
        MOV R0,#73H
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 73
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 72
        NOP        ;***************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#02H  ;CHANNEL 2
        ORL ADCON,#08H  ;START ADC
LOOP3:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP3
        MOV R0,#75H
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 75
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 74
        NOP        ;*************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#03H  ;CHANNEL 3
        ORL ADCON,#08H  ;START ADC
LOOP4:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP4
        MOV R0,#77H
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 77
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 76
        NOP       ;****************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#04H  ;CHANNEL 4
        ORL ADCON,#08H  ;START ADC
LOOP5:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP5
        MOV R0,#79H
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 79
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 78
        NOP       ;***************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#05H  ;CHANNEL 5
        ORL ADCON,#08H  ;START ADC
LOOP6:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP6
        MOV R0,#7BH
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 7B
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 7A
        NOP     ;********************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#06H  ;CHANNEL 6
        ORL ADCON,#08H  ;START ADC
LOOP7:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP7
        MOV R0,#7DH
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 7D
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 7C
        NOP     ;*********************************************
        MOV ADCON,#00H  ;CLEAR ADCON
        MOV ADCON,#07H  ;CHANNEL 7
        ORL ADCON,#08H  ;START ADC
LOOP8:  NOP
        MOV A,ADCON
        JNB 0E4H,LOOP8
        MOV R0,#7FH
        MOV A,ADCH
        MOV B,#64H      ;BIN TO BCD CONVERSION
        DIV AB
        MOV @R0,A       ;HUNDREDS TO BYTE 7F
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        DEC R0
        MOV @R0,A       ;10'S AND 1'S TO BYTE 7E
        CLR ANALOG
        RET
SUB3:   NOP          ;TIME CLOCK ROUTINE
        MOV R0,#65H
        MOV A,#10H    ;10SEC
        ADD A,@R0
        MOV @R0,A
        CJNE @R0,#60H,FINISH
        NOP
        MOV @R0,#00H   ;CLEAR SECONDS
        ;*********************************
        INC R0         ;66
        MOV A,#01H     ;UPDATE MINUTES
        ADD A,@R0
        DA A
        MOV @R0,A
        CJNE @R0,#60H,FINISH
        MOV @R0,#00H   ;CLEAR MINUTES
        INC R0         ;67
        MOV A,#01H     ;UPDATE HOURS  
        ADD A,@R0
        DA A
        MOV @R0,A
        CJNE @R0,#24H,FINISH
        MOV @R0,#00H   ;CLEAR HOURS 1 DAY EXPIRED CLEAR COUNTERS
        NOP
        MOV R0,#60H    ;CLEAR TOTAL FLOW
        MOV @R0,#00H
        INC R0         ;61
        MOV @R0,#00H
        INC R0         ;62
        MOV @R0,#00H
        INC R0         ;63
        MOV @R0,#00H
FINISH: NOP
        SETB COMM1
        CLR CLOCK
        RET
SUB4:   NOP          ;INTERNAL WATCH DOG SUBROUTINE
        ORL PCON,#10H
        MOV T3,#156  ;200mSEC TIME OUT
        CLR WIDOG    
        RET
SUB5:   NOP
        CLR REN         ;DISABLE RECEPTION
        CLR ES          ;DISABLE SERIAL INTERRUPT
        CLR COMMS
        CLR P1.4        ;ENABLE 485 XMITTER
        MOV SBUF,#55H
WAIT1:  NOP
        JNB TI,WAIT1
        CLR TI
        MOV SBUF,#55H
WAIT2:  NOP
        JNB TI,WAIT2
        CLR TI
        MOV SBUF,#55H
WAIT3:  NOP
        JNB TI,WAIT3
        CLR TI
        MOV SBUF,#55H
WAIT4:  NOP
        JNB TI,WAIT4
        CLR TI
        MOV SBUF,#2CH      ; ,
WAIT75: NOP
        JNB TI,WAIT75
        CLR TI
        MOV SBUF,#DIG1
WAIT5:  NOP
        JNB TI,WAIT5
        CLR TI
        MOV SBUF,#DIG2
WAIT6:  NOP
        JNB TI,WAIT6
        CLR TI
        MOV SBUF,#DIG3
WAIT7:  NOP
        JNB TI,WAIT7
        CLR TI
        MOV SBUF,#DIG4
WAIT8:  NOP
        JNB TI,WAIT8
        CLR TI
        MOV SBUF,#DIG5
WAIT9:  NOP
        JNB TI,WAIT9
        CLR TI
        MOV SBUF,#3AH      ;  ***  :   ****
WAIT10: NOP
        JNB TI,WAIT10
        CLR TI
      ;******************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1E   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1E:  NOP
        JNB ANALOG,LAB2E  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2E:  NOP
        JNB CLOCK,LAB3E   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3E:  NOP
        JNB WIDOG,LAB4E   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4E:  NOP
        MOV R0,#67H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H       ;HOURS
        SETB 0E4H
        MOV SBUF,A
WAIT11: NOP
        JNB TI,WAIT11
        CLR TI
        MOV R0,#67H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT12: NOP
        JNB TI,WAIT12
        CLR TI
    ;*****************************************************
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT10A:NOP
        JNB TI,WAIT10A
        CLR TI
        MOV R0,#66H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H         ;MINUTES
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT13: NOP
        JNB TI,WAIT13
        CLR TI
        MOV R0,#66H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT14: NOP
        JNB TI,WAIT14
        CLR TI
     ;***************************************************
        MOV SBUF,#3AH      ;  ***  :   ****
WAIT15: NOP
        JNB TI,WAIT15
        CLR TI
     ;***************************************************   
        MOV R0,#71H
        MOV A,@R0
        CJNE A,#00H,LAB70
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB71
        MOV SBUF,#30H
WAIT16: NOP
        JNB TI,WAIT16
        CLR TI
        LJMP LAB72
LAB70:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC0   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT17: NOP
        JNB TI,WAIT17
        CLR TI
LAB71:  NOP
        MOV R0,#70H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT18: NOP
        JNB TI,WAIT18
        CLR TI
        MOV R0,#70H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT19: NOP
        JNB TI,WAIT19
        CLR TI
LAB72:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT20: NOP
        JNB TI,WAIT20 
        CLR TI
    ;*************************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1F   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1F:  NOP
        JNB ANALOG,LAB2F  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2F:  NOP
        JNB CLOCK,LAB3F   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3F:  NOP
        JNB WIDOG,LAB4F   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4F:  NOP
        MOV R0,#73H
        MOV A,@R0
        CJNE A,#00H,LAB73
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB74
        MOV SBUF,#30H
WAIT21: NOP
        JNB TI,WAIT21
        CLR TI
        LJMP LAB75
LAB73:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC1   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT22: NOP
        JNB TI,WAIT22
        CLR TI
LAB74:  NOP
        MOV R0,#72H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT23: NOP
        JNB TI,WAIT23
        CLR TI
        MOV R0,#72H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT24: NOP
        JNB TI,WAIT24
        CLR TI
LAB75:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT25: NOP
        JNB TI,WAIT25 
        CLR TI
    ;*************************************************************
        MOV R0,#75H
        MOV A,@R0
        CJNE A,#00H,LAB76
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB77
        MOV SBUF,#30H
WAIT26: NOP
        JNB TI,WAIT26
        CLR TI
        LJMP LAB78
LAB76:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC2   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT27: NOP
        JNB TI,WAIT27
        CLR TI
LAB77:  NOP
        MOV R0,#74H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT28: NOP
        JNB TI,WAIT28
        CLR TI
        MOV R0,#74H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT29: NOP
        JNB TI,WAIT29
        CLR TI
LAB78:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT30: NOP
        JNB TI,WAIT30 
        CLR TI
    ;*************************************************************
        MOV R0,#77H
        MOV A,@R0
        CJNE A,#00H,LAB79
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB80
        MOV SBUF,#30H
WAIT31: NOP
        JNB TI,WAIT31
        CLR TI
        LJMP LAB81
LAB79:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC3   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT32: NOP
        JNB TI,WAIT32
        CLR TI
LAB80:  NOP
        MOV R0,#76H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT33: NOP
        JNB TI,WAIT33
        CLR TI
        MOV R0,#76H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT34: NOP
        JNB TI,WAIT34
        CLR TI
;******************************************************************
LAB81:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT35: NOP
        JNB TI,WAIT35
        CLR TI
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1G   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1G:  NOP
        JNB ANALOG,LAB2G  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2G:  NOP
        JNB CLOCK,LAB3G   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3G:  NOP
        JNB WIDOG,LAB4G   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4G:  NOP
           
;*************************************************************
        MOV R0,#79H
        MOV A,@R0
        CJNE A,#00H,LAB82
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB83
        MOV SBUF,#30H
WAIT36: NOP
        JNB TI,WAIT36
        CLR TI
        LJMP LAB84
LAB82:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC4   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT37: NOP
        JNB TI,WAIT37
        CLR TI
LAB83:  NOP
        MOV R0,#78H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT38: NOP
        JNB TI,WAIT38
        CLR TI
        MOV R0,#78H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT39: NOP
        JNB TI,WAIT39
        CLR TI
LAB84:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT40: NOP
        JNB TI,WAIT40 
        CLR TI
    ;*************************************************************
        MOV R0,#7BH
        MOV A,@R0
        CJNE A,#00H,LAB85
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB86
        MOV SBUF,#30H
WAIT41: NOP
        JNB TI,WAIT41
        CLR TI
        LJMP LAB87
LAB85:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC5   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT42: NOP
        JNB TI,WAIT42
        CLR TI
LAB86:  NOP
        MOV R0,#7AH
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT43: NOP
        JNB TI,WAIT43
        CLR TI
        MOV R0,#7AH
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT44: NOP
        JNB TI,WAIT44
        CLR TI
LAB87:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT45: NOP
        JNB TI,WAIT45 
        CLR TI
    ;*************************************************************
        MOV R0,#7DH
        MOV A,@R0
        CJNE A,#00H,LAB88
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB89
        MOV SBUF,#30H
WAIT46: NOP
        JNB TI,WAIT46
        CLR TI
        LJMP LAB90
LAB88:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC6   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT47: NOP
        JNB TI,WAIT47
        CLR TI
LAB89:  NOP
        MOV R0,#7CH                         
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT48: NOP
        JNB TI,WAIT48
        CLR TI
        MOV R0,#7CH                        
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT49: NOP
        JNB TI,WAIT49
        CLR TI
LAB90:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT50: NOP
        JNB TI,WAIT50 
        CLR TI
    ;*************************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1H   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1H:  NOP
        JNB ANALOG,LAB2H  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2H:  NOP
        JNB CLOCK,LAB3H   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3H:  NOP
        JNB WIDOG,LAB4H   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4H:  NOP
        MOV R0,#7FH
        MOV A,@R0
        CJNE A,#00H,LAB91
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB92
        MOV SBUF,#30H
WAIT51: NOP
        JNB TI,WAIT51
        CLR TI
        LJMP LAB93
LAB91:  NOP
        CLR 0E7H
        CLR 0E6H         ;ADC7   
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT52: NOP
        JNB TI,WAIT52
        CLR TI
LAB92:  NOP
        MOV R0,#7EH                                 
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                 
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT53: NOP
        JNB TI,WAIT53
        CLR TI
        MOV R0,#7EH                           
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT54: NOP
        JNB TI,WAIT54
        CLR TI
LAB93:  NOP
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT55: NOP
        JNB TI,WAIT55 
        CLR TI
    ;*************************************************************
        MOV R0,#INPUT      
        MOV B,#0AH         ; INPUT STATUS
        MOV A,@R0
        DIV AB
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
        MOV A,B
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
WAIT56: NOP
        JNB TI,WAIT56 
        CLR TI
        MOV SBUF,A
WAIT57: NOP
        JNB TI,WAIT57 
        CLR TI
   ;***************************************************************
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT61: NOP
        JNB TI,WAIT61 
        CLR TI
        MOV R0,#64H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H         ;FLOW / SEC
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT58: NOP
        JNB TI,WAIT58
        CLR TI
        MOV R0,#64H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT59: NOP
        JNB TI,WAIT59
        CLR TI
  ;*******************************************************************
        MOV SBUF,#2CH      ;  ***  ,   ****
WAIT62: NOP
        JNB TI,WAIT62 
        CLR TI
  ;********************************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1J   ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1J:  NOP
        JNB ANALOG,LAB2J  ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2J:  NOP
        JNB CLOCK,LAB3J   ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3J:  NOP
        JNB WIDOG,LAB4J   ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4J:  NOP
        MOV R0,#63H        ;TOTAL FLOW MAX 99 999 999 LITRES
        MOV A,@R0
        CJNE A,#00H,LAB94
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB95
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB96
        DEC R0
        MOV A,@R0
        CJNE A,#00H,LAB100
        LJMP LAB99
        NOP
LAB100: LJMP LAB97
LAB99:  MOV SBUF,#30H
WAIT63: NOP
        JNB TI,WAIT63
        CLR TI
        LJMP LAB98
LAB94:  NOP
        MOV R0,#63H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                      
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT64: NOP
        JNB TI,WAIT64
        CLR TI
        MOV R0,#63H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT65: NOP
        JNB TI,WAIT65
        CLR TI
LAB95:  NOP
        MOV R0,#62H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                       
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT66: NOP
        JNB TI,WAIT66
        CLR TI
        MOV R0,#62H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT67: NOP
        JNB TI,WAIT67
        CLR TI
LAB96:  NOP
        MOV R0,#61H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                    
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT68: NOP
        JNB TI,WAIT68
        CLR TI
        MOV R0,#61H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT69: NOP
        JNB TI,WAIT69
        CLR TI
LAB97:  NOP
        MOV R0,#60H
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H                         
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT70: NOP
        JNB TI,WAIT70
        CLR TI
        MOV R0,#60H
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT71: NOP
        JNB TI,WAIT71
        CLR TI
LAB98:  NOP
        MOV SBUF,#26H      ;  ***  &   ****
WAIT72: NOP
        JNB TI,WAIT72 
        CLR TI
        MOV SBUF,#0DH      ;  ***  LF/CR  ****
WAIT73: NOP
        JNB TI,WAIT73 
        CLR TI
        CLR RI
        SETB P1.4       ;DISABLE 485 XMITTER
        MOV R0,#90H     ;CLEAR COMMS BUFFER
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        INC R0
        MOV @R0,#00H
        CLR COMMS
        SETB ES         ;ENABLE SERIAL INTERRUPT
        SETB REN
        RET
SUB6:   NOP
        CLR REN         ;DISABLE RECEPTION
        CLR ES          ;DISABLE SERIAL INTERRUPT
        CLR COMM1
        MOV SBUF,#3CH   ;SEND START CHARACTER
WAIT1A: NOP
        JNB TI,WAIT1A
        CLR TI
        MOV SBUF,67H    ;67
WAIT2A: NOP
        JNB TI,WAIT2A
        CLR TI
        MOV SBUF,66H    ;66
WAIT3A: NOP
        JNB TI,WAIT3A
        CLR TI
        MOV SBUF,71H    ;71
WAIT4A: NOP
        JNB TI,WAIT4A
        CLR TI
        MOV SBUF,70H    ;70
WAIT5A: NOP
        JNB TI,WAIT5A
        CLR TI
        MOV SBUF,73H    ;73
WAIT6A: NOP
        JNB TI,WAIT6A
        CLR TI
        MOV SBUF,72H    ;72
WAIT7A: NOP
        JNB TI,WAIT7A
        CLR TI
        MOV SBUF,75H    ;75
WAIT8A: NOP
        JNB TI,WAIT8A
        CLR TI
        MOV SBUF,74H    ;74
WAIT9A: NOP
        JNB TI,WAIT9A
        CLR TI
        MOV SBUF,77H    ;77                 
WAIT10B:NOP
        JNB TI,WAIT10B
        CLR TI
      ;******************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1E1  ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1E1: NOP
        JNB ANALOG,LAB2E1 ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2E1: NOP
        JNB CLOCK,LAB3E1  ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3E1: NOP
        JNB WIDOG,LAB4E1  ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4E1: NOP
        MOV SBUF,76H      ;76
WAIT11A:NOP
        JNB TI,WAIT11A
        CLR TI
        MOV SBUF,79H      ;79
WAIT12A:NOP
        JNB TI,WAIT12A
        CLR TI
        MOV SBUF,78H      ;78
WAIT13A:NOP
        JNB TI,WAIT13A
        CLR TI
        MOV SBUF,7BH      ;7B
WAIT14A:NOP
        JNB TI,WAIT14A
        CLR TI
        MOV SBUF,7AH      ;7A              
WAIT15A:NOP
        JNB TI,WAIT15A
        CLR TI
        MOV SBUF,7DH      ;7D
WAIT16A:NOP
        JNB TI,WAIT16A
        CLR TI
        MOV SBUF,7CH      ;7C
WAIT17A:NOP
        JNB TI,WAIT17A
        CLR TI
        MOV SBUF,7FH      ;7F
WAIT18A:NOP
        JNB TI,WAIT18A
        CLR TI
        MOV SBUF,7EH      ;7E
WAIT19A:NOP
        JNB TI,WAIT19A
        CLR TI
    ;*************************************************************
        NOP               ;CHECK CONTROL BITS
        JNB WEDOG,LAB1F1  ;EXT W/DOG SUBROUTINE ENABLE
        LCALL SUB1
LAB1F1: NOP
        JNB ANALOG,LAB2F1 ;ANALOG CONVERSION SUBROUTINE ENABLE
        LCALL SUB2
LAB2F1: NOP
        JNB CLOCK,LAB3F1  ;TIME CLOCK SUBROUTINE ENABLE
        LCALL SUB3
LAB3F1: NOP
        JNB WIDOG,LAB4F1  ;INT W/DOG SUBROUTINE ENABLE
        LCALL SUB4
LAB4F1: NOP
        MOV SBUF,26H      ;26
WAIT21A:NOP
        JNB TI,WAIT21A
        CLR TI
        MOV SBUF,64H      ;64
WAIT22A:NOP
        JNB TI,WAIT22A
        CLR TI
        MOV SBUF,63H      ;63
WAIT23A:NOP
        JNB TI,WAIT23A
        CLR TI
        MOV SBUF,62H      ;62
WAIT24A:NOP
        JNB TI,WAIT24A
        CLR TI
        MOV SBUF,61H      ;61              
WAIT25A:NOP
        JNB TI,WAIT25A
        CLR TI
        MOV SBUF,60H      ;60
WAIT26A:NOP
        JNB TI,WAIT26A
        CLR TI
        MOV SBUF,#3EH      ;3E  >   END OF TRANSMISSION
WAIT27A:NOP
        JNB TI,WAIT27A
        CLR TI
        CLR RI
        SETB REN          ;ENABLE SERIAL RECEPTION
        CLR COMM1
        SETB ES
        RET
RECEIVE:NOP
        PUSH ACC
        PUSH PSW
        SETB RS0
        MOV A,SBUF
        CJNE A,#55H,LAB101  ;CHECK FOR U
        CLR COMMS
        MOV R0,#90H
        LJMP LAB110
LAB101: NOP
        CJNE A,#7EH,LAB102  ;CHECK FOR END OF TRANSMISSION ' ~ '
        CLR COMMS
        LJMP CH_ADD         ;GOTO CHECK ADDRESS
LAB102: NOP
        CJNE R0,#9FH,LAB102A ;CHECK FOR COMMS BUFFER FULL
        CLR COMMS
        MOV R0,#90H
        LJMP LAB110
LAB102A:NOP
        CLR COMMS
        MOV @R0,A
        INC R0
        LJMP LAB110
CH_ADD: NOP         ;CHECK ADDRESS
        MOV R0,#91H
        CJNE @R0,#DIG2,LAB301
        LJMP LAB300
LAB301: NOP
        LJMP LAB110
LAB300: NOP
        INC R0                  ;92
        CJNE @R0,#DIG3,LAB110
        INC R0                  ;93
        CJNE @R0,#DIG4,LAB110
        INC R0                  ;94
        CJNE @R0,#DIG5,LAB110
        MOV R0,#95H
        CJNE @R0,#44H,LAB103      ;CHECK FOR ' D '
        SETB COMMS
        LJMP LAB110
LAB103: NOP                       ;CHECK FOR ' T '
        MOV R0,#95H
        CJNE @R0,#54H,LAB104                       
        MOV R0,#97H               ;ENTER NEW HOURS    
        MOV A,@R0
        SWAP A
        INC R0      
        XCHD A,@R0
        MOV 67H,A   
        ;**********************************************************
        INC R0                    ;ENTER NEW MINUTES  
        MOV A,@R0
        SWAP A
        INC R0      
        XCHD A,@R0
        MOV 66H,A  
        LJMP LAB110
LAB104: NOP                       ;CHECK FOR ' L '
        MOV R0,#95H
        CJNE @R0,#4CH,LAB105                      
        MOV R0,#97H               ;ENTER NEW LEVEL    
        MOV A,@R0
        SWAP A
        INC R0     
        XCHD A,@R0
        MOV 6FH,A   
        LJMP LAB110
LAB105: NOP                       ;CHECK FOR ' F '
        MOV R0,#95H
        CJNE @R0,#46H,LAB106                       
        MOV R0,#97H               ;ENTER NEW FLOW RATE
        MOV A,@R0
        SWAP A
        INC R0     
        XCHD A,@R0
        MOV 6CH,A  
        LJMP LAB110
LAB106: NOP                       ;CHECK FOR ' m '
        MOV R0,#95H
        CJNE @R0,#6DH,LAB110                       
        MOV R0,#98H               ;CHECK FOR ' ON '
        CJNE @R0,#4EH,LAB107                       
        JNB AUTO,LAB110
        SETB AUT_ON
        LJMP LAB110
LAB107: NOP                       ;CHECK FOR ' OFF '
        MOV R0,#98H
        CJNE @R0,#46H,LAB110                       
        CLR AUT_ON
LAB110: NOP
        CLR RS0
        POP PSW
        POP ACC
        CLR RI          ;CLEAR RECEIVE INTERRUPT
        RETI



END
