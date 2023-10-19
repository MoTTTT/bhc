#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "c:\include\serial\serial.h"
#include "c:\include\serial\char_def.h"
#include "c:\include\serial\packets.h"
#include "c:\include\c_incl\TYPE_DEF.h"
#include "c:\include\handheld\handheld.h"

extern char connect[];
extern int MaxY, MaxX, DelayTimeGI;
void sendRS232_CHAR(Byte karakter);
extern int PellaWaardes;
#define HOSPITAL
#define IRECT
#define ECTRON
#ifndef VECTRON
extern int select_port;
#else
	int select_port;
#endif

extern char BOX_NO[];
break_comms()
	{
	sendRS232_CHAR( 3);
	sendRS232_CHAR( 3);
	sendRS232_CHAR( 3);
	sleep(2);
	sendRS232_CHAR( 'D');
	wait_char (':');
	sendRS232_CHAR( '\r');
	wait_char (':');
	}
/* ************************ receiving on serial port *********************/

Byte recRS232( char *rx_error, int type)
{

	long no_rx_loop = 1470000; /*970000 RXCHAR_CHECKS;*/
	char return_char=0;

	if ( type == 1)
#ifdef DIRECT
		no_rx_loop = 145000; /*970000 RXCHAR_CHECKS;*/
#else
		no_rx_loop = 200000; /*970000 RXCHAR_CHECKS;*/
#endif
	else if ( type < 0)
		no_rx_loop = 8000;
	else if ( type ==0)
		 no_rx_loop = 10900;
	while ( ( no_rx_loop-- !=0) && ( return_char == 0))
			return_char = inportb(select_port+LSR) & RCVRDY;
	if ( no_rx_loop <= 00)
		*rx_error = 1;
	else
	{
		*rx_error =0;
		return_char = inportb(select_port);
/*		printf( "%c", return_char, return_char );*/
	}
	return(return_char);
}

clear_port()
{
	outportb(select_port+MCR ,0);
}

/* ************************ transmit on serial port *********************/
void sendRS232(int length_buffer ,Byte *buffer)
{
	int i ,j =1;
	char error, buf=0;


/*
	buf = RTS|DTR;
	outportb(select_port+MCR, buf);
*/
	for(i=0;i!=length_buffer;i++)
	{
		for( j = 0; j < 50; j++)
			while ((inportb(select_port+5) & 0x20)==0);
		outportb(select_port,*buffer++);
	}
	buf=0;
	outportb(select_port+MCR, buf);
}

/***************************  CRC calculation  *****************************/
unsigned int crc_calc(int length_buffer ,Byte *buffer_pointer)
{

	Byte temp_xor_high = 0 ,temp_xor_low = 0;
	unsigned int i,crc_calc;

	i=0;
	do
	{
		temp_xor_high ^= buffer_pointer[i++];
		temp_xor_low  ^= buffer_pointer[i++];
	} while (i<length_buffer-2);
	crc_calc = (temp_xor_high<<8)+temp_xor_low;
	return (crc_calc);
}

/*  ************* this routine initializes both serial ports  ************* */

int initRS232(int port,int speed,int bits,char parity,int stops)
{

	Byte i;
	Byte control;
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
	outportb(select_port+3,(Byte)0x80);

/**										speed initialization  **/

	speed = 115200 / speed;
	outportb(select_port+1,(Byte)(speed>>8));
	outportb(select_port,(Byte)speed);
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
	outportb(select_port+4,(Byte)CTRL_D);
	outportb(select_port+1,(Byte)ZERO);
	setport();
	return(0);
}

send_ok()
	{
	char *str_ok= "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUOK&\r";
	int length, loops;
		sendRS232( strlen ( str_ok), str_ok);
}
send_ack()
	{
	char *str_ok= "UUUUUUUUUUUUUUUUUOack&\r";
	int length, loops;
		sendRS232( strlen ( str_ok), str_ok);
}

/**
	= ************************************************************** =
	=								                                 								 =
	=  This is the routine whitch transmits the packets              =
	=    															 															 =
	= ************************************************************** =
**/

tx_packets( Byte *address,long grote, Byte *tx_buffer)
{

	Byte *p_tx_buf;
	char header[] = "HeAdEr";
	int  packet_nr =0, Size, buf_len, counter;
	int  crc, I_p;
	float packets;

	Size = PACKET_SIZE;
	packets = CRC_SIZE + SYNCH_SIZE + BOX_NUMBER_SIZE + NUMBER_OF_PACKETS_SIZE;
	buf_len = Size - packets;
	packets = PACKET_SIZE-packets;
	if( packets ==0)
		packets=1;
	packets = (float) grote / packets;
	I_p = (int) packets;

	counter =0;
	memset ( tx_buffer, 0, Size);
	p_tx_buf = tx_buffer;
	strcpy ( p_tx_buf, SYNCH);
	p_tx_buf += strlen ( SYNCH);
	strncat ( p_tx_buf, BOX_NO ,8);
	p_tx_buf += strlen ( BOX_NO);
	*p_tx_buf++ = I_p >> 8;				/** send amount of packets **/
	*p_tx_buf++ = I_p;
	counter += strlen ( SYNCH);
	counter += strlen ( BOX_NO);
	counter += strlen ( header);
	counter ++; 		/** increment for type of transmition **/
	counter +=2;		/** add 2 for CRC calc **/
	counter +=2;		/** add 2 for packetsize **/
	counter +=2;		/** add 2 for data size **/

	*p_tx_buf = counter>>8;	/** send amount of packets **/
	p_tx_buf++;
	*p_tx_buf = counter;
	p_tx_buf++;
	*p_tx_buf++ = 'H';			/** define the type of transmition **/
	strncpy ( p_tx_buf, header, strlen ( header) +1);
	p_tx_buf +=  strlen ( header);
	*p_tx_buf++ =0;
	*p_tx_buf =0;
	*p_tx_buf--;
	crc = crc_calc ( counter, tx_buffer);
	*p_tx_buf++ = crc >>8;
	*p_tx_buf   = crc;
	counter =0;
	do
	{
		memset ( tx_buffer, 0, Size);
		p_tx_buf = tx_buffer;
		strcpy ( p_tx_buf, SYNCH);
		p_tx_buf += strlen ( SYNCH);
		strncat ( p_tx_buf, BOX_NO ,8);
		p_tx_buf += strlen ( BOX_NO);
		*p_tx_buf = packet_nr>>8;
		p_tx_buf++;
		*p_tx_buf = packet_nr;
		p_tx_buf++;
		*p_tx_buf = Size>>8;
		p_tx_buf++;
		*p_tx_buf = Size;
		p_tx_buf++;
		counter =0;
		while ( counter++ < buf_len)
			*p_tx_buf++ = *address++;
		*p_tx_buf++  = 0;
		*p_tx_buf = 0;
		p_tx_buf--;
		crc = crc_calc( Size, tx_buffer);
		*p_tx_buf++ = crc >>8;
		*p_tx_buf   = crc;
		do
		{
			sendRS232( PACKET_SIZE, tx_buffer);
			gotoxy(1,2);
			printf ("TX packet number % .4d of %4.0f\n" ,packet_nr+1, packets);
		}while ( !rx_ok());
		packet_nr++;
	} while ( packet_nr < packets);
}

int rx_packet( char *rx_buffer)
{
	int rx_buf_counter, counter;
	char rx_error;

	recRS232( &rx_error, -1);
	memset( rx_buffer, 0, sizeof( rx_buffer));
	do
	{
		rx_buf_counter=0;
		do
		{
			rx_buffer[rx_buf_counter] = recRS232( &rx_error,1);
			if ( !rx_error)
				{
/*				putchar( rx_buffer[rx_buf_counter]);*/
				if(rx_buffer[ rx_buf_counter++] == '&')
					rx_buf_counter = 10000;
				} else
					{
						rx_buf_counter = 0;
						memset ( rx_buffer,(char )0, 10);
						return(1);
					}
		}while(rx_buf_counter < 9995);
	}while(rx_buf_counter == 0);
	return(0);
}
/**
	= ************************************************************** =
	=								                                 								 =
	=  This is the routine whitch receives the packets               =
	=    															 															 =
	= ************************************************************** =
**/

int rx_packets(Byte *rx_buf )
{

	int rx_buf_counter;
	static packets;

	char rx_error;
	Int crc ,tmpcrc;
	Int counter ,tI;

	rx_buf_counter = 0;
	do
	{
		memset ( rx_buf,(char )0, 10);
		rx_buf_counter = 0;
		rx_packet( rx_buf);
		packets = (rx_buf[rx_buf_counter-4] *256);
		packets += rx_buf[rx_buf_counter-3];
		counter = (rx_buf[rx_buf_counter-2] *256);
    counter += rx_buf[rx_buf_counter-1];
    while(rx_buf_counter != counter)
			rx_buf[rx_buf_counter++] =  recRS232( &rx_error,0);
		tmpcrc = rx_buf[rx_buf_counter-2] << 8;
		tmpcrc |= rx_buf[rx_buf_counter-1];
		 rx_buf[rx_buf_counter-2] =  0;
		 rx_buf[rx_buf_counter-1] =  0;
		crc = crc_calc( counter, rx_buf);
	}while(  crc != tmpcrc);
	return(0);
}

/* ************************ transmit on serial port *********************/

void sendRS232_CHAR(Byte karakter)
{
	int j;
	unsigned char tmp;

/*
	do
	{
		tmp = inportb( select_port+MSR);
		tmp &= CTS;
	}while ( tmp == 0);
*/
	for( j = 0; j < 800; j++)
		while ((inportb(select_port+5) & 0x20)==0);
	outportb(select_port, karakter);
}

int wait_char( char kar)
{
	char tmp=0, error;
	int loops=0;

	do
	{
		tmp = recRS232( &error,1);
		loops++;
		if( error  != 0)
			loops = 90;
	}	while (tmp != kar && loops < 80);
	if( loops <80)
		return(0);
	else
		return(1);
}
/**
		this sub routine reads the Modem Status Register
		and mask off all the unwanted bits
**/
char check_wait_ri()
{
	return ( inportb( select_port+MSR) & RI);
}
/* ************************ Wait for the RO indicator *********************/
void wait_ri()
{
	char tmp=0;
	while (! tmp)
		tmp = check_wait_ri();
}
/**
	 This function sets the Paknet modem to the properstate for
	 coms etc.
**/

setport()
{
	char buf=0;

	buf = RTS|DTR;
	outportb(select_port+MCR, buf);
}
setportnorx()
{
	char buf=0x13;

	sendRS232_CHAR ( buf);
}
setportrx()
{
	char buf=0x11;

	sendRS232_CHAR ( buf);
}

int rx_ok ( )
{

	char rx_buffer[255], rx_error, tmp_char;
	int rx_buf_counter=0, rx_loops=0;

	memset ( rx_buffer, 0 ,sizeof ( rx_buffer));
	do
	{
		if(! wait_char('U'))
			rx_loops=400;
	}while (	rx_loops++ < 300);
	if( rx_loops < 300)
		return(1);
	do
	{
		tmp_char = recRS232( &rx_error,0);
		if ( rx_error == 0)
		{
			rx_buffer[ rx_buf_counter++] = tmp_char;
			rx_loops++;
		}
		if( rx_loops++ > 1000)
			return(1);
	}while(rx_buf_counter < 4);
	rx_loops=0;
	do
	{
		tmp_char = recRS232( &rx_error,0);
		if( rx_loops++ > 1000)
			return(1);
	} while(rx_error);
	rx_buffer[rx_buf_counter++] = tmp_char;
	rx_loops=0;
	do
	{
		tmp_char = recRS232( &rx_error,0);
		if( rx_loops++ > 1000)
			return(1);
	} while(rx_error);
	rx_buffer[rx_buf_counter++] = tmp_char;
	if  (strstr ( rx_buffer, "UO"))
		return(0);
	else
		return(1);
}


int call_station( int StationI)
	{
	char kar=0, String[15];
	int tries=0,TriesI=1;

	switch ( StationI)
		{
		case  0 :
			strcpy ( String, "c wcr001\r");
		break;
		case 1 :
			strcpy ( String, "c wcr002\r");
		break;
		case 2 :
			strcpy ( String, "c wcr003\r");
		break;
			}
	do
		{
		sendRS232(  strlen( String), String);
		delay(DelayTimeGI);
		kar = wait_char( '@');
		if( kar ==1 )
			{
			if( tries++ >TriesI)
			 {
			 setbkcolor( LIGHTMAGENTA);
			 return(1);
			 }
			sendRS232_CHAR( '\r');
			if( wait_char (':') ==1)
				{
				break_comms();
				sendRS232_CHAR( '\r');
				wait_char (':');
				}
			}
		} while ( kar == 1);
	}

wait_colon()
	{
	sendRS232_CHAR( '\r');
	wait_char (':');
	}

int get_data(rx_buffer, DataFieldsI, Waardes )
char *rx_buffer;
int DataFieldsI, Waardes[];
	{

	char *pchar;
	int LOOPS,	loops=0, RetriesI=0;
	char  kar, temp_buf[248];
	int errorCounts=0,err=0, counts, counter, RetryI=3, StationI=2;

 do
	 {
	 if( call_station( StationI) == 0)
	 {
	 memset( rx_buffer, 0 , sizeof( rx_buffer));
	 delay(600);
	 do
		{
		 pchar = rx_buffer;
		 send_ok();
		 loops=0;
		 delay(DelayTimeGI);
		 if ( rx_packet( rx_buffer) ==0)
			 {
			 do
				 {
				 if ( *pchar == ' ')
					 loops++;
				 } while ( *pchar++ != '&');
			 if( loops!= DataFieldsI && *--pchar != '&')
				 {
				 err++;
				 setbkcolor(LIGHTRED);
				 memset ( rx_buffer, 0, sizeof(rx_buffer));
				 wait_char ('&');
				 } else
					 RetryI=-4;
			 } else
				 {
				 memset ( rx_buffer, 0, sizeof(rx_buffer));
				 wait_char ('&');
				 }
	 } while ( RetryI-->0 );
	 break_comms();
	 setbkcolor(BLUE);
	 if ( RetryI < -1)
		 return(0);
	}
	} while ( RetriesI++ <1);
	return(1);
}
