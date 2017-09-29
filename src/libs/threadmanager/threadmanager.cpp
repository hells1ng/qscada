#include "threadmanager.h"


ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent),
    Guid_Owen_1(GuidClass::TypeOfGuid::GUID_TYPE_SUBTABLE),
    Modbus(USB1, 115200, 'N', 8, 1, "192.168.88.100", 4006, ModbusClass::TCP),
    Can(USB0, 9600, 'N', 8, 1, "192.168.88.100", 4007, CanClass::TCP),
    Owen_16D_1(1, 16, "RS485"),
    Owen_8A_11(11, 8, "RS485"),
    Owen_8AC_41(41, 8, "RS485"),
    NL_8R_2(2, 8, "RS485"),
    Mercury_1(0, "CAN")
//    Pulse(11223344)
{
            /*Init OvenVector*/
    OwenVector.push_back(&Owen_16D_1);
    OwenVector.push_back(&Owen_8A_11);
    //OwenVector.push_back(&Owen_8AC_41); // 8AC pogib
    //OwenVector.push_back(&Owen_KM_56);
    //OwenVector.push_back(&UNO_61);
    //OwenVector.push_back(&NL_8R_2);

    Guid_Mercury_1.init(&sqlDriver, QString("mercury_1"));
    Guid_Owen_1.init(&sqlDriver, QString("owen_1"));

    thread1 = new QThread;
    thread2 = new QThread;
    thread3 = new QThread;
    thread4 = new QThread;

    QObject::connect(thread1, SIGNAL(started()), this, SLOT(mercury_slot()), Qt::DirectConnection);
    QObject::connect(thread2, SIGNAL(started()), this, SLOT(owen_slot()),    Qt::DirectConnection);
    QObject::connect(thread3, SIGNAL(started()), this, SLOT(send_slot()),    Qt::DirectConnection);
//    QObject::connect(thread4, SIGNAL(started()), this, SLOT(debug_slot()),   Qt::DirectConnection);

    thread1->start();
    thread2->start();
    thread3->start();
    thread4->start();
}

ThreadManager::~ThreadManager()
{
    delete thread1;
    delete thread2;
    delete thread3;
    delete thread4;
}

void ThreadManager::doEvery(std::function<void()> myFunction, qint64 interval)
{
    QElapsedTimer timer;

    while (1) {

        timer.restart();

        myFunction();

        qint64 time = timer.elapsed();
        if ( interval > time)
            QThread::msleep(interval - time);
    }
}

void ThreadManager::deb()
{
    qDebug() << "DEBUG" << endl;
}

void ThreadManager::owen_thread()
{

    for (quint16 i = 0; i < Guid_Owen_1.size(); i++) {
        Owen_ptr = OwenVector[i];
        Guid_Owen_1.set_index(i);
        sqlDriver.push(Owen_ptr->read_data(&Modbus, &Guid_Owen_1));
    }

}

void ThreadManager::mercury_thread()
{
    for (quint16 i = 0; i < Guid_Mercury_1.size(); i++) {
        Guid_Mercury_1.set_index(i);
        sqlDriver.push(Mercury_1.read_data(&Can, &Guid_Mercury_1));
    }
}

void ThreadManager::sendToServer()
{
    /* Get data from queue*/
    Data sendData = sqlDriver.pop(10);

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

    sqlDriver.toDataTable(sendData);

    /* Get data from sqlite*/
    Data dataFromTable = sqlDriver.fromDataTable(50);

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

    sqlDriver.toDataTable(dataFromTable);

}

void ThreadManager::getSensorIntervalFromServer()
{
//    httpsDriver.Send( HttpsDriver::GET_SENSOR_PERIOD );
}


