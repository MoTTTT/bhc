/*
 00 =  0 close window
 12 =  1 select pin code
 02 =  2 alchol test
 22 =  3 random test
 10 =  4 test failed
 30 =  5 car out of action
 12 =  6 slect lowest speed
 32 =	 7 beware exceeding your own speed selection
 01 =	 8 dangerous speed controleaction
 21 =	 9 dangerous driving behave your selve
 33 =  no speach
*/
#define CLOSE_WINDOW           0X00
#define CONTROLE_ACTION        0X01
#define ALC_TEST_SPEACH        0X02
#define TEST_FAILED            0X10
#define SELECT_LOW_SPEED       0X12
#define PIN_CODE_SPEACH        0X20
#define DANGEROUS_DRIVING      0X21
#define RANDOM_TEST            0X22
#define CAR_OUT_OF_ACTION      0X30
#define BEWARE_EXCEEDING_SPEED 0X32
#define NO_SPEACH              0XFF
