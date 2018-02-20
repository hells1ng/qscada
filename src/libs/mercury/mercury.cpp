
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
    //TODO need sz?
    sz++;

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



// -- Check the communication channel
int MercuryClass::checkChannel(int address)
{
    // Command initialisation
    TestCmd testCmd = { testCmd.address = address, testCmd.command = 0x00, 0x00 };
    testCmd.CRC = ModRTU_CRC((byte*)&testCmd, sizeof(testCmd) - sizeof(UInt16));

    // Send Req and Get responce
//    byte buf[BSZ];
//    int len = 0;
//    len = write((byte*)&testCmd, sizeof(testCmd), buf);
//    qDebug() << "Mercury write checkChannel";
//    emit write((byte*)&testCmd, sizeof(testCmd));

//    ioDriver.write((byte*)&testCmd, sizeof(testCmd));
//    while (!receivedData) {
//    };
//    receivedData = false;

    QByteArray response = ioDriver.writes((byte*)&testCmd, sizeof(testCmd));
    received(response);
    if (len == 0)
        return CHECK_CHANNEL_TIME_OUT;

    return checkResult_1b(buf, len);
}

// -- Connection initialisation
int MercuryClass::initConnection(int address)
{
    InitCmd initCmd = {
        /*initCmd.address =*/ (unsigned char)address,
        /*initCmd.command =*/ 0x01,
        /*initCmd.accessLevel=*/ 0x01,
        {0x01,0x01,0x01,0x01,0x01,0x01},
        0x00
    };
    //initCmd.password[6] = 0x01;//{0x01,0x01,0x01,0x01,0x01,0x01};
    initCmd.CRC = ModRTU_CRC((byte*)&initCmd, sizeof(initCmd) - sizeof(UInt16));

    // Send Req and Get responce
//    byte buf[BSZ];
//    int len = 0;
//    len = can->write((byte*)&initCmd, sizeof(initCmd), buf);
//    emit write((byte*)&initCmd, sizeof(initCmd));

//    ioDriver.write((byte*)&initCmd, sizeof(initCmd));
//    while (!receivedData) {
//    };
//    receivedData = false;

    QByteArray response = ioDriver.writes((byte*)&initCmd, sizeof(initCmd));
    received(response);
    if (len == 0)
        return CHECK_CHANNEL_TIME_OUT;

    return checkResult_1b(buf, len);
}
// -- Close connection
int MercuryClass::closeConnection(int address)
{
    ByeCmd byeCmd = { byeCmd.address = address, byeCmd.command = 0x02, 0x00};
    byeCmd.CRC = ModRTU_CRC((byte*)&byeCmd, sizeof(byeCmd) - sizeof(UInt16));

    // Send Req and Get responce
//    byte buf[BSZ];
//    int len = 0;
//    len = can->write((byte*)&byeCmd, sizeof(byeCmd), buf);
//    emit write((byte*)&byeCmd, sizeof(byeCmd));

//    ioDriver.write((byte*)&byeCmd, sizeof(byeCmd));
//    while (!receivedData) {
//    };
//    receivedData = false;

    QByteArray response = ioDriver.writes((byte*)&byeCmd, sizeof(byeCmd));
    received(response);
    if (len == 0)
        return CHECK_CHANNEL_TIME_OUT;

    return checkResult_1b(buf, len);
}

/* Get power counters by phases for the period
    periodId - one of PowerPeriod enum values
    month - month number when periodId is PP_MONTH
    tariffNo - 0 for all tariffs, 1 - tariff #1, 2 - tariff #2 etc. */
int MercuryClass::getW(int address, PWV* W, int periodId, int month, int tariffNo)
{
    ReadParamCmd getWCmd =
    {
        getWCmd.address = address,
        getWCmd.command = 0x05,
        getWCmd.paramId = (periodId << 4) | (month & 0xF),
        getWCmd.BWRI = tariffNo,
        0x00
    };
    getWCmd.CRC = ModRTU_CRC((byte*)&getWCmd, sizeof(getWCmd) - sizeof(UInt16));

    // Send Req and Get responce
//    byte buf[BSZ];
//    int len = 0;
//    len = can->write((byte*)&getWCmd, sizeof(getWCmd), buf);
//    emit write((byte*)&getWCmd, sizeof(getWCmd));

//    ioDriver.write((byte*)&getWCmd, sizeof(getWCmd));
//    while (!receivedData) {
//    };
//    receivedData = false;

    QByteArray response = ioDriver.writes((byte*)&getWCmd, sizeof(getWCmd));
    received(response);
    if (len == 0)
        return CHECK_CHANNEL_TIME_OUT;

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



// Get voltage (U) by phases
int MercuryClass::getU(int address, P3V* U)
{
    ReadParamCmd getUCmd =
    {
        getUCmd.address = address,
        getUCmd.command = 0x08,
        getUCmd.paramId = 0x16,
        getUCmd.BWRI = 0x11,
        0x00
    };
    getUCmd.CRC = ModRTU_CRC((byte*)&getUCmd, sizeof(getUCmd) - sizeof(UInt16));
    printPackage((byte*)&getUCmd, sizeof(getUCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getUCmd, sizeof(getUCmd), buf);

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
int MercuryClass::getI(int address, P3V* I)
{
    ReadParamCmd getICmd =
    {
        getICmd.address = address,
        getICmd.command = 0x08,
        getICmd.paramId = 0x16,
        getICmd.BWRI = 0x21,
        0x00
    };
    getICmd.CRC = ModRTU_CRC((byte*)&getICmd, sizeof(getICmd) - sizeof(UInt16));
    printPackage((byte*)&getICmd, sizeof(getICmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getICmd, sizeof(getICmd), buf);

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
int MercuryClass::getCosF(int address, P3VS* C)
{
    ReadParamCmd getCosCmd =
    {
        getCosCmd.address = address,
        getCosCmd.command = 0x08,
        getCosCmd.paramId = 0x16,
        getCosCmd.BWRI = 0x30,
        0x00
    };
    getCosCmd.CRC = ModRTU_CRC((byte*)&getCosCmd, sizeof(getCosCmd) - sizeof(UInt16));
    printPackage((byte*)&getCosCmd, sizeof(getCosCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getCosCmd, sizeof(getCosCmd), buf);

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
int MercuryClass::getF(int address, float *f)
{
    ReadParamCmd getFCmd =
    {
        getFCmd.address = address,
        getFCmd.command = 0x08,
        getFCmd.paramId = 0x16,
        getFCmd.BWRI = 0x40,
        0x00
    };
    getFCmd.CRC = ModRTU_CRC((byte*)&getFCmd, sizeof(getFCmd) - sizeof(UInt16));
    printPackage((byte*)&getFCmd, sizeof(getFCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getFCmd, sizeof(getFCmd), buf);

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
int MercuryClass::getA(int address, P3V* A)
{
    ReadParamCmd getACmd =
    {
        getACmd.address = address,
        getACmd.command = 0x08,
        getACmd.paramId = 0x16,
        getACmd.BWRI = 0x51,
        0x00
    };
    getACmd.CRC = ModRTU_CRC((byte*)&getACmd, sizeof(getACmd) - sizeof(UInt16));
    printPackage((byte*)&getACmd, sizeof(getACmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getACmd, sizeof(getACmd), buf);

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
int MercuryClass::getP(int address, P3VS* P)
{
    ReadParamCmd getPCmd =
    {
        getPCmd.address = address,
        getPCmd.command = 0x08,
        getPCmd.paramId = 0x16,
        getPCmd.BWRI = 0x00,
        0x00
    };
    getPCmd.CRC = ModRTU_CRC((byte*)&getPCmd, sizeof(getPCmd) - sizeof(UInt16));
    printPackage((byte*)&getPCmd, sizeof(getPCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getPCmd, sizeof(getPCmd), buf);

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
int MercuryClass::getS(int address, P3VS* S)
{
    ReadParamCmd getSCmd =
    {
        getSCmd.address = address,
        getSCmd.command = 0x08,
        getSCmd.paramId = 0x16,
        getSCmd.BWRI = 0x08,
        0x00
    };
    getSCmd.CRC = ModRTU_CRC((byte*)&getSCmd, sizeof(getSCmd) - sizeof(UInt16));
    printPackage((byte*)&getSCmd, sizeof(getSCmd), OUT);

    // Send Req and Get responce
    byte buf[BSZ];
    int len = 0;
//    len = can->write((byte*)&getSCmd, sizeof(getSCmd), buf);

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

// -- Output formatting and print
void MercuryClass::printOutput(int format, OutputBlock o, int header)
{
    // getting current time for timestamp
    char timeStamp[BSZ];

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

MercuryClass::MercuryClass(quint8 Type, QString server_com, quint16 port_props, quint16 timeout/* = 1000*/) :
    ioDriver(Type, server_com, port_props, timeout)
{
    mutex = new QMutex();

//    connect(this, SIGNAL(write(unsigned char *, int )), &ioDriver, SLOT(write(unsigned char *, int )));
    connect(&ioDriver, SIGNAL(response(QByteArray)), this, SLOT(received(QByteArray)));
    connect(&ioDriver, SIGNAL(timeout()), this, SLOT(timeout()));

//    ioDriver.moveToThread(this);

    header = 0;
    bzero(&o, sizeof(o));
    format = OF_HUMAN;

    receivedData = false;
}
MercuryClass::~MercuryClass()
{
    delete mutex;
}

void MercuryClass::received(QByteArray buf_)
{
    len = buf_.size();
    for (int i = 0; i < len; i++)
    {
        buf[i] = buf_[i];
    }
//    qDebug() << "Receive buf = " << buf_.toHex() << " with size = " << len;
    receivedData = true;
}

void MercuryClass::timeout()
{
    len = 0;
    receivedData = true;
}

Data MercuryClass::read_data(GuidClass* guid, quint8 id)
{
    QMutexLocker locker(mutex);

    QStringList qsl1, qsl2;
    Data retData;
    QString mercury_guid;

    address = guid->get_address(id).toInt();

    mercury_guid = guid->get_guid(id);
//    qDebug() << "Mercury aktiv!" ;

//    qDebug() << "Mercury address = " << address;
    int state = 0;
    switch (state) {
    case 0:
        if (OK != checkChannel(address)) {
            qDebug()<< "Mercury check channel error.";
            break;
        }
    case 1:
        if (OK != initConnection(address)) {
            qDebug()<< "Mercury connection initialisation error.";
            break;
        }
    case 2:
        if (OK != getW(address, &o.PRT[0], PP_RESET, 0, 0+1) ||	// day tariff from reset
            OK != getW(address, &o.PRT[1], PP_RESET, 0, 1+1) )	// night tariff from reset)
        {
            qDebug() << "Mercury get power error.";
            break;
        }
        else
        {
            if ((o.PRT[0].ap >= 0.00) && (o.PRT[1].ap >= 0.00)) {

                char dat[10];
                sprintf(dat, "%.3f", o.PRT[0].ap);//ncluding day tariff (KW)
//                qsl1.append(guid->get_guid(id));
                qsl1.append(mercury_guid);
                qsl1.append(QString::fromStdString(dat));
                qsl1.append(QString::number(DATA_VALUE_FLAG1));

                retData.append(qsl1);

                sprintf(dat, "%.3f", o.PRT[1].ap);//ncluding night tariff (KW)
//                qsl2.append(guid->get_guid(id));
                qsl2.append(mercury_guid);
                qsl2.append(QString::fromStdString(dat));
                qsl2.append(QString::number(DATA_VALUE_FLAG2));

                retData.append(qsl2);

                o_prev = o;
            }
        }
    default:
        break;
    }
    if (OK != closeConnection(address))
        qDebug() << "Mercury connection closing error.";

/*
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
//        if     (OK != getW(address, &o.PR, PP_RESET, 0, 0) ||		// total from reset
//                OK != getW(address, &o.PRT[0], PP_RESET, 0, 0+1) ||	// day tariff from reset
//                OK != getW(address, &o.PRT[1], PP_RESET, 0, 1+1) ||	// night tariff from reset
//                OK != getW(address, &o.PY, PP_YESTERDAY, 0, 0) ||
//                OK != getW(address, &o.PT, PP_TODAY, 0, 0))
//            exitFailure("Cannot collect power counters data.");

//        if (OK != closeConnection(address))
//            exitFailure("Power meter connection closing error.");

//        break;

//    case CHECK_CHANNEL_TIME_OUT :
//        break;

//    default :
//        exitFailure("Power meter communication channel test failed.");
//        connection_error = 1;
//    }
*/
   /*
        //sprintf(str_data[0], "%d", PM_ADDRESS);
//        data = "";
//        data = data + (*guid)[str_id];
//        sprintf(str_data[1], "%.2f", o.U.p1);//Voltage 1(V)
//        sprintf(str_data[2], "%.2f", o.U.p2);//Voltage 2(V)
//        sprintf(str_data[3], "%.2f", o.U.p3);//Voltage 3(V)
//        sprintf(str_data[4], "%.2f", o.I.p1);//Current 1(A)
//        sprintf(str_data[5], "%.2f", o.I.p2);//Current 2(A)
//        sprintf(str_data[6], "%.2f", o.I.p3);//Current 3(A)
//        sprintf(str_data[7], "%.2f", o.C.p1);//Cos(f) 1
//        sprintf(str_data[8], "%.2f", o.C.p2);//Cos(f) 2
//        sprintf(str_data[9], "%.2f", o.C.p3);//Cos(f) 3
//        sprintf(str_data[10], "%.2f", o.C.sum);//Cos(f) sum
//        sprintf(str_data[11], "%.2f", o.f);//Frequency (Hz)
//        sprintf(str_data[12], "%.2f", o.P.p1);//Active power (W) 1
//        sprintf(str_data[13], "%.2f", o.P.p2);//Active power (W) 2
//        sprintf(str_data[14], "%.2f", o.P.p3);//Active power (W) 3
//        sprintf(str_data[15], "%.2f", o.P.sum);//Active power (W) sum
//        sprintf(str_data[16], "%.2f", o.S.p1);//Reactive power (W) 1
//        sprintf(str_data[17], "%.2f", o.S.p2);//Reactive power (W) 2
//        sprintf(str_data[18], "%.2f", o.S.p3);//Reactive power (W) 3
//        sprintf(str_data[19], "%.2f", o.S.sum);//Reactive power (W) sum
//        sprintf(str_data[20], "%.2f", o.PR.ap);//Total consumed, all tariffs (KW)
//        sprintf(str_data[21], "%.3f", o.PRT[0].ap);//ncluding day tariff (KW)
//        sprintf(str_data[22], "%.3f", o.PRT[1].ap);//ncluding night tariff (KW)
//        sprintf(str_data[23], "%.2f", o.PY.ap);//Yesterday consumed (KW)
//        sprintf(str_data[24], "%.2f", o.PT.ap);//Today consumed (KW)
*/

    qDebug() << "Mercury with Address = " << address << "and Guid = " << mercury_guid
             << " Values : " << retData << endl;

    return retData;
}


