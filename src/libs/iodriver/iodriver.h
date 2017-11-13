#ifndef IODRIVER_H
#define IODRIVER_H

#include <QtCore/QtCore>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtSerialPort/QSerialPort>

class IODriver : public QThread
{
    Q_OBJECT
public:
    explicit IODriver(QObject *parent = nullptr);
    enum {
        RTU,
        UDP,//not support yet
        TCP,
        TIMEOUT = 500,
        TCP_ADD_TIMEOUT = 100,
    };
    enum {
      //add com-propertis if you need others
      Com9600_8N1,
      COM600_8N1
    };
    quint8      Type;
    QString     ServerCom; //name of server ip / name of com-port
    QTcpSocket  *tcp;
    QSerialPort *com;
    quint16     PortProps; //port number / properties of com-port
    QByteArray  Request;
    quint16     Timeout;

    IODriver(quint8 type, QString server_com, quint16 port_props, quint16 timeout = TIMEOUT);

//    int write(unsigned char * cmd, int cmdsize, unsigned char * buf);
signals:
    void response(QByteArray buf);
    void timeout();
public slots:
    void write(unsigned char * cmd, int cmdsize);
    void write(QByteArray req);

private:
    QMutex* mutex;
    bool debug;

    void write_(QByteArray Request);
};

#endif // IODRIVER_H
