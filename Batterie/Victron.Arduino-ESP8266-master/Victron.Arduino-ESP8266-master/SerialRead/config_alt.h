/*
  config.h - config the keywords for Victron.DIRECT
*/

#define BMV602s
// BMV602s
#ifdef BMV602s
#define byte uint8_t
	const byte buffsize = 32;
	const byte value_bytes = 33;
	const byte label_bytes = 9;
	const byte num_keywords = 28;

	char keywords[num_keywords][label_bytes] = {
	"V",
	"VS",
	"I",
	"CE",
	"SOC",
	"TTG",
	"Alarm",
	"Relay",
	"AR",
	"H1",
	"H2",
	"H3",
	"H4",
	"H5",
	"H6",
	"H7",
	"H8",
	"H9",
	"H10",
	"H11",
	"H12",
	"H13",
	"H14",
	"H15",
	"H16",
	"BMV",
	"FW",
	"Checksum"
	};
	#define V 0
	#define VS 1
	#define I 2	// Offically SER# but # does not play that well as macro
	#define CE 3     // ScV
	#define SOC 4     // ScI
	#define TTG 5   // PVV
	#define Alarm 6   // PVI = PVV / VPV
	#define Relay 7    // ScS
	#define AR 8   // ScERR
	#define H1 9  // SLs
	#define H2 10   // SLI
	#define H3 11
	#define H4 12
	#define H5 13
	#define H6 14
	#define H7 15
	#define H8 16
	#define H9 17
	#define H10 18
	#define H11 19
	#define H12 20
	#define H13 21
	#define H14 22
	#define H15 23
	#define H16 24
	#define BMV 25
	#define FW 26
	#define CHECKSUM 27
#endif
