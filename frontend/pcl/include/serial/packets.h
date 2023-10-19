#define PACKET_SIZE 255
#define CRC_SIZE 2
#define SYNCH_SIZE 5
#define PACKET_NR_SIZE 2
#define BOX_NUMBER_SIZE 8
#define NUMBER_OF_PACKETS_SIZE 2
#define OVER_HEADS = CRC_SIZE + SYNCH_SIZE + BOX_NUMBER_SIZE + NUMBER_OF_PACKETS_SIZE
#define DATA_SIZE = PACKET_SIZE - OVERHEADS
#define SYNCH "UUUUU"
#define XM_BUF 256 			/** Transmision buffer sizes **/
