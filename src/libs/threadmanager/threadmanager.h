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
      DEFAULT_SENSOR_TIMEOUT = 3600 * 1000 // 60 min
    };
    ModbusClass     Modbus;
    ModbusClass     Modbus_Sphera;

    GuidClass*       Guid;
    GuidClass*       Guid_M1;
    GuidClass*       Guid_M2;
    GuidClass*       Guid_M3;
    GuidClass*       Guid_M4;
    GuidClass*       Guid_P1;
    GuidClass*       Guid_P2;
    GuidClass*       Guid_P3;
    GuidClass*       Guid_P4;
    quint8          ID_Mercury_1,
                    ID_Mercury_2,
                    ID_Pulsar_1,
                    ID_Pulsar_2,
                    ID_Mercury_3,
                    ID_Mercury_4,
                    ID_Pulsar_3,
                    ID_Pulsar_4,
                    ID_Owen_1,
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
    MercuryClass*    Mercury_2;
    MercuryClass*    Mercury_3;
    MercuryClass*    Mercury_4;
    //------------------Pulsar---------------------------------------//
    PulsarClass*     Pulsar_1;
    PulsarClass*     Pulsar_2;
    PulsarClass*     Pulsar_3;
    PulsarClass*     Pulsar_4;
    //------------------Sphera MODULES-------------------------------//
    Sphera_24CI*     Sphera24_1;
    //------------------SQL---------------------------//
    SqlDriver       sqlDriver;
    SqlDriver       sqlDriverTEST, sqlDriverTEST3;
    //------------------HTTPS-------------------------//
    HttpsDriver     httpsDriver;
    //------------TIMERS-----------//
    QTimer timer1;
    QTimer timer2;
    QTimer timer3;
    QTimer timer4;

private:
    QThread *thread_mercury_1;
    void mercury_1_thread();

    QThread *thread_mercury_2;
    void mercury_2_thread();

    QThread *thread_pulsar_1;
    void pulsar_1_thread();

    QThread *thread_pulsar_2;
    void pulsar_2_thread();

    QThread *thread_mercury_3;
    void mercury_3_thread();

    QThread *thread_mercury_4;
    void mercury_4_thread();

    QThread *thread_pulsar_3;
    void pulsar_3_thread();

    QThread *thread_pulsar_4;
    void pulsar_4_thread();

    QThread *thread_read_all_sensors;
    void read_all_sensors();

    QThread *thread2;
    QThread *thread4;
    QThread *thread_send;
    QThread *thread_read;
    QThread *thread_queue;
    QThread *thread_cmdline;

    void doEvery(std::function<void()> myFunction, qint64 interval);
    void doEvery(std::function<void()> myFunction);
    void owen_thread();
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

    void mercury_1_slot() {
        doEvery(std::bind(&ThreadManager::mercury_1_thread, this)/*, 5000*/);
    }
    void mercury_2_slot() {
        doEvery(std::bind(&ThreadManager::mercury_2_thread, this)/*, 5000*/);
    }

    void pulsar_1_slot() {
        doEvery(std::bind(&ThreadManager::pulsar_1_thread, this)/*, 5000*/);
    }
    void pulsar_2_slot() {
        doEvery(std::bind(&ThreadManager::pulsar_2_thread, this)/*, 5000*/);
    }

    void mercury_3_slot() {
        doEvery(std::bind(&ThreadManager::mercury_3_thread, this)/*, 5000*/);
    }
    void mercury_4_slot() {
        doEvery(std::bind(&ThreadManager::mercury_4_thread, this)/*, 5000*/);
    }

    void pulsar_3_slot() {
        doEvery(std::bind(&ThreadManager::pulsar_3_thread, this)/*, 5000*/);
    }
    void pulsar_4_slot() {
        doEvery(std::bind(&ThreadManager::pulsar_4_thread, this)/*, 5000*/);
    }
    void read_all_sensors_slot() {
        doEvery(std::bind(&ThreadManager::read_all_sensors, this)/*, 5000*/);
    }

    void owen_slot() {
        doEvery(std::bind(&ThreadManager::owen_thread, this), 30000);
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

    void slot1();
    void slot2();
};

#endif // THREADMANAGER_H_INCLUDED
