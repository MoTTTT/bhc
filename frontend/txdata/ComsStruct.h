/*******************************************************/
/**                                                   **/
/**  INTER MODULE DATA                                **/
/**                                                   **/
/*******************************************************/

#define Paklen 235
struct MesgBuf
  {
  long pkNrL;
  int  pkSI;
  char Buffer[Paklen];
  int crcI;
  } TxMesS;

