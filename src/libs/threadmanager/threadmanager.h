#ifndef THREADMANAGER_H_INCLUDED
#define THREADMANAGER_H_INCLUDED

#include <QtCore/QtCore>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include "../../includes.h"
#include "../../libs/https/https.h"
#include "../../libs/pulsar/pulsarclass.h"

class ThreadManager : public QObject
{
    Q_OBJECT

public:
    explicit ThreadManager(QObject *parent = 0);
    ~ThreadManager();
    enum {
      DEFAULT_SENSOR_TIMEOUT = 60 * 1000 // 1 min
    };
    ModbusClass     Modbus;
    ModbusClass     Modbus_Sphera;

    GuidClass       Guid;
    quint8          ID_Mercury_1,
                    ID_Owen_1,
                    ID_Pulsar_1,
                    ID_Sphera24_1;

    //------------------OWEN MODULES---------------------------------------//
    OwenClass_16D*   Owen_16D_1;
    OwenClass_8A*    Owen_8A_11;
//    OwenClass_8AC   Owen_8AC_41;
//    OwenClass_NL_8R NL_8R_2;

//    vector<OwenClass*>  OwenVector;
//    OwenClass*          Owen_ptr;
    //------------------Mercury MODULES---------------------------------------//
    MercuryClass*    Mercury_1;
    //------------------Pulsar---------------------------------------//
    PulsarClass*     Pulsar_1;
    //------------------Sphera MODULES-------------------------------//
    Sphera_24CI*     Sphera24_1;
    //------------------SQL---------------------------//
    SqlDriver       sqlDriver;
    //------------------HTTPS-------------------------//
    HttpsDriver     httpsDriver;

private:
    QThread *thread1;
    QThread *thread2;
    QThread *thread3;
    QThread *thread4;
    QThread *thread_send;
    QThread *thread_read;
    QThread *thread_queue;
    QThread *thread_cmdline;

    void doEvery(std::function<void()> myFunction, qint64 interval);
    void doEvery(std::function<void()> myFunction);
    void owen_thread();
    void mercury_thread();
    void pulsar_thread();
    void sendToServer();
    void deb();
    void getInfoFromServer_thread();
    void sphera_thread();
    void QueueReqFromServer_thread();
    void cmdline_thread();

    qint64 _sensorTimeout;
    QMutex*  _sensorTimeout_mutex;

signals:
    void finish();
public slots:

    void mercury_slot() {
        doEvery(std::bind(&ThreadManager::mercury_thread, this)/*, 5000*/);
    }
    void owen_slot() {
        doEvery(std::bind(&ThreadManager::owen_thread, this), 30000);
    }
    void pulsar_slot() {
        doEvery(std::bind(&ThreadManager::pulsar_thread, this)/*, 5000*/);
    }
    void send_slot() {
        doEvery(std::bind(&ThreadManager::sendToServer, this), 1000);
    }
    void getInfoFromServer_slot() {
        doEvery(std::bind(&ThreadManager::getInfoFromServer_thread, this), 5000);
    }
    void sphera_slot() {
        doEvery(std::bind(&ThreadManager::sphera_thread, this)/*, 5000*/);
    }

    void QueueReqFromServer_slot() {
        doEvery(std::bind(&ThreadManager::QueueReqFromServer_thread, this), 1000);
    }
    void cmdline_slot() {
            doEvery(std::bind(&ThreadManager::cmdline_thread, this), 1000);
    }
};

#endif // THREADMANAGER_H_INCLUDED
