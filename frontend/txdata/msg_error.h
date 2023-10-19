#define Rx_char_wait 1
#define Rx_char_wait_from_tnc 1
#define Retry_errors  80
#define DRIVE 1

char *error_strings[] = {

  "Could not open file ",           /* error no 0 **/
  "Mode definition in file ",       /* error no 1 **/
  "IPCS message creation error ",   /* error no 2 **/
  "Could not open device - ",       /* error no 3 **/
  "Controle device error - ",       /* error no 4 **/
  "Message send error no - ",       /* error no 5 **/
  "Remote computer COMS ERROR",     /* error no 6 **/
  "Remote computer TIMEOUT ERROR\n" , /* error no 7 **/
  "Could not open GPIB",            /* error no 8 **/
  "Could not find GPIB board\n",    /* error no 9 **/
  "GPIB Voltage error with ",       /* error no 10 **/
  "Remote computer NOT RESPONDING", /* error no 11 **/
  "",               /* error no 12 **/
  "",               /* error no 13 **/
  "",               /* error no 14 **/
  "",               /* error no 15 **/
  "",               /* error no 16 **/
  "",               /* error no 17 **/
  "",               /* error no 18 **/
  "",               /* error no 19 **/
  "",               /* error no 20 **/
  ""                /** leave Blank !!!  **/
};
