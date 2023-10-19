typedef unsigned char Byte;		/* define Byte type */
typedef unsigned int  Int;		/* define Int type */
typedef unsigned long Long;		/* define Long type */

struct vechile_data {
	char box_no[9];		  /**  serial number vechile number **/
	char doc_no[9];		  /**  document number							**/
	char driveri1[9];		/**  Driver 1 id  								**/
	char driveri2[9];		/**  Driver 2 id  								**/
	char driveri3[9];		/**  Driver 3 id  								**/
	char driveri4[9];		/**  Driver 4 id  								**/
	char driveri5[9];		/**  Driver 5 id  								**/
	char driveri6[9];		/**  Driver 6 id									**/
	char driveri7[9];		/**  Driver 7 id									**/
	char tr_no1[9];		  /**  Trailer ID 1	  							**/
	char tr_no2[9];		  /**  Trailer ID 2	  							**/
	char tr_no3[9];		  /**  Trailer ID 3	  							**/
	char direction[1];	/**  Direction of vechile 				**/
	char trip_km[9];	  /**  trip kilometer 							**/
	char vech_km[9];	  /**  Vehicle speedometer reading 	**/
	char trip_fuel[9];   /**  Fuel used										**/
	char maks_speed[9];  /** Maks allowed speed           **/
	char maks_rpm[9];    /** Maks allowed RPM             **/
	char maks_temp[9];   /** Maks temp                    **/
	char min_pres[9];    /** Min pressure                 **/
	char maks_pres[9];   /** Maks pressure                 **/
}t_vech_data ,*p_vech_data;
/*
struct driver {
	char time[21];
	char driveri[9];	/**  Driver **/
	char driverO[9];	/**  Driver **/
	char drivero[9];	/**  Driver **/
	char fuel[9];
	char trip_km[9];
}signon;
*/
struct vechile_his {
	Long trip_km;
	Long vech_km;
	Int  cnts_speed;
	Int  cnts_liter;
	Int  cnts_kilo;
	Byte adc;
	Byte dig_output;
	Byte dig_input;
} *p_vech;
