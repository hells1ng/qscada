#ifndef PULSARCLASS_H
#define PULSARCLASS_H

#include <QtCore/QtCore>
//#include <QDebug>
//#include <QObject>
//#include <QtGlobal>

#define PULSAR_READ_CMD     0x01
#define PULSAR_ENERGY_CH    (quint8)(7<<1)


class PulsarClass : public QObject
{
//    Q_OBJECT
public:
    explicit PulsarClass(QObject *parent = 0);
    PulsarClass(quint32 address_) :
        address(address_)
    {
//        qDebug() << address << endl;
    }
    // Energy meter parameters read command
    typedef struct
    {
        quint32     address; // HiLo
        quint8      command;
        quint8      lenght;
        quint32     data_in;
        quint16     ID;
        quint16     CRC; // LoHi
    } ReadParamCmd;
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

private:
    quint32     address;
    QByteArray  Request;
    QByteArray  Response;
    float       energy;

    void create_request(const quint8 CMD, quint8 numOfData);
signals:

public slots:

};

#endif // PULSARCLASS_H
