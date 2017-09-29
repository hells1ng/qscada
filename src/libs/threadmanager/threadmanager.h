#ifndef THREADMANAGER_H_INCLUDED
#define THREADMANAGER_H_INCLUDED

#include <QtCore/QtCore>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include "../../includes.h"
#include "../../libs/https/https.h"

class ThreadManager : public QObject
{
    Q_OBJECT

public:
    explicit ThreadManager(QObject *parent = 0);
    ~ThreadManager();

    GuidClass       Guid_Mercury_1;
    GuidClass       Guid_Owen_1;
    ModbusClass     Modbus;
    CanClass        Can;

    //------------------OWEN MODULES---------------------------------------//
    OwenClass_16D   Owen_16D_1;
    OwenClass_8A    Owen_8A_11;
    OwenClass_8AC   Owen_8AC_41;
    OwenClass_NL_8R NL_8R_2;

    vector<OwenClass*>  OwenVector;
    OwenClass*          Owen_ptr;
    //------------------Mercury MODULES---------------------------------------//
    MercuryClass    Mercury_1;
    //------------------Pulsar---------------------------------------//
//    PulsarClass Pulse;
    //------------------SQL---------------------------//
    SqlDriver       sqlDriver;
    //------------------HTTPS-------------------------//
    HttpsDriver     httpsDriver;

private:
    QThread *thread1;
    QThread *thread2;
    QThread *thread3;
    QThread *thread4;

    void doEvery(std::function<void()> myFunction, qint64 interval);

    void owen_thread();
    void mercury_thread();
    void sendToServer();
    void deb();


public slots:

    void mercury_slot() {
        doEvery(std::bind(&ThreadManager::mercury_thread, this), 2000);
    }
    void owen_slot() {
        doEvery(std::bind(&ThreadManager::owen_thread, this), 2000);
    }
    void send_slot() {
        doEvery(std::bind(&ThreadManager::sendToServer, this), 1000);
    }
    void debug_slot() {
        doEvery(std::bind(&ThreadManager::deb, this), 1000);
    }

    void getSensorIntervalFromServer();

};


#endif // THREADMANAGER_H_INCLUDED
