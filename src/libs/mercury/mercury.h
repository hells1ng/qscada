#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <termios.h>
#include "../https/https.h"
#include "../../defines.h"
#include "../id/id.h"
#include "../network/networkclass.h"
#include <boost/thread/mutex.hpp>
#include <QMutex>

#define UInt16		uint16_t
#define byte		unsigned char
#pragma pack(1)

#define BAUDRATE 	B9600		// 9600 baud
#define _POSIX_SOURCE 	1		// POSIX compliant source

#define TIME_OUT	50* 1000	// Mercury inter-command delay (mks)
#define CH_TIME_OUT	2		// Channel timeout (sec)
#define BSZ		255
//#define PM_ADDRESS	44		// RS485 address of the power meter
#define TARRIF_NUM	4		// 2 tariffs supported
#define OPT_DEBUG	"--debug"
#define OPT_HELP	"--help"
#define OPT_TEST_RUN	"--testRun"
#define OPT_HUMAN	"--human"
#define OPT_CSV		"--csv"
#define OPT_JSON	"--json"
#define OPT_HEADER	"--header"
#define debugPrint 0

using std::string;
void getDateTimeStr(char *, int, time_t);

// ***** Commands
// Test connection
typedef struct
{
    byte	address;
    byte	command;
    UInt16	CRC;
} TestCmd;

// Connection initialisaton command
typedef struct
{
    byte	address;
    byte	command;
    byte 	accessLevel;
    byte	password[6];
    UInt16	CRC;
} InitCmd;

// Connection terminaion command
typedef struct
{
    byte	address;
    byte	command;
    UInt16	CRC;
} ByeCmd;

// Power meter parameters read command
typedef struct
{
    byte	address;
    byte	command;	// 8h
    byte	paramId;	// No of parameter to read
    byte	BWRI;
    UInt16 	CRC;
} ReadParamCmd;

// ***** Results
// 1-byte responce (usually with status code)
typedef struct
{
    byte	address;
    byte	result;
    UInt16	CRC;
} Result_1b;

// 3-byte responce
typedef struct
{
    byte	address;
    byte	res[3];
    UInt16	CRC;
} Result_3b;

// Result with 3 bytes per phase
typedef struct
{
    byte	address;
    byte	p1[3];
    byte	p2[3];
    byte	p3[3];
    UInt16	CRC;
} Result_3x3b;

// Result with 3 bytes per phase plus 3 bytes for phases sum
typedef struct
{
    byte	address;
    byte	sum[3];
    byte	p1[3];
    byte	p2[3];
    byte	p3[3];
    UInt16	CRC;
} Result_4x3b;

// Result with 4 bytes per phase plus 4 bytes for sum
typedef struct
{
    byte	address;
    byte	ap[4];		// active +
    byte	am[4];		// active -
    byte	rp[4];		// reactive +
    byte	rm[4];		// reactive -
    UInt16	CRC;
} Result_4x4b;

// 3-phase vector (for voltage, frequency, power by phases)
typedef struct
{
    float	p1;
    float	p2;
    float	p3;
} P3V;

// 3-phase vector (for voltage, frequency, power by phases) with sum by all phases
typedef struct
{
    float	sum;
    float	p1;
    float	p2;
    float	p3;
} P3VS;

// Power vector
typedef struct
{
    float 	ap;		// active +
    float	am;		// active -
    float 	rp;		// reactive +
    float 	rm;		// reactive -
} PWV;

// Output results block
typedef struct
{
    P3V 	U;			// voltage
    P3V	I;			// current
    P3V	A;			// phase angles
    P3VS	C;			// cos(f)
    P3VS	P;			// current active power consumption
    P3VS	S;			// current reactive power consumption
    PWV	PR;			// power counters from reset (all tariffs)
    PWV	PRT[TARRIF_NUM];	// power counters from reset (by tariffs)
    PWV	PY;			// power counters for yesterday
    PWV	PT;			// power counters for today
    float	f;			// grid frequency
} OutputBlock;

// **** Enums
typedef enum
{
    OUT = 0,
    IN = 1
} Direction;

typedef enum
{
    OK = 0,
    ILLEGAL_CMD = 1,
    INTERNAL_COUNTER_ERR = 2,
    PERMISSION_DENIED = 3,
    CLOCK_ALREADY_CORRECTED = 4,
    CHANNEL_ISNT_OPEN = 5,
    WRONG_RESULT_SIZE = 256,
    WRONG_CRC = 257,
    CHECK_CHANNEL_TIME_OUT = 258
} ResultCode;

typedef enum
{
    EXIT_OK = 0,
    EXIT_FAIL = 1
} ExitCode;

typedef enum 			// How much energy consumed:
{
    PP_RESET = 0,		// from reset
    PP_YTD = 1,		// this year
    PP_LAST_YEAR = 2,	// last year
    PP_MONTH = 3,		// for the month specified
    PP_TODAY = 4,		// today
    PP_YESTERDAY = 5	// yesterday
} PowerPeriod;

typedef enum			// Output formatting
{
    OF_HUMAN = 0,		// human readable
    OF_CSV = 1,		// comma-separated values
    OF_JSON = 2		// json
} OutputFormat;


/*
 * CAN properties class
 */

class CanClass //: public QThread
{
public:
    enum {
        RTU = 0,
        TCP = 1
    };
    int fd;
    struct termios oldtio, newtio;
    const char* com_port;
    unsigned int baudrate, data_bit, stop_bit;
    char parity;
    NetworkClass net;
    int mode;
    CanClass();
    CanClass(const char *device, unsigned int baud_, char parity_,
             unsigned int data_bit_, unsigned int stop_bit_, const char *ip, int port, int mode_);

    ~CanClass() {
    };
    bool isConnected();
    void closeCan();
    void lock();
    void unlock();
    int write_read_rtu_tcp(byte *cmd, size_t sizecmd, byte* buf);
private:
    QMutex* mutex;
};


//--MERCURY CLASS
class MercuryClass {

public:

    static string str_id;
    string str_line;
    bool isFirstReading;
    int address, connection_error;
    static char str_data[STR_DATA_LINE][STR_DATA_COLUMNS];
    static std::string data;
    int format, header ;//delete fd
    OutputBlock o, o_prev;
    MercuryClass();
    MercuryClass(int address_, const string str_line_ = "") :
                 address(address_), str_line(str_line_)
    {
        header = 0;
        bzero(&o, sizeof(o));
        isFirstReading = true;
        connection_error = 0;
        format = OF_HUMAN;
    }
    ~MercuryClass() {
    }

    void read_data(CanClass* can, GuidClass* guid);
    void send_data();
    string makeId(string line_, int addr_, int pin_);

    // -- Abnormal termination
    static void exitFailure(const char* msg);

    // -- Print out data buffer in hex
    static void printPackage(byte *data, int size_, int isin);

    // -- Non-blocking file read with timeout
    // -- Returns 0 if timed out.
    static int nb_read_impl(int fd, byte* buf, int sz);

    // -- Non-blocking file read with timeout
    // -- Aborts if timed out.
    static int nb_read(int fd, byte* buf, int sz);

private:

    // Compute the MODBUS RTU CRC
    // Source: http://www.ccontrolsys.com/w/How_to_Compute_the_Modbus_RTU_Message_CRC
    UInt16 ModRTU_CRC(byte* buf, int len);

    // -- Check 1 byte responce
    int checkResult_1b(byte* buf, int len);

    // -- Check 3 byte responce
    int checkResult_3b(byte* buf, int len);

    // -- Check 3 bytes x 3 phase responce
    int checkResult_3x3b(byte* buf, int len);

    // -- Check 3 bytes x 3 phase and sum responce
    int checkResult_4x3b(byte* buf, int len);

    // -- Check 4 bytes x 3 phase and sum responce
    int checkResult_4x4b(byte* buf, int len);

    // -- Check the communication channel
    int checkChannel(CanClass* can, int address);

    // -- Connection initialisation
    int initConnection(CanClass* can, int address);

    // -- Close connection
    int closeConnection(CanClass* can, int address);

    // Decode float from 3 bytes
    float B3F(byte b[3], float factor);

    // Decode float from 4 bytes
    float B4F(byte b[4], float factor);

    // Get voltage (U) by phases
    int getU(CanClass* can, int address, P3V* U);

    // Get current (I) by phases
    int getI(CanClass* can, int address, P3V* I);

    // Get power consumption factor cos(f) by phases
    int getCosF(CanClass* can, int address, P3VS* C);

    // Get grid frequency (Hz)
    int getF(CanClass* can, int address, float *f);

    // Get phases angle
    int getA(CanClass* can, int address, P3V* A);

    // Get active power (W) consumption by phases with total
    int getP(CanClass* can, int address, P3VS* P);

    // Get reactive power (VA) consumption by phases with total
    int getS(CanClass* can, int address, P3VS* S);

    /* Get power counters by phases for the period
        periodId - one of PowerPeriod enum values
        month - month number when periodId is PP_MONTH
        tariffNo - 0 for all tariffs, 1 - tariff #1, 2 - tariff #2 etc. */
    int getW(CanClass* can, int address, PWV* W, int periodId, int month, int tariffNo);

    // -- Command line usage help
    void printUsage();

    // -- Output formatting and print
    void printOutput(int format, OutputBlock o, int header);
};

