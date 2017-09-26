#ifndef THREADMANAGER_H_INCLUDED
#define THREADMANAGER_H_INCLUDED
//#include <iostream>
//#include <boost/function.hpp>
//#include "boost/date_time/posix_time/posix_time.hpp"
//#include <boost/thread/thread.hpp>
//#include <boost/date_time/date.hpp>
//#include <boost/date_time/gregorian/gregorian.hpp>

#include <QtCore/QtCore>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include "../../includes.h"
#include "../../libs/https/https.h"

/*
using namespace std;
void doEvery(const string& every, boost::function<void()> f, const string& taskname);
void doAtAndEvery(const string& when, const string& interval,
                  boost::function<void()> f, const string& taskname);
*/
class ThreadManager : public QObject
{
    Q_OBJECT

public:
    explicit ThreadManager(QObject *parent = 0);
    ~ThreadManager();

    GuidClass       Guid;
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
    MercuryClass    Mercury_44;
    //------------------Pulsar---------------------------------------//
//    PulsarClass Pulse;
    //------------------SQL---------------------------//
    SqlDriver       sqlDriver;
    //------------------HTTPS-------------------------//
    HttpsDriver     httpsDriver;

//    void addTaskEvery(const string& every, boost::function<void()> f, const string& taskname = "no_name");
//    void addTaskAtAndEvery(const string& when, const string& interval,
//                           boost::function<void()> f, const string& taskname = "no_name");
//    void startAll();

private:
    QTimer *timer1;
    QTimer *timer2;
    QTimer *timer3;
    QTimer *timer4;

public slots:
    void owen_thread();
    void mercury_thread();
    void sendToServer();
    void getSensorIntervalFromServer();

};


#endif // THREADMANAGER_H_INCLUDED
