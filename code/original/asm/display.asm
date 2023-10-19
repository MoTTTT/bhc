

                   ;BORE HOLE DISPLAY PROGRAM

        ;BYTE ALLOCATION
        ;ADC0----71,70   LEVEL          51,50
        ;ADC1----73,72   VALVE POSITION 53,52
        ;ADC2----75,74   PRESSURE       55,54
        ;ADC3----77,76    DIRECT DISPLAY
        ;ADC4----79,78       "      "
        ;ADC5----7B,7A       "      "
        ;ADC6----7D,7C       "      "
        ;ADC7----7F,7E       "      "
        ;TOTAL FLOW----63,62,61,60   DIRECT DISPLAY
        ;FLOW/SEC-----64             DIRECT DISPLAY
        ;TIME CLOCK-----67,66   HOURS MINUTES   DIRECT DISPLAY
        ;INPUT----26      5F,5E,5D,5C,5B,5A,59

        ;ADDRESS IN ASCII
        DIG1 EQU 33H
        DIG2 EQU 34H
        DIG3 EQU 30H
        DIG4 EQU 30H
        DIG5 EQU 31H
        
        ;DIGITAL INPUTS
        ;***************
        PHFAIL BIT 30H             
        AUTO BIT 31H
        MAN BIT 32H
       


$INCLUDE(REG552.PDF)

        LJMP SETUP
ORG 23H
        NOP
        LJMP RECEIVE

SETUP:  NOP
        CLR 0C7H        ;EN CLEAR CONTROL SIGNALS
        CLR 0C5H        ;R/S
        CLR 0C6H        ;R/W
        SETB 0C3H
        SETB 0C2H
        SETB 0C1H
        SETB 0C0H
        MOV P1,#00H     ;CLEAR PORT 1
        NOP
        MOV R0,#40H     ;CLEAR BYTES 40 TO 7F
        MOV R1,#64
LOOP:   NOP
        MOV @R0,#00H
        INC R0
        DJNZ R1,LOOP
        NOP
        MOV 26H,#00H    ;CLEAR DIGITAL INPUT BYTE
        MOV SP,#0F0H    ;MOV STACK POINTER TO START AT 3rD REG BANK
        MOV TMOD,#20H   ;SET T1 AS AUTO RELOAD
        MOV TH1,#250    ;4800 BAUD
        MOV TL1,#250
        MOV SCON,#50H   ;SET COMMS MODE 1 
        SETB EA         ;ENABLE ALL INTERRUPTS
        SETB ES         ;ENABLE SERIAL INTERRUPT
        SETB TR1        ;START TIMER 1
        LCALL LCDSET
        NOP
        LJMP SCREEN1 
LCDSET: NOP
        MOV P1,#38H     ;SET FOR 8 BIT MODE
        MOV R7,#0FFH
DELAY1: NOP
        DJNZ R7,DELAY1
        SETB 0C7H      ;ENABLE HIGH
        MOV R7,#80H
DELAY2: NOP
        DJNZ R7,DELAY2
        CLR 0C7H       ;ENABLE LOW
        MOV R7,#80H
DELAY3: NOP
        DJNZ R7,DELAY3
        MOV P1,#06H    ;DISPLAY FREEZE AND INCREMENT
        MOV R7,#80H  
DELAY4: NOP
        DJNZ R7,DELAY4
        SETB 0C7H      ;ENABLE HIGH
        MOV R7,#80H
DELAY5: NOP
        DJNZ R7,DELAY5
        CLR 0C7H       ;ENABLE LOW
        MOV R7,#80H
DELAY6: NOP
        DJNZ R7,DELAY6
        MOV P1,#0CH    ;DISPLAY ON,CURSOR OFF        
        MOV R7,#80H  
DELAY7: NOP
        DJNZ R7,DELAY7
        SETB 0C7H      ;ENABLE HIGH
        MOV R7,#80H
DELAY8: NOP
        DJNZ R7,DELAY8
        CLR 0C7H       ;ENABLE LOW
        MOV R7,#80H
DELAY9: NOP
        DJNZ R7,DELAY9
        RET
;************************************************************************
SCREEN1:NOP
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB1:   NOP
        JNB T2OV,LAB1
        MOV TM2CON,#00H 
        CLR T2OV
        ;******LEVEL CALCULATION********
        MOV 51H,#00H  ;CLEAR BYTE 51
        MOV 50H,#00H  ;CLEAR BYTE 50
        MOV R5,#00H
        ;****CONVERT BYTE 71,70 TO HEX VALUE*******
        MOV B,#64H    ;100 TO B REGISTER
        MOV A,71H     ;BYTE 71 TO ACC
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        MOV R5,A      ;HEX VALUE TO R5
        MOV B,#0AH    ;10 TO B REGISTER
        MOV A,70H
        SWAP A
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        ADD A,R5
        MOV R5,A
        MOV A,70H
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        ADD A,R5
        MOV R5,A
        CLR C
        MOV A,R5      ;R5-55
        SUBB A,#55
        MOV R5,A
        JNC LAB50
        LJMP LAB51
LAB50:  NOP     
        MOV B,#04H    ;ADD .25 OF R5 TO R5
        MOV A,R5
        DIV AB
        ADD A,R5
        MOV R5,A
        ;BIN TO BCD CONVERSION
        ;*********************
        MOV A,R5
        MOV B,#64H
        DIV AB
        MOV 51H,A
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        MOV 50H,A
LAB51:  NOP
        ;******PRESSURE CALCULATION********
        MOV 55H,#00H  ;CLEAR BYTE 55
        MOV 54H,#00H  ;CLEAR BYTE 54
        MOV R5,#00H
        ;****CONVERT BYTE 75,74 TO HEX VALUE*******
        MOV B,#64H    ;100 TO B REGISTER
        MOV A,75H     ;BYTE 71 TO ACC
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        MOV R5,A      ;HEX VALUE TO R5
        MOV B,#0AH    ;10 TO B REGISTER
        MOV A,74H
        SWAP A
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        ADD A,R5
        MOV R5,A
        MOV A,74H
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        ADD A,R5
        MOV R5,A
        CLR C
        MOV A,R5      ;R5-55
        SUBB A,#55
        MOV R5,A
        JNC LAB50A
        LJMP LAB51A
LAB50A: NOP     
        MOV B,#02H    ;R5 DIV BY 2         
        MOV A,R5
        DIV AB
        MOV R5,A
        ;BIN TO BCD CONVERSION
        ;*********************
        MOV A,R5
        MOV B,#64H
        DIV AB
        MOV 55H,A
        MOV A,B
        MOV B,#0AH
        DIV AB
        SWAP A
        ADD A,B
        MOV 54H,A
LAB51A: NOP
        ;******VALVE POSITION CALCULATION********
        MOV 53H,#00H  ;CLEAR BYTE 53
        MOV 52H,#00H  ;CLEAR BYTE 52
        MOV R3,#00H
        MOV R4,#00H
        MOV R5,#00H
        MOV R6,#00H
        ;****CONVERT BYTE 73,72 TO HEX VALUE*******
        MOV B,#64H    ;100 TO B REGISTER
        MOV A,73H     ;BYTE 71 TO ACC
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        MOV R5,A      ;HEX VALUE TO R5
        MOV B,#0AH    ;10 TO B REGISTER
        MOV A,72H
        SWAP A
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        MUL AB
        ADD A,R5
        MOV R5,A
        MOV A,72H
        CLR 0E7H      ;CLEAR HIGH NIBBLE
        CLR 0E6H
        CLR 0E5H
        CLR 0E4H
        ADD A,R5
        MOV R5,A
        CLR C
        MOV A,R5      ;R5-15
        SUBB A,#16
        MOV R5,A
        JNC LAB50B 
        LJMP LAB51B
LAB50B: NOP     
        MOV B,#0AH    ;R5 * 10            
        MOV A,R5
        MUL AB
        MOV R5,A
        MOV R6,B
LAB52B: NOP
        CLR C
        MOV A,#24
        ADD A,R3
        MOV R3,A
        CLR A
        ADDC A,R4
        MOV R4,A
        CLR C
        MOV A,R3
        SUBB A,R5
        MOV A,R4
        SUBB A,R6
        JNC LAB51C
        CLR C
        MOV A,52H
        ADD A,#01H
        DA A
        MOV 52H,A
        CLR A
        ADDC A,53H
        DA A
        MOV 53H,A
        LJMP LAB52B
LAB51C: NOP
        CLR C
        MOV A,52H
        ADD A,#01H
        DA A
        MOV 52H,A
        CLR A
        ADDC A,53H
        DA A
        MOV 53H,A
LAB51B: NOP
        MOV P1,#80H     ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3AH    ;:
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,51H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,50H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#2CH    ;,
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,50H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4DH    ;M     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH    ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#56H    ;V
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#50H    ;P
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3AH    ;:
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,53H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,52H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,52H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#25H    ;PERCENT
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#50H    ;P
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3AH    ;:
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,55H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,54H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#2CH    ;,
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,54H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#42H    ;B
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,67H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,67H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#48H    ;H
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,66H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,66H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
 ;*******************************************************
 ;*******SCAN KEYBOARD************
        CLR 0C3H        ;CHECK FOR OPEN              
        NOP
        MOV A,P5
        JB 0E0H,LAB13E 
        SETB 0C3H
        LCALL SEND4
        NOP
        LJMP SCREEN1
LAB13E: NOP
        SETB 0C3H
        CLR 0C0H        ;CHECK FOR CLOSE             
        NOP
        MOV A,P5
        JB 0E0H,LAB13F 
        SETB 0C0H
        LCALL SEND5
        NOP
        LJMP SCREEN1
LAB13F: NOP
        SETB 0C0H
        CLR 0C1H        ;CHECK FOR DOWM ARROW
        NOP
        MOV A,P5
        JB 0E0H,LAB8   
        SETB 0C1H
        LJMP SCREEN2                          
LAB8:   NOP
        SETB 0C1H       ;CHECK FOR *
        CLR 0C0H
        NOP
        MOV A,P5
        JB 0E4H,LAB9   
        SETB 0C0H
        LJMP SCREEN4                          
LAB9:   NOP
        SETB 0C0H
        LJMP SCREEN1
;************************************************************************
SCREEN2:NOP
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB2:   NOP
        JNB T2OV,LAB2
        MOV TM2CON,#00H 
        CLR T2OV
        ;CHECK STATUS
        JB PHFAIL,LAB16
        MOV 5FH,#50H    ;P
        MOV 5EH,#48H    ;H
        MOV 5DH,#46H    ;F
        LJMP LAB20
LAB16:  NOP
        MOV 5FH,#0FEH   ;SPACE
        MOV 5EH,#0FEH   ;SPACE
        MOV 5DH,#0FEH   ;SPACE
LAB20:  NOP
        JB AUTO,LAB17
        JB MAN,LAB18
        MOV 5CH,#4FH    ;O
        MOV 5BH,#46H    ;F
        MOV 5AH,#46H    ;F
        MOV 59H,#0FEH   ;SPACE
        LJMP LAB19
LAB17:  NOP
        MOV 5CH,#41H    ;A
        MOV 5BH,#55H    ;U
        MOV 5AH,#54H    ;T
        MOV 59H,#4FH    ;O     
        LJMP LAB19
LAB18:  NOP
        MOV 5CH,#4DH    ;M
        MOV 5BH,#41H    ;A
        MOV 5AH,#4EH    ;N
        MOV 59H,#0FEH   ;SPACE
LAB19:  NOP
        MOV P1,#80H     ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#46H    ;F
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#2FH    ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#53H    ;S
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3AH    ;:
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,64H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,64H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH    ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5FH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5EH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5DH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5CH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5BH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,5AH
        MOV P1,A       ;STATUS
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,59H
        MOV P1,A       ;STATUS 
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4FH    ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3AH    ;:
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,63H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,63H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,62H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,62H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,61H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,61H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,60H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,60H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
 ;*******SCAN KEYBOARD************
        CLR 0C3H        ;CHECK FOR OPEN              
        NOP
        MOV A,P5
        JB 0E0H,LAB13C 
        SETB 0C3H
        LCALL SEND4
        NOP
        LJMP SCREEN2
LAB13C: NOP
        SETB 0C3H
        CLR 0C0H        ;CHECK FOR CLOSE             
        NOP
        MOV A,P5
        JB 0E0H,LAB13D 
        SETB 0C0H
        LCALL SEND5
        NOP
        LJMP SCREEN2
LAB13D: NOP
        SETB 0C0H
        CLR 0C1H        ;CHECK FOR DOWM ARROW
        NOP
        MOV A,P5
        JB 0E0H,LAB10  
        SETB 0C1H
        LJMP SCREEN3                          
LAB10:  NOP
        SETB 0C1H       ;CHECK FOR *
        CLR 0C0H
        NOP
        MOV A,P5
        JB 0E4H,LAB11  
        SETB 0C0H
        LJMP SCREEN4                          
LAB11:  NOP
        SETB 0C0H
        CLR 0C1H        ;CHECK FOR UP ARROW
        NOP
        MOV A,P5
        JB 0E1H,LAB12  
        SETB 0C1H
        LJMP SCREEN1                          
LAB12:  NOP
        SETB 0C1H
        LJMP SCREEN2
;************************************************************************
SCREEN3:NOP
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB3:   NOP
        JNB T2OV,LAB3
        MOV TM2CON,#00H 
        CLR T2OV
        MOV P1,#80H     ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#53H    ;S
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#50H    ;P
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#44H     ;D    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#43H    ;C    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#27H    ;'
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#53H    ;S
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,77H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,76H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,76H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0      
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,79H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,78H
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,78H
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7BH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7AH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7AH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7DH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7CH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7CH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7FH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7EH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,7EH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;0
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
 ;*******SCAN KEYBOARD************
        CLR 0C3H        ;CHECK FOR OPEN              
        NOP
        MOV A,P5
        JB 0E0H,LAB13A 
        SETB 0C3H
        LCALL SEND4
        NOP
        LJMP SCREEN3
LAB13A: NOP
        SETB 0C3H
        CLR 0C0H        ;CHECK FOR CLOSE             
        NOP
        MOV A,P5
        JB 0E0H,LAB13B 
        SETB 0C0H
        LCALL SEND5
        NOP
        LJMP SCREEN3
LAB13B: NOP
        SETB 0C0H
        CLR 0C1H        ;CHECK FOR DOWM ARROW
        NOP
        MOV A,P5
        JB 0E0H,LAB13  
        SETB 0C1H
        LJMP SCREEN1                          
LAB13:  NOP
        SETB 0C1H       ;CHECK FOR *
        CLR 0C0H
        NOP
        MOV A,P5
        JB 0E4H,LAB14  
        SETB 0C0H
        LJMP SCREEN4                          
LAB14:  NOP
        SETB 0C0H
        CLR 0C1H        ;CHECK FOR UP ARROW
        NOP
        MOV A,P5
        JB 0E1H,LAB15  
        SETB 0C1H
        LJMP SCREEN2                          
LAB15:  NOP
        SETB 0C1H
        LJMP SCREEN3
;************************************************************************
SCREEN4:NOP            ;MENU
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB4:   NOP
        JNB T2OV,LAB4
        MOV TM2CON,#00H 
        CLR T2OV
        MOV P1,#80H    ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#31H    ;1
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#29H    ;)
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#49H    ;I
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4DH    ;M
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H   ;E    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE 
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH    ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#32H    ;2
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#29H    ;)
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L      
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#56H    ;V
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L      
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#33H    ;3
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#29H    ;)
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#46H    ;F
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4FH    ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#57H    ;W
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
 ;*******SCAN KEYBOARD************
        CLR 0C3H        ;CHECK FOR 1                     
        NOP
        MOV A,P5
        JB 0E4H,LAB21  
        SETB 0C3H
        LJMP SCREEN5                          
LAB21:  NOP
        SETB 0C3H       ;CHECK FOR 2          
        CLR 0C3H
        NOP
        MOV A,P5
        JB 0E3H,LAB22  
        SETB 0C3H
        LJMP SCREEN7                          
LAB22:  NOP
        SETB 0C3H
        CLR 0C3H        ;CHECK FOR 3                  
        NOP
        MOV A,P5
        JB 0E2H,LAB23  
        SETB 0C3H
        LJMP SCREEN6                          
LAB23:  NOP
        SETB 0C3H
        CLR 0C0H        ;CHECK FOR . DOT              
        NOP
        MOV A,P5
        JB 0E2H,LAB24  
        SETB 0C0H
        LJMP SCREEN1                          
LAB24:  NOP
        SETB 0C0H
        LJMP SCREEN4

;************************************************************************
SCREEN5:NOP            ;ENTER TIME
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB5:   NOP
        JNB T2OV,LAB5
        MOV TM2CON,#00H 
        CLR T2OV
        MOV P1,#80H    ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4EH    ;N
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#49H    ;I    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4DH    ;M    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3CH    ;<
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#48H    ;H    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4EH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4EH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4DH    ;M    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3EH    ;>    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
        ;SCAN KEYBOARD
        ;*************
        CLR 0C0H        ;CHECK FOR ENTER             
        NOP
        MOV A,P5
        JB 0E1H,LAB25A 
        SETB 0C0H
        LCALL SEND1
        NOP
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN1
LAB25A: NOP
        SETB 0C0H
        CLR 0C0H        ;CHECK FOR . DOT              
        NOP
        MOV A,P5
        JB 0E2H,LAB25  
        SETB 0C0H
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN4                          
LAB25:  NOP
        SETB 0C0H
        CLR 0C3H        ;CHECK FOR 1                     
        NOP
        MOV A,P5
        JB 0E4H,LAB28  
        SETB 0C3H
        MOV R2,#01H
        LJMP SHIFT1                           
LAB28:  NOP
        SETB 0C3H       ;CHECK FOR 2          
        CLR 0C3H
        NOP
        MOV A,P5
        JB 0E3H,LAB29  
        SETB 0C3H
        MOV R2,#02H
        LJMP SHIFT1                           
LAB29:  NOP
        SETB 0C3H
        CLR 0C3H        ;CHECK FOR 3                  
        NOP
        MOV A,P5
        JB 0E2H,LAB30  
        SETB 0C3H
        MOV R2,#03H
        LJMP SHIFT1                           
LAB30:  NOP
        SETB 0C3H
        CLR 0C2H        ;CHECK FOR 4                     
        NOP
        MOV A,P5
        JB 0E4H,LAB31  
        SETB 0C2H
        MOV R2,#04H
        LJMP SHIFT1                           
LAB31:  NOP
        SETB 0C2H       ;CHECK FOR 5          
        CLR 0C2H
        NOP
        MOV A,P5
        JB 0E3H,LAB32  
        SETB 0C2H
        MOV R2,#05H
        LJMP SHIFT1                           
LAB32:  NOP
        SETB 0C2H
        CLR 0C2H        ;CHECK FOR 6                  
        NOP
        MOV A,P5
        JB 0E2H,LAB33  
        SETB 0C2H
        MOV R2,#06H
        LJMP SHIFT1                           
LAB33:  NOP
        SETB 0C2H
        CLR 0C1H        ;CHECK FOR 7                     
        NOP
        MOV A,P5
        JB 0E4H,LAB34  
        SETB 0C1H
        MOV R2,#07H
        LJMP SHIFT1                           
LAB34:  NOP
        SETB 0C1H       ;CHECK FOR 8          
        CLR 0C1H
        NOP
        MOV A,P5
        JB 0E3H,LAB35  
        SETB 0C1H
        MOV R2,#08H
        LJMP SHIFT1                           
LAB35:  NOP
        SETB 0C1H
        CLR 0C1H        ;CHECK FOR 9                  
        NOP
        MOV A,P5
        JB 0E2H,LAB36  
        SETB 0C1H
        MOV R2,#09H
        LJMP SHIFT1                           
LAB36:  NOP
        SETB 0C1H
        CLR 0C0H        ;CHECK FOR 0                     
        NOP
        MOV A,P5
        JB 0E3H,LAB37  
        SETB 0C0H
        MOV R2,#00H                         
SHIFT1: NOP
        MOV A,4FH
        SWAP A
        MOV 4FH,A
        MOV A,4EH
        SWAP A
        MOV 4EH,A
        MOV R0,#4EH
        MOV A,R2
        XCHD A,@R0
        INC R0
        XCHD A,@R0
LAB37:  NOP
        SETB 0C0H
        LJMP SCREEN5
;************************************************************************
SCREEN6:NOP            ;ENTER FLOW RATE
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB6:   NOP
        JNB T2OV,LAB6
        MOV TM2CON,#00H 
        CLR T2OV
        MOV P1,#80H    ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4EH    ;N
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#46H    ;F     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4FH    ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#57H    ;W
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H   ;R      
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#41H    ;A    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3CH    ;<
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#2FH    ;/    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#53H    ;S    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3EH    ;>    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
        ;SCAN KEYBOARD
        ;*************
        CLR 0C0H        ;CHECK FOR ENTER             
        NOP
        MOV A,P5
        JB 0E1H,LAB26A 
        SETB 0C0H
        LCALL SEND2
        NOP
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN1
LAB26A: NOP
        SETB 0C0H
        CLR 0C0H        ;CHECK FOR . DOT              
        NOP
        MOV A,P5
        JB 0E2H,LAB26  
        SETB 0C0H
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN4                          
LAB26:  NOP
        SETB 0C0H
        CLR 0C3H        ;CHECK FOR 1                     
        NOP
        MOV A,P5
        JB 0E4H,LAB28A 
        SETB 0C3H
        MOV R2,#01H
        LJMP SHIFT2                           
LAB28A: NOP
        SETB 0C3H       ;CHECK FOR 2          
        CLR 0C3H
        NOP
        MOV A,P5
        JB 0E3H,LAB29A 
        SETB 0C3H
        MOV R2,#02H
        LJMP SHIFT2                           
LAB29A: NOP
        SETB 0C3H
        CLR 0C3H        ;CHECK FOR 3                  
        NOP
        MOV A,P5
        JB 0E2H,LAB30A 
        SETB 0C3H
        MOV R2,#03H
        LJMP SHIFT2                           
LAB30A: NOP
        SETB 0C3H
        CLR 0C2H        ;CHECK FOR 4                     
        NOP
        MOV A,P5
        JB 0E4H,LAB31A 
        SETB 0C2H
        MOV R2,#04H
        LJMP SHIFT2                           
LAB31A: NOP
        SETB 0C2H       ;CHECK FOR 5          
        CLR 0C2H
        NOP
        MOV A,P5
        JB 0E3H,LAB32A 
        SETB 0C2H
        MOV R2,#05H
        LJMP SHIFT2                           
LAB32A: NOP
        SETB 0C2H
        CLR 0C2H        ;CHECK FOR 6                  
        NOP
        MOV A,P5
        JB 0E2H,LAB33A 
        SETB 0C2H
        MOV R2,#06H
        LJMP SHIFT2                           
LAB33A: NOP
        SETB 0C2H
        CLR 0C1H        ;CHECK FOR 7                     
        NOP
        MOV A,P5
        JB 0E4H,LAB34A 
        SETB 0C1H
        MOV R2,#07H
        LJMP SHIFT2                           
LAB34A: NOP
        SETB 0C1H       ;CHECK FOR 8          
        CLR 0C1H
        NOP
        MOV A,P5
        JB 0E3H,LAB35A 
        SETB 0C1H
        MOV R2,#08H
        LJMP SHIFT2                           
LAB35A: NOP
        SETB 0C1H
        CLR 0C1H        ;CHECK FOR 9                  
        NOP
        MOV A,P5
        JB 0E2H,LAB36A 
        SETB 0C1H
        MOV R2,#09H
        LJMP SHIFT2                           
LAB36A: NOP
        SETB 0C1H
        CLR 0C0H        ;CHECK FOR 0                     
        NOP
        MOV A,P5
        JB 0E3H,LAB37A 
        SETB 0C0H
        MOV R2,#00H                         
SHIFT2: NOP
        MOV A,4FH
        SWAP A
        MOV 4FH,A
        MOV R0,#4FH
        MOV A,R2
        XCHD A,@R0
LAB37A: NOP
        SETB 0C0H
        LJMP SCREEN6
;************************************************************************
SCREEN7:NOP            ;ENTER LEVEL    
        LCALL WDOG
        NOP
        MOV TM2CON,#89H  ;.25SEC DELAY
LAB7:   NOP
        JNB T2OV,LAB7
        MOV TM2CON,#00H 
        CLR T2OV
        MOV P1,#80H    ;TOP ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4EH    ;N
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#54H    ;T
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#52H    ;R
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#56H    ;V    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#45H    ;E
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#4CH    ;L    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH  ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
;*******************************************************************       
        MOV P1,#0C0H   ;BOTTOM ROW LEFT HAND DIGIT
        SETB 0C7H      ;ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#28H    ;(     
        SETB 0C5H      ;SET R/S HIGH
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#35H    ;5    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#32H    ;2    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#2DH    ;-    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#39H    ;9
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#39H    ;9    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#29H    ;)    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3CH    ;<    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O  
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV A,4FH
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV P1,A       ;O    
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#3EH    ;>     
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        LCALL DELAY
        NOP
        MOV P1,#0FEH   ;SPACE
        SETB 0C7H      ;SET ENABLE HIGH
        LCALL DELAY
        NOP
        CLR 0C7H       ;ENABLE LOW
        CLR 0C5H       ;R/S LOW
        LCALL DELAY
        NOP
        ;SCAN KEYBOARD
        ;*************
        CLR 0C0H        ;CHECK FOR ENTER             
        NOP
        MOV A,P5
        JB 0E1H,LAB27A 
        SETB 0C0H
        LCALL SEND3
        NOP
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN1
LAB27A: NOP
        SETB 0C0H
        CLR 0C0H        ;CHECK FOR . DOT              
        NOP
        MOV A,P5
        JB 0E2H,LAB27  
        SETB 0C0H
        MOV 4FH,#00H
        MOV 4EH,#00H
        LJMP SCREEN4                          
LAB27:  NOP
        SETB 0C0H
        CLR 0C3H        ;CHECK FOR 1                     
        NOP
        MOV A,P5
        JB 0E4H,LAB28B 
        SETB 0C3H
        MOV R2,#01H
        LJMP SHIFT3                           
LAB28B: NOP
        SETB 0C3H       ;CHECK FOR 2          
        CLR 0C3H
        NOP
        MOV A,P5
        JB 0E3H,LAB29B 
        SETB 0C3H
        MOV R2,#02H
        LJMP SHIFT3                           
LAB29B: NOP
        SETB 0C3H
        CLR 0C3H        ;CHECK FOR 3                  
        NOP
        MOV A,P5
        JB 0E2H,LAB30B 
        SETB 0C3H
        MOV R2,#03H
        LJMP SHIFT3                           
LAB30B: NOP
        SETB 0C3H
        CLR 0C2H        ;CHECK FOR 4                     
        NOP
        MOV A,P5
        JB 0E4H,LAB31B 
        SETB 0C2H
        MOV R2,#04H
        LJMP SHIFT3                           
LAB31B: NOP
        SETB 0C2H       ;CHECK FOR 5          
        CLR 0C2H
        NOP
        MOV A,P5
        JB 0E3H,LAB32B 
        SETB 0C2H
        MOV R2,#05H
        LJMP SHIFT3                           
LAB32B: NOP
        SETB 0C2H
        CLR 0C2H        ;CHECK FOR 6                  
        NOP
        MOV A,P5
        JB 0E2H,LAB33B 
        SETB 0C2H
        MOV R2,#06H
        LJMP SHIFT3                           
LAB33B: NOP
        SETB 0C2H
        CLR 0C1H        ;CHECK FOR 7                     
        NOP
        MOV A,P5
        JB 0E4H,LAB34B 
        SETB 0C1H
        MOV R2,#07H
        LJMP SHIFT3                           
LAB34B: NOP
        SETB 0C1H       ;CHECK FOR 8          
        CLR 0C1H
        NOP
        MOV A,P5
        JB 0E3H,LAB35B 
        SETB 0C1H
        MOV R2,#08H
        LJMP SHIFT3                           
LAB35B: NOP
        SETB 0C1H
        CLR 0C1H        ;CHECK FOR 9                  
        NOP
        MOV A,P5
        JB 0E2H,LAB36B 
        SETB 0C1H
        MOV R2,#09H
        LJMP SHIFT3                           
LAB36B: NOP
        SETB 0C1H
        CLR 0C0H        ;CHECK FOR 0                     
        NOP
        MOV A,P5
        JB 0E3H,LAB37B 
        SETB 0C0H
        MOV R2,#00H                         
SHIFT3: NOP
        MOV A,4FH
        SWAP A
        MOV 4FH,A
        MOV R0,#4FH
        MOV A,R2
        XCHD A,@R0
LAB37B: NOP
        SETB 0C0H
        LJMP SCREEN7


DELAY:  MOV R7,#80H
DELAY13:NOP
        DJNZ R7,DELAY13
        RET

SEND1:  NOP              ;TRANSMIT NEW TIME
        CLR ES           ;DISABLE SERIAL INTERRUPT
        MOV SBUF,#55H    ;U
WAIT1:  NOP
        JNB TI,WAIT1
        CLR TI
        MOV SBUF,#DIG1   ;3    
WAIT2:  NOP
        JNB TI,WAIT2
        CLR TI
        MOV SBUF,#DIG2   ;4
WAIT3:  NOP
        JNB TI,WAIT3
        CLR TI
        MOV SBUF,#DIG3   ;0
WAIT4:  NOP
        JNB TI,WAIT4
        CLR TI
        MOV SBUF,#DIG4   ;0
WAIT5:  NOP
        JNB TI,WAIT5
        CLR TI
        MOV SBUF,#DIG5   ;1
WAIT6:  NOP
        JNB TI,WAIT6
        CLR TI
        MOV SBUF,#54H    ;T
WAIT7:  NOP
        JNB TI,WAIT7
        CLR TI
        MOV SBUF,#2CH    ;,
WAIT8:  NOP
        JNB TI,WAIT8
        CLR TI
        MOV R0,#4FH
        MOV A,@R0  
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT9:  NOP
        JNB TI,WAIT9
        CLR TI
        MOV R0,#4FH
        MOV A,@R0  
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT10: NOP
        JNB TI,WAIT10
        CLR TI
        MOV R0,#4EH
        MOV A,@R0  
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT11: NOP
        JNB TI,WAIT11
        CLR TI
        MOV R0,#4EH
        MOV A,@R0  
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT12: NOP
        JNB TI,WAIT12
        CLR TI
        MOV SBUF,#7EH       ;~
WAIT13: NOP
        JNB TI,WAIT13
        CLR TI
  ;**********************************      
        MOV SBUF,#54H       ;T
WAIT14: NOP
        JNB TI,WAIT14
        CLR TI
        MOV SBUF,#49H       ;I
WAIT15: NOP
        JNB TI,WAIT15
        CLR TI
        MOV SBUF,#4DH       ;M
WAIT16: NOP
        JNB TI,WAIT16
        CLR TI
        MOV SBUF,#45H       ;E
WAIT17: NOP
        JNB TI,WAIT17
        CLR TI
        SETB ES
        RET
SEND2:  NOP              ;TRANSMIT NEW FLOW RATE
        CLR ES           ;DISABLE SERIAL INTERRUPT
        MOV SBUF,#55H    ;U
WAIT1A: NOP
        JNB TI,WAIT1A
        CLR TI
        MOV SBUF,#DIG1       
WAIT2A: NOP
        JNB TI,WAIT2A
        CLR TI
        MOV SBUF,#DIG2
WAIT3A: NOP
        JNB TI,WAIT3A
        CLR TI
        MOV SBUF,#DIG3
WAIT4A: NOP
        JNB TI,WAIT4A
        CLR TI
        MOV SBUF,#DIG4
WAIT5A: NOP
        JNB TI,WAIT5A
        CLR TI
        MOV SBUF,#DIG5
WAIT6A: NOP
        JNB TI,WAIT6A
        CLR TI
        MOV SBUF,#46H       ;F
WAIT7A: NOP
        JNB TI,WAIT7A
        CLR TI
        MOV SBUF,#2CH       ;,
WAIT8A: NOP
        JNB TI,WAIT8A
        CLR TI
        MOV R0,#4FH
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT9A: NOP
        JNB TI,WAIT9A
        CLR TI
        MOV R0,#4FH
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT10A:NOP
        JNB TI,WAIT10A
        CLR TI
        MOV SBUF,#7EH       ;~
WAIT13A:NOP
        JNB TI,WAIT13A
        CLR TI
  ;**********************************      
        MOV SBUF,#54H       ;T
WAIT14A:NOP
        JNB TI,WAIT14A
        CLR TI
        MOV SBUF,#49H       ;I
WAIT15A:NOP
        JNB TI,WAIT15A
        CLR TI
        MOV SBUF,#4DH       ;M
WAIT16A:NOP
        JNB TI,WAIT16A
        CLR TI
        MOV SBUF,#45H       ;E
WAIT17A:NOP
        JNB TI,WAIT17A
        SETB ES
        RET
SEND3:  NOP              ;TRANSMIT NEW LEVEL
        CLR ES           ;DISABLE SERIAL INTERRUPT
        MOV SBUF,#55H    ;U
WAIT1B: NOP
        JNB TI,WAIT1B
        CLR TI
        MOV SBUF,#DIG1       
WAIT2B: NOP
        JNB TI,WAIT2B
        CLR TI
        MOV SBUF,#DIG2
WAIT3B: NOP
        JNB TI,WAIT3B
        CLR TI
        MOV SBUF,#DIG3
WAIT4B: NOP
        JNB TI,WAIT4B
        CLR TI
        MOV SBUF,#DIG4
WAIT5B: NOP
        JNB TI,WAIT5B
        CLR TI
        MOV SBUF,#DIG5
WAIT6B: NOP
        JNB TI,WAIT6B
        CLR TI
        MOV SBUF,#4CH       ;L
WAIT7B: NOP
        JNB TI,WAIT7B
        CLR TI
        MOV SBUF,#2CH       ;,
WAIT8B: NOP
        JNB TI,WAIT8B
        CLR TI
        MOV R0,#4FH
        MOV A,@R0
        SWAP A
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT9B: NOP
        JNB TI,WAIT9B
        CLR TI
        MOV R0,#4FH
        MOV A,@R0
        CLR 0E7H
        CLR 0E6H
        SETB 0E5H
        SETB 0E4H
        MOV SBUF,A
WAIT10B:NOP
        JNB TI,WAIT10B 
        CLR TI
        MOV SBUF,#7EH       ;~
WAIT13B:NOP
        JNB TI,WAIT13B
        CLR TI
  ;**********************************      
        MOV SBUF,#54H       ;T
WAIT14B:NOP
        JNB TI,WAIT14B
        CLR TI
        MOV SBUF,#49H       ;I
WAIT15B:NOP
        JNB TI,WAIT15B
        CLR TI
        MOV SBUF,#4DH       ;M
WAIT16B:NOP
        JNB TI,WAIT16B
        CLR TI
        MOV SBUF,#45H       ;E
WAIT17B:NOP
        JNB TI,WAIT17B
        SETB ES
        RET
SEND4:  NOP              ;TRANSMIT MOTOR ON
        CLR ES           ;DISABLE SERIAL INTERRUPT
        MOV SBUF,#55H    ;U
WAIT1C: NOP
        JNB TI,WAIT1C
        CLR TI
        MOV SBUF,#DIG1       
WAIT2C: NOP
        JNB TI,WAIT2C
        CLR TI
        MOV SBUF,#DIG2
WAIT3C: NOP
        JNB TI,WAIT3C
        CLR TI
        MOV SBUF,#DIG3
WAIT4C: NOP
        JNB TI,WAIT4C
        CLR TI
        MOV SBUF,#DIG4
WAIT5C: NOP
        JNB TI,WAIT5C
        CLR TI
        MOV SBUF,#DIG5
WAIT6C: NOP
        JNB TI,WAIT6C
        CLR TI
        MOV SBUF,#6DH       ;m
WAIT7C: NOP
        JNB TI,WAIT7C
        CLR TI
        MOV SBUF,#2CH       ;,
WAIT8C: NOP
        JNB TI,WAIT8C
        CLR TI
        MOV SBUF,#4FH       ;O
WAIT9C: NOP
        JNB TI,WAIT9C
        CLR TI
        MOV SBUF,#4EH       ;N
WAIT11C:NOP
        JNB TI,WAIT11C
        CLR TI
        MOV SBUF,#7EH       ;~
WAIT13C:NOP
        JNB TI,WAIT13C
        CLR TI
        SETB ES
        RET
SEND5:  NOP              ;TRANSMIT MOTOR OFF
        CLR ES           ;DISABLE SERIAL INTERRUPT
        MOV SBUF,#55H    ;U
WAIT1D: NOP
        JNB TI,WAIT1D
        CLR TI
        MOV SBUF,#DIG1       
WAIT2D: NOP
        JNB TI,WAIT2D
        CLR TI
        MOV SBUF,#DIG2
WAIT3D: NOP
        JNB TI,WAIT3D
        CLR TI
        MOV SBUF,#DIG3
WAIT4D: NOP
        JNB TI,WAIT4D
        CLR TI
        MOV SBUF,#DIG4
WAIT5D: NOP
        JNB TI,WAIT5D
        CLR TI
        MOV SBUF,#DIG5
WAIT6D: NOP
        JNB TI,WAIT6D
        CLR TI
        MOV SBUF,#6DH       ;m
WAIT7D: NOP
        JNB TI,WAIT7D
        CLR TI
        MOV SBUF,#2CH       ;,
WAIT8D: NOP
        JNB TI,WAIT8D
        CLR TI
        MOV SBUF,#4FH       ;O
WAIT9D: NOP
        JNB TI,WAIT9D
        CLR TI
        MOV SBUF,#46H       ;F
WAIT10D:NOP
        JNB TI,WAIT10D
        CLR TI
        MOV SBUF,#46H       ;F
WAIT11D:NOP
        JNB TI,WAIT11D
        CLR TI
        MOV SBUF,#7EH       ;~
WAIT13D:NOP
        JNB TI,WAIT13D
        CLR TI
        SETB ES
        RET
        
RECEIVE:NOP
        PUSH ACC
        PUSH PSW
        MOV A,SBUF
        CJNE A,#3CH,LAB101   ;CHECK FOR ' < ' START OF TRANSMISSION
        MOV R1,#90H
        LJMP LAB110
LAB101: NOP
        CJNE A,#3EH,LAB102   ;CHECK FOR END OF TRANSMISSION ' > '
        LJMP LOAD            ;GOTO LOAD DATA
LAB102: NOP
        CJNE R1,#0A9H,LAB102A ;CHECK FOR COMMS BUFFER FULL
        MOV R1,#90H
        LJMP LAB110
LAB102A:NOP
        MOV @R1,A
        INC R1
        LJMP LAB110
LOAD:   NOP
        MOV R1,#90H     ;90
        MOV 67H,@R1
        INC R1          ;91
        MOV 66H,@R1
        INC R1          ;92
        MOV 71H,@R1
        INC R1          ;93
        MOV 70H,@R1
        INC R1          ;94
        MOV 73H,@R1
        INC R1          ;95
        MOV 72H,@R1
        INC R1          ;96
        MOV 75H,@R1
        INC R1          ;97
        MOV 74H,@R1
        INC R1          ;98
        MOV 77H,@R1
        INC R1          ;99
        MOV 76H,@R1
        INC R1          ;9A
        MOV 79H,@R1
        INC R1          ;9B
        MOV 78H,@R1
        INC R1          ;9C
        MOV 7BH,@R1
        INC R1          ;9D
        MOV 7AH,@R1
        INC R1          ;9E
        MOV 7DH,@R1
        INC R1          ;9F
        MOV 7CH,@R1
        INC R1          ;A0
        MOV 7FH,@R1
        INC R1          ;A1
        MOV 7EH,@R1
        INC R1          ;A2
        MOV 26H,@R1
        INC R1          ;A3
        MOV 64H,@R1
        INC R1          ;A4
        MOV 63H,@R1
        INC R1          ;A5
        MOV 62H,@R1
        INC R1          ;A6
        MOV 61H,@R1
        INC R1          ;A7
        MOV 60H,@R1
LAB110: NOP
        POP PSW
        POP ACC
        CLR RI
        RETI
WDOG:   NOP
        ORL PCON,#10H
        MOV T3,#56       ;400mSEC TIME OUT
        RET

END

