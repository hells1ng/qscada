#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#define STR_DATA_LINE    25+1
#define STR_DATA_COLUMNS 10+1

#define DELAY_PRE_SEND   500000
#define SENDING          1
#define PRINT_RESULTS    1
#define SIM_MODE         0
#define DELTA            1

#define USB0 "/dev/ttyUSB0"
#define USB1 "/dev/ttyUSB1"
#define USB2 "/dev/ttyUSB1"


#include <QtCore/QtCore>
typedef QVector<QStringList> Data;
enum {
    DATA_VALUE_FLAG0 = 0,
    DATA_VALUE_FLAG1 = 1,
    DATA_VALUE_FLAG2 = 2,
    DATA_ERROR_FLAG0 = 0,
    DATA_ERROR_FLAG1 = 1,
    DATA_ERROR_FLAG2 = 2,
    DATA_POS_GUID    = 0,
    DATA_POS_VALUE   = 1,
    DATA_POS_VALUEFLAG   = 2,
    DATA_POS_TIME           = 3,
    DATA_POS_ERRORFLAG    = 4,
    DATA_NUM_OF_ELEM_START = 3
};

#endif // DEFINES_H_INCLUDED
