#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/fcntl.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/termio.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "/usr/grassie/INCLUDE/Msg.h"
#include "/usr/grassie/INCLUDE/msg.h"
#include "/usr/grassie/INCLUDE/monitor.h"
#include "/usr/grassie/INCLUDE/msg_error.h"

#include "ComsStruct.h"

#define WAITCHARPERIOD 3
#define WAIT_LOOPS     5
#define Tes
#define RM
#define Debug
#define Serial
#define DoSleep

char rx_bufferGCA[1024], tx_bufferGCA[1024], id[3], master[100];
int serial_device_id;
long MultiplierGL, MesqidGL, TxFileL;
float CounterGF;
struct termio local_term;


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  SeekPacket( long PacketL, fp *FpPointer)         **/
/**                                                   **/
/**  this function searh a spesific packet string     **/
/**  from the beginning of the file                   **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  SeekPacket( long PacketL, FpPointer)             **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**                                                   **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**       long PaketL packet number                   **/
/**       fpPointer external file opened by tx_data   **/
/**                                                   **/
/*******************************************************/

SeekPacket( long PacketL, FILE *FpPointer)
  {

  rewind ( FpPointer);
  fseek ( FpPointer, PacketL, SEEK_SET);
  
  }



/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  empty_rx_buf( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/
char crc_calc(int length_buffer ,char *buffer_pointer)
  {

  char xor=0;
  unsigned int i,crc_calc;

  i=0;
  xor = rx_bufferGCA[i++];
  do
    {
    xor ^= rx_bufferGCA[i++];
    } while (i< length_buffer);
  return ( xor );
}


int  rx_Packet(long PacketsL)
  {

  char *tmp_p, local_char, *tmp_char1P;
  char BufferCA[1024], CrcCA[9], TCA[256];
  int PakSizeI, CharRxI=0, TestI, CrcI, CrcOKI=1, Rx_ErrorI =0;
  long PakNrL, CrcOKLD=0, CrcNOKLD=0;
  FILE *fp_t;

/**    Wait for D from AWS **/
  PakNrL=0;

#ifdef Deug
  printf( "Waiting for first U\n");
#endif
  PakSizeI=0;
  tmp_char1P = BufferCA;
  tmp_p = BufferCA;
  do
    {
    TestI = read ( serial_device_id, tmp_char1P ,1 );
    if ( TestI == 0)
      {
      if ( Rx_ErrorI++ > WAIT_LOOPS)
        return(1);
      }
    } while ( *tmp_char1P != 'U');
  memset ( BufferCA, 0, sizeof( BufferCA));

#ifdef Deug
  printf( "1 Waiting for U\n");
#endif
  do
    {
    TestI = read ( serial_device_id, tmp_char1P ,1 );
    if ( TestI == 0)
      {
      if ( Rx_ErrorI++ > WAIT_LOOPS)
        return(2);
      }
    } while ( *tmp_char1P == 'U');

  PakSizeI=0;
#ifdef Deug
  printf( "Rx packet\n");
#endif
  do
    {
    TestI = read ( serial_device_id, tmp_p ,1 );
    if ( TestI > 0)
      {
      tmp_p++;
      PakSizeI++;
      } else 
        {
        if ( Rx_ErrorI++ > WAIT_LOOPS)
          return(3);
        }
    } while ( PakSizeI < 8);

/**   rx Pakets to be received **/

    do
      {
      TestI = read ( serial_device_id, tmp_p ,1 );
      if ( TestI > 0)
        {
        tmp_p++;
        PakSizeI++;
        } else 
           {
           if ( Rx_ErrorI++ > WAIT_LOOPS)
             return(4);
           }
      } while ( PakSizeI < 3);

    tmp_char1P= tmp_p;

    do
      {
      TestI = read ( serial_device_id, tmp_p ,1 );
      if ( TestI > 0)
        {
        tmp_p++;
        PakSizeI++;
        } else 
           {
           if ( Rx_ErrorI++ > WAIT_LOOPS)
             return(5);
           }
      } while ( PakSizeI < 12);

    tmp_char1P = strchr ( BufferCA, ' ');
    tmp_char1P++;
    CharRxI = atoi ( tmp_char1P);
#ifdef Debug
printf(   "Pk Length = %d %s PakSizeI %d Pk-Nr %ld\n", CharRxI, BufferCA, PakSizeI, PakNrL+1);
#endif
    memset ( BufferCA, 0, sizeof( BufferCA));
    memset ( rx_bufferGCA, 0, sizeof( rx_bufferGCA));
    PakSizeI=0;
    tmp_p = rx_bufferGCA;
#ifdef Deug
    printf( "Rx all chars\n");
#endif
    do
      {
      if ( read ( serial_device_id, tmp_p ,1 ) > 0)
        {
        PakSizeI++;
        tmp_p++;
        } else 
           {
           if ( Rx_ErrorI++ > WAIT_LOOPS)
             return(6);
           }
      } while ( PakSizeI < CharRxI);
#ifdef Debug
    printf( "Rxed all chars\n");
#endif
    tmp_p = CrcCA;
    memset ( CrcCA, 0, sizeof( CrcCA));
    CrcI=0;
    do
      {
      if ( read ( serial_device_id, tmp_p ,1 ) > 0)
        {
        CrcI++;
        tmp_p++;
        }
      } while ( CrcI < 5);
    CrcI = atoi ( CrcCA);

    TestI = (int)crc_calc( PakSizeI , rx_bufferGCA);
    if ( TestI == CrcI)
      {
     /** data OK **/
      return( 0);
      } else
        {
        return( 1);
        }
  }


int checkRX()
  {

  char *tmp_p;
  int LoopsI, counter;

  tmp_p = &tx_bufferGCA[0];
  putchar( 0x0f);
  for( LoopsI = 0; LoopsI < 5; LoopsI++)
    {
    counter = read ( serial_device_id, tmp_p,1 );
    if( counter != 0)
      {
      putchar ( *tmp_p );
      if( *tmp_p == 'U')
        {
        printf( "%d U RX'ed\n", LoopsI);
        }
      } else
        printf( "%d Check RX\n", LoopsI);
    }
  }



/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  send_error()                                     **/
/**                                                   **/
/**  this function sends a error message to the       **/
/**  schedular/error process.                         **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  send_error(error_string,  error_no)              **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**       int - error_no NOT SYSTEM ERROR !!!!        **/
/**       char - error_string error message           **/
/**                                                   **/
/*******************************************************/

send_error( error_string, error_number)
char *error_string;
int error_number;
  {
  int flag, msg_size;

  flag = IPC_NOWAIT;
  Msgsnd.mtype= error_number;
  msg_size = strlen ( error_string)-1;
  strncpy ( Msgsnd.mtext, error_string, msg_size);
  msgsnd ( msqid_tnc_tx_sched, &Msgsnd, msg_size, -1);
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  send_error()                                     **/
/**                                                   **/
/**  this function sends a error message to the       **/
/**  schedular/error process.                         **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  send_error(error_string,  error_no)              **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**       int - error_no NOT SYSTEM ERROR !!!!        **/
/**       char - error_string error message           **/
/**                                                   **/
/*******************************************************/

Send_error( string, name)
char *string, *name;
  {
  int flag, msg_size;
  int error_number;

  flag = IPC_NOWAIT;
  msg_rx.m_type= error_number;
  memset( msg_rx.m_text, 0 , sizeof( msg_rx.m_text));
  msg_size = strlen (  string)-1;
  sprintf ( msg_rx.m_text, "UUU%s,f,%s&\r\n",name, string);
  msgsnd ( msqid_tnc_tx_maint, &msg_rx, msg_size, -1);
#ifdef Debug
  printf( "%s\r\n", msg_rx.m_text);
#endif
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  break_coms()                                     **/
/**                                                   **/
/** this function ensures that the TNC returns to the **/
/** cmd: mode:                                        **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  break_coms()                                     **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**   wait_Char();                                    **/
/**   write();                                        **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**                                                   **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

break_coms()
  {
  char kar, buffer[526], *tmp_p = &buffer[0];
  int counter;

#ifdef Debug
  printf( "Breaking comms\r\n");
#endif
  empty_rx_buf();
  do
    {
#ifdef Debug
    sleep(2);
    printf("Breaking coms !! \r\nReturning to cmd: !!!!\r\n");
#endif
    write ( serial_device_id, breakcoms, strlen(breakcoms) );
    write ( serial_device_id, breakcoms, strlen(breakcoms) );
    write ( serial_device_id, breakcoms, strlen(breakcoms) );
#ifdef Debug
    printf("Wait 1 sec\r\n");
#endif
    sleep(2);
#ifdef Debug
    printf("Wait :\r\n");
#endif
    write ( serial_device_id, end_line, strlen (end_line) );
#ifdef Debug
    printf("Wait :\r\n");
#endif
    kar = ':';
    wait_Char (&kar );
    }while( kar == 0);
#ifdef Debug
    printf("Send disconnect :\r\n");
#endif
  sprintf( buffer, "D%c", 0x0D);
    write ( serial_device_id, buffer, 2);
    empty_rx_buf();
#ifdef Debug
  printf( "Ext break\r\n");
#endif
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  int init_mssg()                                  **/
/**                                                   **/
/** this function creates all the inter process       **/
/** comunication from tnc to others and others to     **/
/** tnc.                                              **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  int init_mssg()                                  **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  mssget()                                         **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/** Returns a zero if IPPCS was succesfull            **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

int init_mssg()
  {
  int init_flags, test=0;

  init_flags = IPC_CREAT;
#ifdef Debug
  printf( "Init messages\r\n");
#endif

  msqid_tnc_tx_sched   = msgget( TNC_TX_MAINT, 0 );
  if ( msqid_tnc_tx_sched > 0)
    msgctl ( msqid_tnc_tx_sched , IPC_RMID, 0);
  msqid_tnc_tx_sched   = msgget( TNC_TX_MAINT, IPC_CREAT | 0644);
  if (! msqid_tnc_rx_sched<0 )
     return(1);  /** create mail box for coms1 **/
  return(0);
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  int init_coms()                                  **/
/**                                                   **/
/** This function initializes the coms port defined   **/
/** by the vairable passed to the program when the    **/
/** program was started.                              **/
/**                                                   **/
/** The port would be set to NO PARITY, 8BITS.        **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  int init_coms()                                  **/
/**                                                   **/
/**  int is 0 if the init. was succesfull             **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  init_mssg()                                      **/
/**  open()                                           **/
/**  ioctl()                                          **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/** Returns a zero if INIT  was succesfull            **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

int init_coms( device_name)
char *device_name;
  {
  int min;

  serial_device_id = open( device_name, O_RDWR,  O_SYNC);

  if ( serial_device_id == -1 )
    {
    printf( "%s %s %d !!\r\n",error_strings[4], device_name, errno);
#ifdef Debug
#endif
    exit(0);
    } else
      {
#ifdef Debug
      printf ( "Opening link with TNC !!\r\n");
#endif
      local_term.c_oflag    &= OPOST ;
      local_term.c_iflag    &= ~ICANON ;
      local_term.c_lflag    &= ~(ISIG|ECHO|IXOFF|IXON);
      local_term.c_cflag    = B19200|CS8|CREAD ;
      local_term.c_cc[VTIME] = 100;
      local_term.c_cc[VMIN] = 0;
      min = ioctl ( serial_device_id, TCSETA, &local_term);
      if ( min == -1 )
        {
#ifdef Debug
        printf( "%s%d\r\n",error_strings[4], errno);
#endif
        min = close ( serial_device_id);
        exit( errno);
        } else
          {
          
          }
      }
/** create all ipcs **/
    min= init_mssg();
    if ( min != 0)
      {
#ifdef Debug
      printf( "%s%d !!\r\nProgram aborted!!\r\n", min);
#endif
      exit(0);
      }
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**   wait_Char( kar)                                 **/
/**                                                   **/
/** This function reads the serial line to see        **/
/** weather an char has been received. If a char was  **/
/** received the counter min would be set to 0.       **/
/** If no char was received and Rx_char_wait was      **/
/** exeeded kar would be set to 0.                    **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**   wait_Char( kar)                                 **/
/**                                                   **/
/**    kar - char to be received from the TNC         **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**   read();                                         **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**   tipe - is a pointer to the charachter the       **/
/**          should be waiting for.                   **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

wait_Char( tipe)
  char *tipe;
  {
  char local_char=0, t_char;
  long  min=0, test;

  t_char = *tipe;
  do
    {
    test = read ( serial_device_id, &local_char ,1 );
    if ( test !=0)
      {
      min =0;
      }
    }while ( (min++ < (Rx_char_wait*MultiplierGL) ) && ( t_char !=local_char) );

   if ( min > Rx_char_wait)
     {
     *tipe = 0;
     }
   }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**   wait_char( kar)                                 **/
/**                                                   **/
/** This function calls wait_Char with the type of    **/
/** kar char. The program will remain in this loop    **/
/** untilkar was received.                            **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**   wait_char( kar)                                 **/
/**                                                   **/
/**    kar - char to be received from the TNC         **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**   wait_Char();                                    **/
/**   write();                                        **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**    kar - defines the charachter which should be   **/
/**          received on the RS 232 port              **/
/**    kar is normaly : or *                          **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

wait_char( karakter)
char *karakter;
  {
  char rx_char;

  rx_char = *karakter;
  do
    {
#ifdef Debug
     printf("3\r\n");
#endif
    wait_Char( &rx_char);
#ifdef Debug
   if ( rx_char !=0)
     putchar( rx_char);
#endif
    } while ( rx_char != *karakter);
  }


int waitOK( long PkNR )
  {
  char rx_charsCP, rxBufCA[100], *charP;
  int test, min, CharNrI=0;

#ifdef Debug
  printf( "Waiting OK\n");
#endif

  test = 0;
  min =0;
/*
  empty_rx_buf();
*/
  memset ( rxBufCA, 0 , sizeof( rxBufCA));
  if ( rx_Packet( 0) == 0)
    {
    printf ( "Returning !!! \n");
    printf( "%s\n", rx_bufferGCA);
    if ( rx_bufferGCA[0] == 'O')
      {
      printf( "%s OK\n", rx_bufferGCA);
      return (0);
      }
    } else 
      {
      printf( "%s not OK\n", rx_bufferGCA);
      return (1);
      }
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  pole a station                                   **/
/**                                                   **/
/** this function call the automatic weather station  **/
/** via the TNC and then wait for the received data   **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**  int pole( name,buffer)                           **/
/**                                                   **/
/**    (int) returns (0) if polling was successfull   **/
/**    name - the name of the remote station          **/
/**    buffer - a pointer to the receive buffer area  **/
/**             in memory.                            **/
/**                                                   **/
/**    A device (TNC) should have been succesfully    **/
/**    opened before calling this module!             **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**   read();                                         **/
/**   write();                                        **/
/**                                                   **/
/**  END EXTERNAL CALLED MODULES                      **/
/*******************************************************/

/*******************************************************/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/**    name - the name of the remote station          **/
/**           with the names off the stations which   **/
/**           could be used for digipeating.          **/
/**                                                   **/
/**    buffer - a pointer to the receive buffer area  **/
/**                                                   **/
/**    serial_device_id - a pointer to the I/O device **/
/**                       eg. /dev/tty11              **/
/**                                                   **/
/**  END INTER MODULE DATA                            **/
/*******************************************************/

int pole ( name)
char *name;
  {
  struct tm *my_time;
  time_t mytime;

  char *t_time, Data_dir[857], kar;
  char *tmp_p1, tmp_buf[513], *tmp_p, test, buffer[256];
  int  lines, error_poles=0, VairablesCounterI=0, TTT;
  int  counter=0, trial=0, error_loop_counter=0, bell=0;
  long rx_counter=0;

/** Build the polling message in tmp_buf to be send to the TNC 
         connect_string + name **/
  break_coms();
  memset ( tmp_buf, 0 , sizeof( tmp_buf));
  sprintf( tmp_buf, "%c", 0x0D);
  write ( serial_device_id, tmp_buf, strlen (tmp_buf) );
  memset ( tmp_buf, 0 , sizeof( tmp_buf));
  sprintf( tmp_buf, "c %s%c", master, 0x0d);
  counter =0;
  tmp_p = Data_dir;
  write ( serial_device_id, tmp_buf, strlen (tmp_buf) );
  tmp_p = buffer;
  do
    {
/** wait the connect string **/
    counter = read ( serial_device_id, tmp_p,1 );
    if( counter == 0)
      {
      rx_counter++;

      if (rx_counter > (long)Rx_char_wait_from_tnc*MultiplierGL)
        {
        if ( error_poles++ > 1)
          {
          send_error(tmp_buf, 7);
          return(2);
          }
        error_poles++;
  empty_rx_buf();
  memset ( tmp_buf, 0 , sizeof( tmp_buf));
        rx_counter =0;
        tmp_p = tmp_buf;
        sprintf( tmp_buf, "%c", 0x0D);
        write ( serial_device_id, tmp_buf, strlen (tmp_buf) );
  empty_rx_buf();
  memset ( tmp_buf, 0 , sizeof( tmp_buf));
        sprintf( tmp_buf, "c %s%c", master, 0x0d);
        write ( serial_device_id, tmp_buf, strlen (tmp_buf) );
        }
      } else 
         { 
         if ( *tmp_p == '@') /** check for bell condition !!*/
           {
           trial =4;
           }
         }
    } while (trial < 3);
   return(0);
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  empty_rx_buf( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/
void TreatTxdata()
  {
  char TempS[5];

#define Paklen 235
struct MesgBuf
  {
  long pkNrL;
  int  pkSI;
  char Buffer[Paklen];
  int crcI;
  } TxMesS;

  long PackL = 0, ppkLD =0 ;
  char *TmpP, CommandCA[800], tbuf[64], tmp_buf [5];
  int RcharI, crcI, ErrorI=0, TestI;
  FILE *fp_read, *fp_write;
  float LoopCounter;

#ifdef Debug
printf( "CounterGF - %f\r\n", CounterGF);
#endif

  fp_write = fopen ( "Float", "wt");
  sprintf ( CommandCA, "%f", ++CounterGF);
  fwrite ( CommandCA, sizeof(char), strlen( CommandCA), fp_write );
  fclose ( fp_write);

  fp_read = fopen ( "T", "rt");
  strncat ( CommandCA, "pak", 4);
  fp_write = fopen ( "pak", "wb");

/** Build the packet **/
  memset( TempS, 0, sizeof( TempS));
  sprintf( TempS, "%c", 0x0D);
  do
    {
    memset( TxMesS.Buffer,0, sizeof( TxMesS.Buffer));
    RcharI =fread ( TxMesS.Buffer, sizeof( char), Paklen, fp_read);
    strcpy( rx_bufferGCA, TxMesS.Buffer);
    TxMesS.pkNrL = PackL++;
    TxMesS.pkSI  = RcharI;
    TxMesS.crcI  = (int) crc_calc(RcharI , TxMesS.Buffer);
    fwrite ( &TxMesS, sizeof( TxMesS),1 , fp_write);
    } while (RcharI == Paklen);

  fclose ( fp_read);
  fclose ( fp_write);
  fp_write = fopen ( "pak", "rb");
  rewind ( fp_write);

/**
packet structure 
UU PKnr PKsize String "pakets in messg" crc
**/

  memset ( TxMesS.Buffer ,0 ,sizeof (TxMesS.Buffer));

  sprintf( tbuf, "%08ld", PackL);
  RcharI = strlen ( tbuf);

  sprintf(  CommandCA, "UUU%08ld %03d %s%05d%c",
        0,                        /** packet nr          **/
        RcharI,                   /** string length      **/
        tbuf,
        TxMesS.crcI  = (int) crc_calc(RcharI , tbuf), 0x0d);
#ifdef Debug
  printf( "FFFF  Packets to tx %d\r\n%sCrc - %d\r\n",(int) PackL, CommandCA, TxMesS.crcI);
#endif
  }


int txdata ( )
  {

  char TempS[5];

#define Paklen 235
struct MesgBuf
  {
  long pkNrL;
  int  pkSI;
  char Buffer[Paklen];
  int crcI;
  } TxMesS;

  long PackL = 0, ppkLD =0 ;
  char *TmpP, CommandCA[800], tbuf[64], tmp_buf [5];
  int RcharI, crcI, ErrorI=0, TestI, ErrorStat = 0;
  FILE *fp_read, *fp_write;
  float LoopCounter;

  do
   {
   printf( "Executing %s\n", CommandCA);
   write ( serial_device_id, CommandCA, strlen (CommandCA) );
   LoopCounter = 4;
   TestI = waitOK( 0 );
   if ( TestI != 0)
     {
       printf( "Wait packet 0 %d - ErrorI \n", ErrorI );
       ErrorStat = -1;
       if( ErrorI++ > 2)
         {
         printf( "This should perform a break !! \n");
         ErrorStat = -1;
         return(1);
         }
     }
   } while ( TestI != 0);

  printf( " Test OK \n");
  if( ErrorStat < 0)
    {
    ErrorStat = -1;
    return( 1);
    } else 
    ErrorI = 0;

  printf( "Main tx loop %s\n", CommandCA);

  ppkLD =0 ;
  do
    {
    printf( "Doing PK-NR %ld\n", ppkLD++);
    if ( fread ( &TxMesS, sizeof( TxMesS),1 , fp_write) != 0)
      {
      memset( CommandCA, 0, sizeof( CommandCA));
      sprintf(  CommandCA, "UUU%08ld %03d %s%05d%c",
          TxMesS.pkNrL, \
          TxMesS.pkSI, \
          TxMesS.Buffer, \
          TxMesS.crcI,   \
           0x0d);
      do
       {
       write ( serial_device_id, CommandCA, strlen (CommandCA) );
       write ( serial_device_id, TempS, strlen (TempS) );
       LoopCounter = 4;
       TestI = waitOK( TxMesS.pkNrL);
       if ( TestI != 0)
         {
         printf( "Wait 2 ErrirI - %d\n", ErrorI );
         if ( ErrorI ++ > 2)
           {
           ErrorStat = -1;
           return(1);
           }
         }
       } while ( TestI != 0);
#ifdef Debug
      printf( "%s Crc %d\r\n", CommandCA , TxMesS.crcI);
#endif
      }
    } while ( !feof( fp_write) && ErrorStat == 0);

   fclose (fp_write);
   if( ErrorStat == 0)
    {
    return (0);
    } else
    return (1);
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  empty_rx_buf( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/
empty_rx_buf( )
  {
  char local_char;
  int counter;

  for ( counter = 0; counter < 3; counter++)
    {
    if ( read ( serial_device_id, &local_char ,1 ) >0)
      {
#ifdef Debug
        putchar( local_char);
#endif
      }
    }
   }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  empty_rx_buf( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/

DoInit()
  {
  char *InitDirCP, String[512], kar, FileName[512], *CharP;
  FILE *fp;

  InitDirCP = getenv ( "HOME");
  sprintf( FileName, "%s/PakInit", InitDirCP);
  fp = fopen( FileName, "rt");

  do
   {
#ifdef Debug
    printf( "Waiting for char modem to respond\r\n");
#endif
   write ( serial_device_id, end_line, strlen (end_line) );
#ifdef Debug
    printf("Wait :\r\n");
#endif
   kar = ':';
   wait_Char (&kar );
   } while ( kar != ':');
  do
   {
   memset( String, 0, sizeof(String));
   CharP = String;
   do
    {
    kar = fgetc( fp);
    if ( kar == '$')
       {
#ifdef Debug
printf( "String - %s Exit from file %s\r\n", String ,FileName);
#endif
       fclose( fp);
       return;
       }
    if( kar != '\n');
    *CharP++ = kar;
    } while ( kar != '\n');
   *--CharP = 0x0d;
   write ( serial_device_id, String, strlen (String) );
#ifdef Debug
printf( "string %s\r\n",String);
#endif
   empty_rx_buf();
   } while ( !feof( fp));
  }


/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  wait_forFile( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/

int wait_forFile(int serial_device_id)
  {
  char *tmp_p, name [512], *HomeDirCP;
  FILE *fp_pointer;
  int  SleepTimeI;

  HomeDirCP = getenv ( "CDU");
  do
    {
    fp_pointer =0;
    sprintf( name, "%s/uolct.%s", HomeDirCP, id);
#ifdef Debug
printf( "%s\r\n", name);
#endif
    fp_pointer = fopen( name, "rb");
    if ( fp_pointer == 0)
          {
          checkRX();
#ifdef Debug
         printf( "Wait %d sec.\r\n", SleepTimeI);
#endif
         }
    } while ( fp_pointer <= 0);
  fclose ( fp_pointer);
  DoInit();

  sprintf( name, "rm %s/t;rm %s/T;rm %s/t.Z;", \
                     HomeDirCP, \
                     HomeDirCP, \
                     HomeDirCP);
#ifdef Debug
  printf( "%s\r\n", name);
#endif
  system ( name);

/** create backup copy **/
  sprintf( name, "cp %s/uolct.%s %s/backup/coms/data.%.0f;", \
                            HomeDirCP, id, HomeDirCP, CounterGF);
  system ( name);

  sprintf( name, "mv %s/uolct.%s %s/t;", \
                            HomeDirCP, id, HomeDirCP);
#ifdef Debug
  printf( "%s\r\n", name);
#endif
  system ( name);
  sprintf( name, "compress %s/t;", HomeDirCP);
#ifdef Debug
  printf( "%s\r\n", name);
#endif
  system ( name);
  sprintf( name, "uuencode %s/t.Z uolct.%s.%.0f.Z> T",\
                            HomeDirCP, id, CounterGF);
#ifdef Debug
  printf( "%s\r\n", name);
#endif
  system ( name);

  sprintf( name, "cp uolct.%s.%.0f.Z ./coms ",\
                            id, CounterGF);
#ifdef Debug
  printf( "%s\r\n", name);
#endif
  system ( name);
  return( 1);
  }

int Randomizer()
  {
  long randL;

   randL = lrand48();
   while ( randL > 600)
      randL/=10;
   return ((int) randL);
  }

/*******************************************************/
/**            MODULE REQUIREMENTS                    **/
/**                                                   **/
/**  empty_rx_buf( )                                  **/
/**  This functions emptys the serial rx_port from    **/
/**  unwanted charachters                             **/
/**                                                   **/
/**            END MODULE REQUIREMENTS                **/
/*******************************************************/

/*******************************************************/
/**            CALL REQUIREMENTS                      **/
/**                                                   **/
/**                                                   **/
/**        END CALL REQUIREMENTS                      **/
/*******************************************************/

/*******************************************************/
/**  EXTERNAL CALLED MODULES                          **/
/**                                                   **/
/**  read();                                          **/
/**                                                   **/
/**        END EXTERNAL CALLED MODULES                **/
/*******************************************************/

/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/
main (argc, argv)
int argc;
char *argv[];
  {
 
  int ErrorId, CharswriteI, PoledI, TT;
  struct tm *my_time;
  long TL=0;
  time_t mytime;
  char filenameCA[512], *t_charP, *t_charP1, *t_charP2, TcharC;
  int oldmin, SleepTimeI;
  FILE *fp_error;

  fp_error = fopen ( "Float", "r+t");
  if( fp_error != 0)
    fscanf ( fp_error, "%f", &CounterGF);
  fclose ( fp_error);

  if ( argc < 5)
    {
printf("Program incorrectly started !!!\nEgg txdata /dev/ttyXXX 5 id master\n");
    exit (0);
    }

#ifdef Debug
printf( "CounterGF = %f\r\n", CounterGF);
#endif
sleep(5);

  MultiplierGL = atol(argv[2]);
  strcpy ( id, argv[3]);
  strcpy ( master, argv[4]);

#ifdef Debug
  printf ("id - %s master - %s\r\n", id, master);
#endif

#ifdef Serial

  init_coms(argv[1]);

  write ( serial_device_id, end_line, strlen (end_line) );
  empty_rx_buf();

#endif
  time ( &mytime);
  my_time = localtime ( &mytime);
  oldmin = my_time->tm_hour;
#ifdef Debug
  system( "clear");
  printf( "Starting for IPC \r\n");
#endif
  
  while (  init_mssg() != 0);
#ifdef Debug
    printf( "creating IPC\r\n");
#endif

  for(;;)
    {
    break_coms();
    DoInit();
    time ( &mytime);
    my_time = localtime ( &mytime);
    if(  oldmin != my_time->tm_hour)
      {
      msgctl ( msqid_tnc_tx_sched, IPC_RMID, 0);
      oldmin = my_time->tm_hour;
      while (  init_mssg() != 0)
        printf( "creating IPC\r\n");
      }
    memset( rx_bufferGCA, 0, sizeof( rx_bufferGCA));
    if(  wait_forFile( ErrorId))
      {
      TT = 1;
      do
        {
        do
         {
/** conect to remote **/
         PoledI = pole( id );
          if ( PoledI == 0)
           {
           TT = 1;
#ifdef Debug
           printf( "Connect OK\r\n");
#endif
           TreatTxdata();
           TT = txdata( );
           break_coms();
           if ( TT != 0)
             {
            SleepTimeI = Randomizer();
#ifdef Debug
           printf( "Another try sleeping %d sec.\r\n", SleepTimeI);
#endif
             }
           } else 
             {
             SleepTimeI = Randomizer();
#ifdef Debug
             printf( "Another try sleeping %d sec.\r\n", SleepTimeI);
#endif
             sleep( SleepTimeI);
             }
         } while ( PoledI !=0);
        } while ( TT != 0);
#ifdef Debug
      printf( "Waiting for new data %d\r\n", TL++);
#endif
      }
    if ( TT == 0 )
      {
      TT = 1;
#ifdef Debug
      printf( "Inc CounterGF %f\n", CounterGF);
      printf( "Another try sleeping %d sec.\r\n", SleepTimeI);
#endif
      }
    }
  }
