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
	const byte num_keywords = 12;

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
  "BMV",
  "FW",
	"Checksum"
	};
	#define Vi 0
	#define VS 1
	#define I 2	// Offically SER# but # does not play that well as macro
	#define CE 3     // ScV
	#define SOC 4     // ScI
	#define TTG 5   // PVV
	#define Alarm 6   // PVI = PVV / VPV
	#define Relay 7    // ScS
  #define AR 8
  #define BMV 9
  #define FW 10
	#define CHECKSUM 11
#endif
