typedef unsigned long Long;

/*   GLOBAL VARIABLES VISIBLE TO THIS FILE */

int errorstat;
int rat;

char assetno[9];
	char functNo[2];
char fleetNo[9];
char vehicle[9];
char drive[] = "Z:\\";
char filename[17];
char initposblock[] = {"128"};

/*   DEFINITIONS  */

#define B_OPEN		0		/** define to open a btrieve file **/
#define B_CLOSE		1		/** define to close a btrieve file **/
#define B_WRITE		2		/** define to write to a btrieve record **/
#define B_GETEQ		5		/** define to read a btrieve record with a key **/
#define B_GETNXT	6		/** define to read the next key btrieve record **/
#define B_GETFST	12	/** define to read the first btrieve key record **/
#define B_STPNXT	24	/** define to read the next logical btrieve record **/
#define B_STOP		25	/** remove BTRIEVE from memory **/
#define B_STPFST	33	/** define to read the first logical record from a btrieve file **/
#define B_STPLST	34	/** define to read the last logical record from a btrieve file **/
#define B_STPPRV	35	/** define to read the previous logical btrieve record **/

#define RUN_HOURS	24

/*   STRUCTURES */


/*			*********************************************************
      *FD  FL-VCTRIN
	!!!! this tructure length should always be 256 char lengths  !!!
*/
struct init {

	char  fleet_no[8];
	char  date[6];
	char  trailer1[8];
	char  trailer2[8];
	char  trailer3[8];
	char  staff1[8];
	char  staff2[8];
	char  staff3[8];
	char  staff4[8];
	char  staff5[8];
	char  staff6[8];
	char  staff7[8];
	char  fuel_low[4];
	char  fuel_high[4];
	char  speed_low[3];
	char  speed_high[3];
	char  rpm_low[4];
	char  rpm_high[4];
	char  water_low[3];
	char  water_high[3];
	char  water_type;
	char  oil_low[3];
	char  oil_high[3];
	char  oil_type;
	char  doc_number[8];
	char 	FILLER[117];
	char	error_flag;
} Initbuf;

struct vctrn {

/*			 01  VCTRN_RECORD   */

	char  serial_no[8];
	char  control_no[8];
	char  fleet_no[8];
	char  kms[7];
	char  trip[7];
	char  memory[6];
	char  fuel_cnt[6];
	char  kms_cnt[6];
	char  log_input[2];
	char  dig_output[2];
	char  dig_input[2];
	char  FILLER[39];
} Specbuf;

struct Cvctrn {
	char  serial_no[9];
	char  control_no[9];
	char  fleet_no[9];
	Long  kms;
	Long  trip;
	int   memory;
	int   fuel_cnt;
	int   kms_cnt;
	int   analog_input;
	int   dig_output;
	int   dig_input;
} C_Specbuf, *TTT;

struct vechile_data {
	char fleet_no[9];		/**  secret number **/
	char box_no[9];			/**  serial number vechile number **/
	char doc_no[9];			/**  document number							**/
	char driveri1[9];		/**  Driver 1 id  								**/
	char driveri2[9];		/**  Driver 2 id  								**/
	char driveri3[9];		/**  Driver 3 id  								**/
	char driveri4[9];		/**  Driver 4 id  								**/
	char driveri5[9];		/**  Driver 5 id  								**/
	char driveri6[9];		/**  Driver 6 id									**/
	char driveri7[9];		/**  Driver 7 id									**/
	char tr_no1[9];			/**  Trailer ID 1	  							**/
	char tr_no2[9];			/**  Trailer ID 2	  							**/
	char tr_no3[9];			/**  Trailer ID 3	  							**/
	char direction[1];	/**  Direction of vechile 				**/
	char trip_km[9];		/**  trip kilometer 							**/
	char vech_km[9];		/**  Vehicle speedometer reading 	**/
	char trip_fuel[9];	/**  Fuel used										**/
	int fuel_low;
	int fuel_high;
	int speed_low;
	int speed_high;
	int rpm_low;
	int rpm_high;
	int water_low;
	int water_high;
	int oil_low;
	int oil_high;
}t_bech_data;

struct control {
	char cntrlkeyname[8];
	char cntrlfuel[2];
	char cntrlfiller1[2];
	char cntrlKms[2];
	char cntrlRatio[5];
	char cntrlfiller2[11];
	char cntrlKeyCode[8];
	char cntrlfunctKey[2];
	char cntrlStart[5];
	char cntrlEnd[5];
} Cntrlbuf;
