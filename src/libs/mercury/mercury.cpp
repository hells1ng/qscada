
#include "mercury.h"
//int debugPrint = 0;

// Compute the MODBUS RTU CRC
// Source: http://www.ccontrolsys.com/w/How_to_Compute_the_Modbus_RTU_Message_CRC
UInt16 MercuryClass::ModRTU_CRC(byte* buf, int len)
{
    UInt16 crc = 0xFFFF;

    for (int pos = 0; pos < len; pos++) {
        crc ^= (UInt16)buf[pos];          // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {    // Loop over each bit
            if ((crc & 0x0001) != 0) {      // If the LSB is set
                crc >>= 1;                    // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                            // Else LSB is not set
                crc >>= 1;                    // Just shift right
        }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
}


// -- Abnormal termination
void MercuryClass::exitFailure(const char* msg)
{
    perror(msg);
    //exit(EXIT_FAIL);
}

// -- Print out data buffer in hex
void MercuryClass::printPackage(byte *data, int size_, int isin)
{
    if (debugPrint)
    {
        printf("%s bytes: %d\n\r\t", (isin) ? "Received" : "Sent", size_);
        for (int i=0; i<size_; i++)
            printf("%02X ", (byte)data[i]);
        printf("\n\r");
        printf("DEBUG_PRINT %d \n\r",debugPrint);
    }
}

// -- Non-blocking file read with timeout
// -- Returns 0 if timed out.
int MercuryClass::nb_read_impl(int fd, byte* buf, int sz)
{
    fd_set set;
    struct timeval timeout;

    // Initialise the input set
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Set timeout
    timeout.tv_sec = CH_TIME_OUT;
    timeout.tv_usec = 0;

    int r = select(fd + 1, &set, NULL, NULL, &timeout);
    if (r < 0)
        exitFailure("Select failed.");
    if (r == 0)
        return 0;

    return read(fd, buf, BSZ);
}

// -- Non-blocking file read with timeout
// -- Aborts if timed out.
int MercuryClass::nb_read(int fd, byte* buf, int sz)
{
    int r = nb_read_impl(fd, buf, sz);
    if (r == 0)
        exitFailure("Communication channel timeout.");
    return r;
}

// -- Check 1 byte responce
int MercuryClass::checkResult_1b(byte* buf, int len)
{
    if (len != sizeof(Result_1b))
        return WRONG_RESULT_SIZE;

    Result_1b *res = (Result_1b*)buf;
    UInt16 crc = ModRTU_CRC(buf, len - sizeof(UInt16));
    if (crc != res->CRC)
        return WRONG_CRC;

    return res->result & 0x0F;
}

// -- Check 3 byte responce
int MercuryClass::checkResult_3b(byte* buf, int len)
{
    if (len != sizeof(Result_3b))
        return WRONG_RESULT_SIZE;

    Result_3b *res = (Result_3b*)buf;
    UInt16 crc = ModRTU_CRC(buf, len - sizeof(UInt16));
    if (crc != res->CRC)
        return WRONG_CRC;

    return OK;
}

// -- Check 3 bytes x 3 phase responce
int MercuryClass::checkResult_3x3b(byte* buf, int len)
{
    if (len != sizeof(Result_3x3b))
        return WRONG_RESULT_SIZE;

    Result_3x3b *res = (Result_3x3b*)buf;
    UInt16 crc = ModRTU_CRC(buf, len - sizeof(UInt16));
    if (crc != res->CRC)
        return WRONG_CRC;

    return OK;
}

// -- Check 3 bytes x 3 phase and sum responce
int MercuryClass::checkResult_4x3b(byte* buf, int len)
{
    if (len != sizeof(Result_4x3b))
        return WRONG_RESULT_SIZE;

    Result_4x3b *res = (Result_4x3b*)buf;
    UInt16 crc = ModRTU_CRC(buf, len - sizeof(UInt16));
    if (crc != res->CRC)
        return WRONG_CRC;

    return OK;
}

// -- Check 4 bytes x 3 phase and sum responce
int MercuryClass::checkResult_4x4b(byte* buf, int len)
{
    if (len != sizeof(Result_4x4b))
        return WRONG_RESULT_SIZE;

    Result_4x4b *res = (Result_4x4b*)buf;
    UInt16 crc = ModRTU_CRC(buf, len - sizeof(UInt16));
    if (crc != res->CRC)
        return WRONG_CRC;

    return OK;
}


// -- Check the communication channel
int MercuryClass::checkChannel(CanClass* can, int address)
{
    // Command initialisation
    TestCmd testCmd = { testCmd.address = address, testCmd.command = 0x00 };
    testCmd.CRC = ModRTU_CRC((byte*)&testCmd, sizeof(testCmd) - sizeof(UInt16));

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&testCmd, sizeof(testCmd), buf);

    if (len == 0)
        return CHECK_CHANNEL_TIME_OUT;

    return checkResult_1b(buf, len);
}

// -- Connection initialisation
int MercuryClass::initConnection(CanClass* can, int address)
{
    InitCmd initCmd = {
        /*initCmd.address =*/ address,
        /*initCmd.command =*/ 0x01,
        /*initCmd.accessLevel=*/ 0x01,
        {0x01,0x01,0x01,0x01,0x01,0x01}
    };
    //initCmd.password[6] = 0x01;//{0x01,0x01,0x01,0x01,0x01,0x01};
    initCmd.CRC = ModRTU_CRC((byte*)&initCmd, sizeof(initCmd) - sizeof(UInt16));

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&initCmd, sizeof(initCmd), buf);
    return checkResult_1b(buf, len);
}
// -- Close connection
int MercuryClass::closeConnection(CanClass* can, int address)
{
    ByeCmd byeCmd = { byeCmd.address = address, byeCmd.command = 0x02 };
    byeCmd.CRC = ModRTU_CRC((byte*)&byeCmd, sizeof(byeCmd) - sizeof(UInt16));

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&byeCmd, sizeof(byeCmd), buf);

    return checkResult_1b(buf, len);
}

// Decode float from 3 bytes
float MercuryClass::B3F(byte b[3], float factor)
{
    int val = ((b[0] & 0x3F) << 16) | (b[2] << 8) | b[1];
    return val/factor;
}

// Decode float from 4 bytes
float MercuryClass::B4F(byte b[4], float factor)
{
    int val = ((b[1] & 0x3F) << 24) | (b[0] << 16) | (b[3] << 8) | b[2];
    return val/factor;
}

// Get voltage (U) by phases
int MercuryClass::getU(CanClass* can, int address, P3V* U)
{
    ReadParamCmd getUCmd =
    {
        getUCmd.address = address,
        getUCmd.command = 0x08,
        getUCmd.paramId = 0x16,
        getUCmd.BWRI = 0x11
    };
    getUCmd.CRC = ModRTU_CRC((byte*)&getUCmd, sizeof(getUCmd) - sizeof(UInt16));
    printPackage((byte*)&getUCmd, sizeof(getUCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getUCmd, sizeof(getUCmd), buf);

    // Check and decode result
    int checkResult = checkResult_3x3b(buf, len);
    if (OK == checkResult)
    {
        Result_3x3b* res = (Result_3x3b*)buf;
        U->p1 = B3F(res->p1, 100.0);
        U->p2 = B3F(res->p2, 100.0);
        U->p3 = B3F(res->p3, 100.0);
    }

    return checkResult;
}

// Get current (I) by phases
int MercuryClass::getI(CanClass* can, int address, P3V* I)
{
    ReadParamCmd getICmd =
    {
        getICmd.address = address,
        getICmd.command = 0x08,
        getICmd.paramId = 0x16,
        getICmd.BWRI = 0x21
    };
    getICmd.CRC = ModRTU_CRC((byte*)&getICmd, sizeof(getICmd) - sizeof(UInt16));
    printPackage((byte*)&getICmd, sizeof(getICmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getICmd, sizeof(getICmd), buf);

    // Check and decode result
    int checkResult = checkResult_3x3b(buf, len);
    if (OK == checkResult)
    {
        Result_3x3b* res = (Result_3x3b*)buf;
        I->p1 = B3F(res->p1, 1000.0);
        I->p2 = B3F(res->p2, 1000.0);
        I->p3 = B3F(res->p3, 1000.0);
    }

    return checkResult;
}

// Get power consumption factor cos(f) by phases
int MercuryClass::getCosF(CanClass* can, int address, P3VS* C)
{
    ReadParamCmd getCosCmd =
    {
        getCosCmd.address = address,
        getCosCmd.command = 0x08,
        getCosCmd.paramId = 0x16,
        getCosCmd.BWRI = 0x30
    };
    getCosCmd.CRC = ModRTU_CRC((byte*)&getCosCmd, sizeof(getCosCmd) - sizeof(UInt16));
    printPackage((byte*)&getCosCmd, sizeof(getCosCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getCosCmd, sizeof(getCosCmd), buf);

    // Check and decode result
    int checkResult = checkResult_4x3b(buf, len);
    if (OK == checkResult)
    {
        Result_4x3b* res = (Result_4x3b*)buf;
        C->p1 = B3F(res->p1, 1000.0);
        C->p2 = B3F(res->p2, 1000.0);
        C->p3 = B3F(res->p3, 1000.0);
        C->sum = B3F(res->sum, 1000.0);
    }

    return checkResult;
}

// Get grid frequency (Hz)
int MercuryClass::getF(CanClass* can, int address, float *f)
{
    ReadParamCmd getFCmd =
    {
        getFCmd.address = address,
        getFCmd.command = 0x08,
        getFCmd.paramId = 0x16,
        getFCmd.BWRI = 0x40
    };
    getFCmd.CRC = ModRTU_CRC((byte*)&getFCmd, sizeof(getFCmd) - sizeof(UInt16));
    printPackage((byte*)&getFCmd, sizeof(getFCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getFCmd, sizeof(getFCmd), buf);

    // Check and decode result
    int checkResult = checkResult_3b(buf, len);
    if (OK == checkResult)
    {
        Result_3b* res = (Result_3b*)buf;
        *f = B3F(res->res, 100.0);
    }

    return checkResult;
}

// Get phases angle
int MercuryClass::getA(CanClass* can, int address, P3V* A)
{
    ReadParamCmd getACmd =
    {
        getACmd.address = address,
        getACmd.command = 0x08,
        getACmd.paramId = 0x16,
        getACmd.BWRI = 0x51
    };
    getACmd.CRC = ModRTU_CRC((byte*)&getACmd, sizeof(getACmd) - sizeof(UInt16));
    printPackage((byte*)&getACmd, sizeof(getACmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getACmd, sizeof(getACmd), buf);

    // Check and decode result
    int checkResult = checkResult_3x3b(buf, len);
    if (OK == checkResult)
    {
        Result_3x3b* res = (Result_3x3b*)buf;
        A->p1 = B3F(res->p1, 100.0);
        A->p2 = B3F(res->p2, 100.0);
        A->p3 = B3F(res->p3, 100.0);
    }

    return checkResult;
}

// Get active power (W) consumption by phases with total
int MercuryClass::getP(CanClass* can, int address, P3VS* P)
{
    ReadParamCmd getPCmd =
    {
        getPCmd.address = address,
        getPCmd.command = 0x08,
        getPCmd.paramId = 0x16,
        getPCmd.BWRI = 0x00
    };
    getPCmd.CRC = ModRTU_CRC((byte*)&getPCmd, sizeof(getPCmd) - sizeof(UInt16));
    printPackage((byte*)&getPCmd, sizeof(getPCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getPCmd, sizeof(getPCmd), buf);

    // Check and decode result
    int checkResult = checkResult_4x3b(buf, len);
    if (OK == checkResult)
    {
        Result_4x3b* res = (Result_4x3b*)buf;
        P->p1 = B3F(res->p1, 100.0);
        P->p2 = B3F(res->p2, 100.0);
        P->p3 = B3F(res->p3, 100.0);
        P->sum = B3F(res->sum, 100.0);
    }

    return checkResult;
}

// Get reactive power (VA) consumption by phases with total
int MercuryClass::getS(CanClass* can, int address, P3VS* S)
{
    ReadParamCmd getSCmd =
    {
        getSCmd.address = address,
        getSCmd.command = 0x08,
        getSCmd.paramId = 0x16,
        getSCmd.BWRI = 0x08
    };
    getSCmd.CRC = ModRTU_CRC((byte*)&getSCmd, sizeof(getSCmd) - sizeof(UInt16));
    printPackage((byte*)&getSCmd, sizeof(getSCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getSCmd, sizeof(getSCmd), buf);

    // Check and decode result
    int checkResult = checkResult_4x3b(buf, len);
    if (OK == checkResult)
    {
        Result_4x3b* res = (Result_4x3b*)buf;
        S->p1 = B3F(res->p1, 100.0);
        S->p2 = B3F(res->p2, 100.0);
        S->p3 = B3F(res->p3, 100.0);
        S->sum = B3F(res->sum, 100.0);
    }

    return checkResult;
}

/* Get power counters by phases for the period
    periodId - one of PowerPeriod enum values
    month - month number when periodId is PP_MONTH
    tariffNo - 0 for all tariffs, 1 - tariff #1, 2 - tariff #2 etc. */
int MercuryClass::getW(CanClass* can,int address, PWV* W, int periodId, int month, int tariffNo)
{
    ReadParamCmd getWCmd =
    {
        getWCmd.address = address,
        getWCmd.command = 0x05,
        getWCmd.paramId = (periodId << 4) | (month & 0xF),
        getWCmd.BWRI = tariffNo
    };
    getWCmd.CRC = ModRTU_CRC((byte*)&getWCmd, sizeof(getWCmd) - sizeof(UInt16));

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
    len = can->write_read_rtu_tcp((byte*)&getWCmd, sizeof(getWCmd), buf);
    // Check and decode result
    int checkResult = checkResult_4x4b(buf, len);
    if (OK == checkResult)
    {
        Result_4x4b* res = (Result_4x4b*)buf;
        W->ap = B4F(res->ap, 1000.0);
        W->am = B4F(res->am, 1000.0);
        W->rp = B4F(res->rp, 1000.0);
        W->rm = B4F(res->rm, 1000.0);
    }

    return checkResult;
}

// -- Command line usage help
void MercuryClass::printUsage()
{
    printf("Usage: mercury236 RS485 [OPTIONS] ...\n\r\n\r");
    printf("  RS485\t\taddress of RS485 dongle (e.g. /dev/ttyUSB0), required\n\r");
    printf("  %s\tto print extra debug info\n\r", OPT_DEBUG);
    printf("  %s\tdry run to see output sample, no hardware required\n\r", OPT_TEST_RUN);
    printf("\n\r");
    printf("  Output formatting:\n\r");
    printf("  %s\thuman readable (default)\n\r", OPT_HUMAN);
    printf("  %s\t\tCSV\n\r", OPT_CSV);
    printf("  %s\tjson\n\r", OPT_JSON);
    printf("  %s\tto print data header (with %s only)\n\r", OPT_HEADER, OPT_CSV);
    printf("\n\r");
    printf("  %s\tprints this screen\n\r", OPT_HELP);
}

void getDateTimeStr(char *str, int length, time_t time)
{
    struct tm *ti = localtime(&time);

    snprintf(str, length, "%4d-%02d-%02d %02d:%02d:%02d",
             ti->tm_year+1900, ti->tm_mon+1, ti->tm_mday,
             ti->tm_hour, ti->tm_min, ti->tm_sec);
}

// -- Output formatting and print
void MercuryClass::printOutput(int format, OutputBlock o, int header)
{
    // getting current time for timestamp
    char timeStamp[BSZ];
    getDateTimeStr(timeStamp, BSZ, time(NULL));

    switch(format)
    {
    case OF_HUMAN:
        printf("  Voltage (V):             		%8.2f %8.2f %8.2f\n\r", o.U.p1, o.U.p2, o.U.p3);
        printf("  Current (A):             		%8.2f %8.2f %8.2f\n\r", o.I.p1, o.I.p2, o.I.p3);
        printf("  Cos(f):                  		%8.2f %8.2f %8.2f (%8.2f)\n\r", o.C.p1, o.C.p2, o.C.p3, o.C.sum);
        printf("  Frequency (Hz):          		%8.2f\n\r", o.f);
        printf("  Phase angles (deg):      		%8.2f %8.2f %8.2f\n\r", o.A.p1, o.A.p2, o.A.p3);
        printf("  Active power (W):        		%8.2f %8.2f %8.2f (%8.2f)\n\r", o.P.p1, o.P.p2, o.P.p3, o.P.sum);
        printf("  Reactive power (VA):     		%8.2f %8.2f %8.2f (%8.2f)\n\r", o.S.p1, o.S.p2, o.S.p3, o.S.sum);
        printf("  Total consumed, all tariffs (KW):	%8.2f\n\r", o.PR.ap);
        printf("    including day tariff (KW):		%8.2f\n\r", o.PRT[0].ap);
        printf("    including night tariff (KW):	%8.2f\n\r", o.PRT[1].ap);
        printf("  Yesterday consumed (KW): 		%8.2f\n\r", o.PY.ap);
        printf("  Today consumed (KW):     		%8.2f\n\r", o.PT.ap);
        break;

    case OF_CSV:
        if (header)
        {
            // to be the same order as params below
            printf("DT,U1,U2,U3,I1,I2,I3,P1,P2,P2,Psum,S1,S2,S3,Ssum,C1,C2,C3,Csum,F,A1,A2,A3,PRa,PYa,PTa\n\r");

        }
        printf("%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n\r",
               timeStamp,
               o.U.p1, o.U.p2, o.U.p3,
               o.I.p1, o.I.p2, o.I.p3,
               o.P.p1, o.P.p2, o.P.p3, o.P.sum,
               o.S.p1, o.S.p2, o.S.p3, o.S.sum,
               o.C.p1, o.C.p2, o.C.p3, o.C.sum,
               o.f,
               o.A.p1, o.A.p2, o.A.p3,
               o.PR.ap, o.PRT[0].ap, o.PRT[1].ap,
                o.PY.ap,
                o.PT.ap
                );
        break;

    case OF_JSON:
        printf("{\"U\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f},\"I\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f},\"CosF\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f,\"sum\":%.2f},\"F\":%.2f,\"A\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f},\"P\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f,\"sum\":%.2f},\"S\":{\"p1\":%.2f,\"p2\":%.2f,\"p3\":%.2f,\"sum\":%.2f},\"PR\":{\"ap\":%.2f},\"PR-day\":{\"ap\":%.2f},\"PR-night\":{\"ap\":%.2f},\"PY\":{\"ap\":%.2f},\"PT\":{\"ap\":%.2f}}\n\r",
               o.U.p1, o.U.p2, o.U.p3,
               o.I.p1, o.I.p2, o.I.p3,
               o.C.p1, o.C.p2, o.C.p3, o.C.sum,
               o.f,
               o.A.p1, o.A.p2, o.A.p3,
               o.P.p1, o.P.p2, o.P.p3, o.P.sum,
               o.S.p1, o.S.p2, o.S.p3, o.S.sum,
               o.PR.ap, o.PRT[0].ap, o.PRT[1].ap,
                o.PY.ap,
                o.PT.ap
                );
        break;

    default:
        exitFailure("Invalid formatting.");
        break;
    }
}

CanClass::CanClass(const char *device,  unsigned int baud_, char parity_,
                   unsigned int data_bit_, unsigned int stop_bit_, const char *ip, int port, int mode_) :
    com_port(device), net(ip, port), stop_bit(stop_bit_), parity(parity_), mode(mode_)

{
    mutex = new QMutex();

    switch (baud_) {
    case 110:
        baudrate = B110;
        break;
    case 300:
        baudrate = B300;
        break;
    case 600:
        baudrate = B600;
        break;
    case 1200:
        baudrate = B1200;
        break;
    case 2400:
        baudrate = B2400;
        break;
    case 4800:
        baudrate = B4800;
        break;
    case 9600:
        baudrate = B9600;
        break;
    case 19200:
        baudrate = B19200;
        break;
    case 38400:
        baudrate = B38400;
        break;
    case 57600:
        baudrate = B57600;
        break;
    case 115200:
        baudrate = B115200;
        break;

    default:
        baudrate = B9600;
        break;
    }

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    switch (data_bit_) {
    case 5:
        data_bit = CS5;
        break;
    case 6:
        data_bit = CS6;
        break;
    case 7:
        data_bit = CS7;
        break;
    case 8:
    default:
        data_bit = CS8;
        break;
    }
}


void CanClass::lock()
{
    mutex->lock();
}

void CanClass::unlock()
{
    mutex->unlock();
}

bool CanClass::isConnected()
{
    lock();
    switch (mode) {
    case RTU:
        fd = open(com_port, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd < 0) {
            cout << "Connection to CAN failed" << endl;
            //exitFailure(com_port);
            return false;
        }
        else {
            fcntl(fd, F_SETFL, 0);
            tcgetattr(fd, &oldtio); /* save current port settings */
            bzero(&newtio, sizeof(newtio));
            cfsetispeed(&newtio, baudrate);
            cfsetospeed(&newtio, baudrate);
            newtio.c_cflag = baudrate | data_bit | CLOCAL | CREAD;
            //	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
            //	newtio.c_cflag = BAUDRATE | CS8 | CREAD;

            /* Stop bit (1 or 2) */
            if (stop_bit == 1)
                newtio.c_cflag &=~ CSTOPB;
            else /* 2 */
                newtio.c_cflag |= CSTOPB;

            /* PARENB       Enable parity bit
               PARODD       Use odd parity instead of even */
            if (parity == 'N') {
                /* None */
                newtio.c_cflag &=~ PARENB;
            } else if (parity == 'E') {
                /* Even */
                newtio.c_cflag |= PARENB;
                newtio.c_cflag &=~ PARODD;
            } else {
                /* Odd */
                newtio.c_cflag |= PARENB;
                newtio.c_cflag |= PARODD;
            }

            newtio.c_iflag = IGNPAR;
            newtio.c_oflag = 0;
            cfmakeraw(&newtio);
            tcsetattr(fd, TCSANOW, &newtio);
        }
        return true;
        break;
    case TCP:
        //ctx = modbus_new_rtutcp(ip,port);
        return true;
        break;
    default:
        return true;
        break;
    }
}
int CanClass::write_read_rtu_tcp(byte *cmd, size_t sizecmd, byte* buf)
{
    int len;
    MercuryClass::printPackage(cmd, sizeof(cmd), OUT);

    switch (mode) {
    case RTU:
        write(fd, cmd, sizecmd);//(byte*)&cmd
        usleep(TIME_OUT);
        //      len = nb_read_impl(ttyd, buf, BSZ);
        len = MercuryClass::nb_read(fd, buf, BSZ);
        break;
    case TCP:
        //        qDebug()<<"TCP_WRite";
        len = net.write(cmd, sizecmd, buf);
        break;
    default:
        break;
    }

    MercuryClass::printPackage(buf, len, IN);

    return len;
}
void CanClass::closeCan()
{
    switch (mode) {
    case RTU:
        close(fd);
        tcsetattr(fd, TCSANOW, &oldtio);
        break;
    case TCP:
        break;
    default:
        break;
    }
    unlock();
}

char MercuryClass::str_data[STR_DATA_LINE][STR_DATA_COLUMNS] = {};
std::string MercuryClass::data = "";
std::string MercuryClass::str_id = "";


void MercuryClass::send_data()
{
//    SqlDriver SQL;
//    QString qs = SQL.prepare_data(QString::fromStdString(data));
//    SQL.toDataTable(qs);

//    SQL.toDataTable(SQL.prepare_data(QString::fromStdString(data)));
//    return data;
}

std::string MercuryClass::makeId(std::string line_, int addr_, int pin_)
{
    char a[4],p[4];
    sprintf(a, "%d",addr_);
    sprintf(p, "%d",pin_);
    return line_+":"+a+":"+p;
}

Data MercuryClass::read_data(CanClass* can, GuidClass* guid)
{
    QStringList qsl1, qsl2;
    Data retData;
    //open_connection();
    if ( can->isConnected() ) {
        switch(checkChannel(can, address)) {
        case OK :
            if (OK != initConnection(can, address)) {
                connection_error = 1;
                exitFailure("Power meter connection initialisation error.");
            }

            //    // Get voltage by phases
            //                if (OK != getU(can, address, &o.U))
            //                    exitFailure("Cannot collect voltage data.");
            //    // Get current by phases
            //                if (OK != getI(can, address, &o.I))
            //                    exitFailure("Cannot collect current data.");
            //    // Get power cos(f) by phases
            //                if (OK != getCosF(can, address, &o.C))
            //                    exitFailure("Cannot collect cos(f) data.");
            //    // Get grid frequency
            //                if (OK != getF(can, address, &o.f))
            //                    exitFailure("Cannot collect grid frequency data.");
            //                    //std::cout << "!!!!!!!!!!!!!2" << std::endl;
            //    // Get phase angles
            //                if (OK != getA(can, address, &o.A))
            //                    exitFailure("Cannot collect phase angles data.");
            //    // Get active power consumption by phases
            //                if (OK != getP(can, address, &o.P))
            //                    exitFailure("Cannot collect active power consumption data.");
            //    // Get reactive power consumption by phases
            //                if (OK != getS(can, address, &o.S))
            //                    exitFailure("Cannot collect reactive power consumption data.");

            // Get power counter from reset, for yesterday and today
            if     (OK != getW(can, address, &o.PR, PP_RESET, 0, 0) ||		// total from reset
                    OK != getW(can, address, &o.PRT[0], PP_RESET, 0, 0+1) ||	// day tariff from reset
                    OK != getW(can, address, &o.PRT[1], PP_RESET, 0, 1+1) ||	// night tariff from reset
                    OK != getW(can, address, &o.PY, PP_YESTERDAY, 0, 0) ||
                    OK != getW(can, address, &o.PT, PP_TODAY, 0, 0))
                exitFailure("Cannot collect power counters data.");

            if (OK != closeConnection(can, address))
                exitFailure("Power meter connection closing error.");

            break;

        case CHECK_CHANNEL_TIME_OUT :
            break;

        default :
            exitFailure("Power meter communication channel test failed.");
        }

        str_id = makeId(str_line, address, 0);

        if (((*guid)[str_id] != "no_guid") && (connection_error == 0)) {
            //sprintf(str_data[0], "%d", PM_ADDRESS);
            data = "";
            data = data + (*guid)[str_id];
            sprintf(str_data[1], "%.2f", o.U.p1);//Voltage 1(V)
            sprintf(str_data[2], "%.2f", o.U.p2);//Voltage 2(V)
            sprintf(str_data[3], "%.2f", o.U.p3);//Voltage 3(V)
            sprintf(str_data[4], "%.2f", o.I.p1);//Current 1(A)
            sprintf(str_data[5], "%.2f", o.I.p2);//Current 2(A)
            sprintf(str_data[6], "%.2f", o.I.p3);//Current 3(A)
            sprintf(str_data[7], "%.2f", o.C.p1);//Cos(f) 1
            sprintf(str_data[8], "%.2f", o.C.p2);//Cos(f) 2
            sprintf(str_data[9], "%.2f", o.C.p3);//Cos(f) 3
            sprintf(str_data[10], "%.2f", o.C.sum);//Cos(f) sum
            sprintf(str_data[11], "%.2f", o.f);//Frequency (Hz)
            sprintf(str_data[12], "%.2f", o.P.p1);//Active power (W) 1
            sprintf(str_data[13], "%.2f", o.P.p2);//Active power (W) 2
            sprintf(str_data[14], "%.2f", o.P.p3);//Active power (W) 3
            sprintf(str_data[15], "%.2f", o.P.sum);//Active power (W) sum
            sprintf(str_data[16], "%.2f", o.S.p1);//Reactive power (W) 1
            sprintf(str_data[17], "%.2f", o.S.p2);//Reactive power (W) 2
            sprintf(str_data[18], "%.2f", o.S.p3);//Reactive power (W) 3
            sprintf(str_data[19], "%.2f", o.S.sum);//Reactive power (W) sum
            sprintf(str_data[20], "%.2f", o.PR.ap);//Total consumed, all tariffs (KW)
            sprintf(str_data[21], "%.3f", o.PRT[0].ap);//ncluding day tariff (KW)
            sprintf(str_data[22], "%.3f", o.PRT[1].ap);//ncluding night tariff (KW)
            sprintf(str_data[23], "%.2f", o.PY.ap);//Yesterday consumed (KW)
            sprintf(str_data[24], "%.2f", o.PT.ap);//Today consumed (KW)
            /*
            for (int i=1; i<25; i++ ){
                data=data+"/"+str_data[i];
            }
            data=data+"/";//+"\r\n";
    */

            if ((o.PRT[0].ap > 0.00) && (o.PRT[1].ap > 0.00)) {
//                data = data + "|" + str_data[21] + "|1"; //day tariff [12]
//                if (PRINT_RESULTS)
//                    std::cout << data << std::endl;
//                if (SENDING /*&&(isFirstReading||(o.PRT[0].ap!=o_prev.PRT[0].ap))*/)
//                    send_data();  //SEND MERCURY DATA!!!!!!!

//                std::cout<<"DAY: o.PRT[0].ap = "<<o.PRT[0].ap <<"o_prev.PRT[0].ap ="<< o_prev.PRT[0].ap<<std::endl;
//                data = "";
//                data = data + "/" + (*guid)[str_id] + "|" + str_data[22] + "|2";//night tariff [16]
//                if (PRINT_RESULTS)
//                    std::cout << data << std::endl;
//                if (SENDING /*&&(isFirstReading||(o.PRT[1].ap!=o_prev.PRT[1].ap))*/)
//                    send_data();  //SEND MERCURY DATA!!!!!!!

//                std::cout<<"NIGHT o.PRT[1].ap = "<<o.PRT[1].ap <<"o_prev.PRT[1].ap ="<< o_prev.PRT[1].ap<<std::endl;

                qsl1.append(QString::fromStdString((*guid)[str_id]));
                qsl1.append(QString::fromStdString(str_data[21]));
                qsl1.append(QString::number(DATA_VALUE_FLAG1));

                retData.append(qsl1);

                qsl2.append(QString::fromStdString((*guid)[str_id]));
                qsl2.append(QString::fromStdString(str_data[22]));
                qsl2.append(QString::number(DATA_VALUE_FLAG2));

                retData.append(qsl2);

//                qDebug() << "mercury_1 = " << retData[0]
//                         << "mercury_2 = " << retData[1] ;

                connection_error = 0;
                isFirstReading = false;
                o_prev = o;
            }
        }
    };
    /*
    //TODO ne rabotaet opros Mercury
    data = "1e146e46623044b1972078ba22ea6579";
    sprintf(str_data[21], "%.3f", o.PRT[0].ap);//ncluding day tariff (KW)
    data = data + "|" + str_data[21] + "|1"; //day tariff [12]
    send_data();
    */
    //
    can->closeCan();

    qsl1.append(QString("wrong_guid"));
    qsl1.append(QString::fromStdString("34.0"));
    qsl1.append(QString::number(DATA_VALUE_FLAG1));

    retData.append(qsl1);

    qsl2.append(QString::fromStdString((*guid)[str_id]));
    qsl2.append(QString::fromStdString("15.0"));
    qsl2.append(QString::number(DATA_VALUE_FLAG2));

    retData.append(qsl2);

    return retData;
}

