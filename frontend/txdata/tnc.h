#define BREAK 3
/** This is the TNC init. strings **/
char *init_string[] = 
{

   "kbg999",       /**                                            **/
   "AB 9600",      /**                                            **/
   "TX 200",       /** introduce 90ms delay befor TX starts       **/
   "CH 1",         /** time out occurs after 10 seconds           **/
   "NO OFF",       /** conect in Convers or Transparent Mode      **/
   "RET 3",        /** resend packet 15 times                     **/
   "CONM T",       /** resend packet 15 times                     **/
   "RI OFF",       /**                                            **/
   "E OFF",        /**                                            **/
   "PERM",         /**                                            **/
   "LF ON",        /**                                            **/
   ""              /**                                            **/
};
int Init_messages = 11; /** number of init_string's   **/

char breakcoms[]      = "\3";
char c_r[]            = "\n";
char crlfadd[]        = "CRL ON";
char connect[]        = "UUUUUU";
char conv[]           = "k";
char disconnect[]     = "D\r";
char datum[]     = "93:1:12:00:00 C";
char datadump[]       = "93:1:12:00:00 C";
char end_line[]    = "\r";
char star8[]      = "*8";
char star6[]      = "*6";
char remotekeyboard[]      = "7H";
char down_load[]      = "F.S. 1";
char dump[]           = "10D";
char init_modems[]    = "kb1abc";
char echo_on[]        = "E ON";
char no_echo[]        = "E OFF\r";
char no_conv[]        = "NO ON";
char o_k[]            = "*";
char r_r[]            = "\r";
char retries[]        = "RET 3\r";     /** resend packet 15 times **/
char reset[]          = "res";     /** RESet the TNC **/
char restore[]        = "restore\r";     /** resend packet 15 times **/
char transparent[]    = "T";
char junk[] = "Hello oom gert hoe gaan dit?\r";
