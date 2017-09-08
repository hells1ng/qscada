#include "networkclass.h"
#include <QDebug>

//NetworkClass::NetworkClass(QObject *parent) :
//    QObject(parent)
//{
//}
NetworkClass::~NetworkClass()
{
}
int NetworkClass::write( unsigned char * cmd, int cmdsize, unsigned char * buf)
{
    QTcpSocket  socket;
    Request = QByteArray((char*)cmd, cmdsize);
    QByteArray responseData;

    socket.connectToHost(ServerName, ServerPort);
    if (!socket.waitForConnected(Timeout))
    {
        //emit error(tr("Can't connect to %1, error code %2")
                  //.arg(serverName).arg(socket.error()));

//          return;
    }
    else
    {
        socket.write(Request);

       // qDebug() << "REQUEST = " << Request.toHex();

        if (socket.waitForBytesWritten(Timeout)) {

            //! [8] //! [10]
            // read response
            if (socket.waitForReadyRead(Timeout))//Если получен ответ в течение зад.времени
            {
                responseData = socket.readAll();
                //qDebug() << "RESPONSE = " << responseData.toHex();
                for (int i = 0; i < responseData.size(); i++)
                {
                    buf[i] = responseData[i];
                }
            }
            else    //а если ничего не пришло
            {
                //            emit this->response_fail(QString (addr.toHex()) + ":" + failstr + ":Timeout FAIL");
                //            emit timeout(tr("Wait read response timeout %1")
                //                         .arg(QTime::currentTime().toString()));
                //            return RESPONSE_ERROR;//если нет ответа в течение таймаута
            }
            //! [9] //! [11]
        } else {
            //        emit timeout(tr("Wait write request timeout %1")
            //                     .arg(QTime::currentTime().toString()));
        }
        //    return RESPONSE_OK;

    }
    socket.disconnectFromHost();
    Request.clear();

    return responseData.size();

}
