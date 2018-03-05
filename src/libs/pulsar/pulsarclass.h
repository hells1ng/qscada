#ifndef PULSARCLASS_H
#define PULSARCLASS_H

#include <QtCore/QtCore>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QObject>
#include "../iodriver/iodriver.h"
#include "../../defines.h"
#include "../id/id.h"


class PulsarClass : public QObject
{
    Q_OBJECT
public:
    explicit PulsarClass(QObject *parent = nullptr);

    typedef enum
    {
        READ_CHANNEL  = 0x01,
        WRITE_CHANNEL = 0x03,
        READ_TIME     = 0x04,
        WRITE_TIME    = 0x05,
        READ_ARCHIVE  = 0x06
        //ADD if you need others
    } CmdEnum;

    typedef enum
    {
        CH1     = 0x01, //сопротивление подачи, Ом (Float32 X.3)
        CH2     = 0x02, // сопротивление обратки, Ом (Float32 X.3)
        CH3     = 0x04, // температура подачи, С (Float32 X.2)
        CH4     = 0x08, // температура обратки, С (Float32 X.2)
        CH5     = 0x10, // перепад температур, С (Float32 X.2)
        CH6     = 0x20, // Мощность, Мкал/ч (Float32 X.3)
        CH7     = 0x40, // Энергия, Мкал (Float32 X.6)
        CH8     = 0x80, // Объем, м^3 (Float32 X.6)
        CH9     = 0x01, // Расход, м^3/ч (Float32 X.3)
        CH10    = 0x02, // Имп вход 1, м^3 (Float32 X.3)
        CH11    = 0x04, // Имп вход 2, м^3 (Float32 X.3)
        CH12    = 0x08 // Температура прибора, С (Float32 X.1)
    } ChEnum;

    typedef enum
    {
        POS_A1   = 0,
        POS_A2   = 1,
        POS_A3   = 2,
        POS_A4   = 3,
        POS_F    = 4,
        POS_L    = 5,
        POS_D    = 6,
    } PosEnum;

    enum
    {
        ID       = 0x55,
        TIMEOUT  = 5000
    };

    IODriver ioDriver;
    PulsarClass(quint8 Type, QString server_com, quint16 port_props, quint16 timeout = TIMEOUT);
    ~PulsarClass();

    Data read_data(GuidClass* guid, quint8 id);

private:
    quint32     address;
    QByteArray  Request;
    QByteArray  Response;
    quint8      Cmd;
    int         len;
    float       energy;
    bool        receivedData;

//    QMutex      mutex;

    void create_request(quint32 channelMask);
    quint16 make_crc(QByteArray *buf, int size);
    bool check_crc();
    bool getP();
    float convert_data();

signals:
    void write(QByteArray req);
public slots:
    void received(QByteArray buf_);
    void timeout();

};

#endif // PULSARCLASS_H
