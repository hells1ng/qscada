#ifndef NETWORKCLASS_H
#define NETWORKCLASS_H

#include <QObject>
#include <QtNetwork>


class NetworkClass : public QThread/*QObject*/
{
    Q_OBJECT
public:
    explicit NetworkClass(QThread *parent = 0);

    NetworkClass(QString server, quint16 port, quint16 timeout = 1000) : ServerName(server), ServerPort(port), Timeout(timeout) {
    }
    ~NetworkClass();
    QString     ServerName;
    //QTcpSocket  socket;
    quint16     ServerPort;
    QByteArray  Request;
    quint16     Timeout;

//    NetworkClass operator=(const NetworkClass& v)//перегрузка
//    {
//      ServerName=v.ServerName;
//      ServerPort=v.ServerPort;
//      return *this;//возвращаем ссылку на текущий объект
//    }
//    NetworkClass operator = (NetworkClass v1)
//    {
//        return NetworkClass(this->ServerName = v1.ServerName, this->ServerPort = v1.ServerPort);
//    }
    int write( unsigned char * cmd, int cmdsize, unsigned char * buf);

private:
signals:

public slots:

};

#endif // NETWORKCLASS_H
