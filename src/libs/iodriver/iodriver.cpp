#include "iodriver.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtSerialPort/QSerialPort>

//IODriver::IODriver(QObject *parent) : QThread(parent)
//{
//}

IODriver::IODriver(quint8 type, QString server_com, quint16 port_props, quint16 timeout/* = 1000*/) :
    Type(type), ServerCom(server_com), PortProps(port_props), Timeout(timeout)
{
    mutex = new QMutex();
    debug = false;

    if (Type == RTU) {
        com = new QSerialPort();
        com->moveToThread(this);

        switch (port_props) {

        case Com9600_8N1:
            com->setBaudRate(9600);
            com->setParity(QSerialPort::Parity::NoParity);
            com->setStopBits(QSerialPort::StopBits::OneStop);
            break;

        case COM600_8N1:
            com->setBaudRate(600);
            com->setParity(QSerialPort::Parity::NoParity);
            com->setStopBits(QSerialPort::StopBits::OneStop);
            break;

        default:
            qFatal("Wrong Com-properties for IODriver");
            break;
        }
    }
    else if (Type == TCP){
        tcp = new QTcpSocket();
        tcp->moveToThread(this);
    }
    else qFatal("Wrong Type for IODriver");
}

IODriver::~IODriver()
{
    delete mutex;
}


void IODriver::write(unsigned char * cmd, int cmdsize)
{
    QMutexLocker locker(mutex);
    Request = QByteArray((char*)cmd, cmdsize);
    while (isRunning());
    start();
}

void IODriver::write(QByteArray request)
{
    QMutexLocker locker(mutex);
    Request.append(request);
    while (isRunning());
        start();
}

void IODriver::run()
{
    QByteArray responseData;

    if (debug)
        qDebug() << "Request = " << Request.toHex();

    if (Type == TCP)
    {
        tcp->connectToHost(ServerCom, PortProps);

        if (!tcp->waitForConnected(Timeout + TCP_ADD_TIMEOUT * 5))
        {
            emit timeout();
            qDebug() << "Can't connect to " << ServerCom << " error code " << tcp->error();
        }
        else
        {
            tcp->write(Request);
            Request.clear();

            if (tcp->waitForBytesWritten(Timeout + TCP_ADD_TIMEOUT)) {

                // read response
                if (tcp->waitForReadyRead(Timeout + TCP_ADD_TIMEOUT))//Если получен ответ в течение зад.времени
                {
                    responseData = tcp->readAll();

                    if (debug)
                        qDebug() << "Response = " << responseData.toHex();

                    emit response(responseData);
                }
                else    //а если ничего не пришло
                {
                    emit timeout();
                }
            } else {
                emit timeout();
            }

            tcp->disconnectFromHost();
        }
    }
    else if (Type == RTU)
    {
        com->setPortName(ServerCom);

        if (!com->open(QIODevice::ReadWrite)) {
            qDebug() << "Can't open " << ServerCom << " error code " << com->error();
            emit timeout();
        }
        else //if opened
        {
            com->write(Request);
            Request.clear();

            if (com->waitForBytesWritten(Timeout)) {
                // read response
                if (com->waitForReadyRead(Timeout)) {
                    responseData = com->readAll();
                    while (com->waitForReadyRead(50)) // was 10
                        responseData += com->readAll();

                    if (debug)
                        qDebug() << "Response = " << responseData.toHex();

                    emit response(responseData);

                } else {
                    emit timeout();
                    //                emit timeout(tr("Wait read response timeout %1")
                    //                             .arg(QTime::currentTime().toString()));
                }
            } else {
                emit timeout();
                //            emit timeout(tr("Wait write request timeout %1")
                //                         .arg(QTime::currentTime().toString()));
            }
//            if (com->isOpen())
                com->close();
        }
    }
    quit();
}

