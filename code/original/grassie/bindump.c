
#include <stdio.h>

#include "serial.h"

int select_port;

void sendRS232(unsigned char Char)
	{
	int i ,j =1;
	char error;


	for( j = 0; j < 1200; j++)
	  while ((inportb(select_port+5) & 0x20)==0);
	outportb(select_port, Char);
	}

/*  ************* this routine initializes both serial ports  ************* */

int initRS232(int port,int speed,int bits,char parity,int stops)
{

	unsigned char i;
	unsigned char control;
	int p=0;
	char parity_tab[5];

	strcpy(parity_tab,"NO0E");
/*					 port number initialization */
	switch(port)
	{
		case 1 :select_port=COM1BASE;
			break;
		case 2 :select_port=COM2BASE;
			break;
	}
	outportb(select_port+3,(unsigned char)0x80);

/**										speed initialization  **/

	speed = 115200 / speed;
	outportb(select_port+1,(unsigned char)(speed>>8));
	outportb(select_port,(unsigned char)speed);
	parity=toupper(parity);
	while(parity_tab[p++]!=parity);
	control=(p-1)<<3;
	switch(stops)
	{
		case 1: break;
		case 2: control|=0x04;
				break;
		default:
			return(-1);
	}
	switch(bits)
	{
		case 7: control|=0x02;
			break;
		case 8: control|=0x03;
			break;
		default:
			return(-1);
	};

/**  initialization for pooling interrupts **/

	outportb(select_port+3,control);
/*	outportb(select_port+4,(unsigned char)CTRL_D);
	outportb(select_port+1,(unsigned char)ZERO);
	setport();
*/
	return(0);
}


main(int argc, char *argv[])
 {
  FILE *fp;
  unsigned char TC;
  char filename[128];
  int Loop=0;



  system ( "del *.hex");
  system ( "del *.bin");
  system ( "oh51 nbore");
  system ( "HEXobj nbore.HEX nbore.BIN");


  system ( "mode com2:96,n,8,1,p");
  Loop = atoi ( argv[1]);
  if(Loop == 1)
    {
    if( initRS232(1, 9600,8 ,'N',1) != 0)
      {
      printf( "Comms error !!");
      exit();
      }
    } else   if( Loop == 2)
	{
	if( initRS232(2, 9600,8 ,'N',1) != 0)
	 {
	 printf( "Comms error !!");
	 exit();
	 }
	}else
	  {
	  printf ("Comms port not spesified");
	  exit(1);
	  }
  fp = fopen( argv[2],"rb");
  if ( fp ==0)
    {
    printf( "Could not find %s\n", argv[2]);
    exit(1);
    }
  while ( feof( fp) == 0)
    {
    TC = fgetc( fp);
    sendRS232( TC);
    if( Loop++ == 100)
      {
      Loop=0;
      printf(".");
      }
    }
  fclose( fp);
  printf( "\nDone!!\n");


 }