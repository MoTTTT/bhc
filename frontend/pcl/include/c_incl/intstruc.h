/*typedef unsigned char unsigned char;*/

struct Sec_15 {
	unsigned char rpm;
	unsigned char speed;
};

struct Min_60 {
	unsigned int fuel;
	unsigned int  ac_fuel;
	unsigned int trip;
	unsigned long tot_km;
	struct Sec_15 sec_15[4];
};
struct Ure {
	time_t tyd;
	struct Min_60 min_60[60];
};
struct Dag {
	struct Ure ure[24];
};