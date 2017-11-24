#include "threadmanager.h"
#include <QTextStream>


ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent),
    Modbus(USB1, 115200, 'N', 8, 1, "192.168.88.100", 4006, ModbusClass::TCP),
    Modbus_Sphera(USB1, 9600, 'N', 8, 1, "192.168.88.100", 4005, ModbusClass::TCP),
    Owen_16D_1(1, 16, "RS485"),
    Owen_8A_11(11, 8, "RS485"),
    Owen_8AC_41(41, 8, "RS485"),
    NL_8R_2(2, 8, "RS485"),
//    Mercury_1(IODriver::RTU, USB0, IODriver::Com9600_8N1),
    Mercury_1(IODriver::TCP, "192.168.88.100", 4007),
    Pulsar_1(IODriver::RTU, USB1, IODriver::COM600_8N1),
    Sphera24_1(1, 50, "RS485")
{
    _sensorTimeout = DEFAULT_SENSOR_TIMEOUT;
    _sensorTimeout_mutex = new QMutex();

            /*Init OvenVector*/
    OwenVector.push_back(&Owen_16D_1);
    OwenVector.push_back(&Owen_8A_11);
    //OwenVector.push_back(&Owen_8AC_41); // 8AC pogib
    //OwenVector.push_back(&Owen_KM_56);
    //OwenVector.push_back(&UNO_61);
    //OwenVector.push_back(&NL_8R_2);

    Guid.init(&sqlDriver, QString("mercury_1"), &ID_Mercury_1);
    Guid.init(&sqlDriver, QString("owen_1"), &ID_Owen_1, GuidClass::GUID_TYPE_SUBTABLE);
    Guid.init(&sqlDriver, QString("pulsar_1"), &ID_Pulsar_1);
    Guid.init(&sqlDriver, QString("sphera24_1"), &ID_Sphera24_1, GuidClass::GUID_TYPE_SUBTABLE);

    thread1 = new QThread;
    thread2 = new QThread;
    thread3 = new QThread;
    thread4 = new QThread;
    thread5 = new QThread;
    thread6 = new QThread;

    QObject::connect(thread1, SIGNAL(started()), this, SLOT(mercury_slot()), Qt::DirectConnection);
    QObject::connect(thread2, SIGNAL(started()), this, SLOT(owen_slot()),    Qt::DirectConnection);
    QObject::connect(thread3, SIGNAL(started()), this, SLOT(send_slot()),    Qt::DirectConnection);
    QObject::connect(thread4, SIGNAL(started()), this, SLOT(get_sensor_interval_slot()),   Qt::DirectConnection);
    QObject::connect(thread5, SIGNAL(started()), this, SLOT(pulsar_slot()),   Qt::DirectConnection);
    QObject::connect(thread6, SIGNAL(started()), this, SLOT(sphera_slot()),   Qt::DirectConnection);

    thread1->start();
    thread2->start();
    thread3->start();
    thread4->start();
    thread5->start();
    thread6->start();
}

void ThreadManager::owen_thread()
{
    sqlDriver.push(Owen_16D_1.read_data(&Modbus, &Guid, ID_Owen_1));
    sqlDriver.push(Owen_8A_11.read_data(&Modbus, &Guid, ID_Owen_1));

    //test code for exiting from qScada

//    QTextStream out(stdout);
//    QTextStream in(stdin);
//    while (1)
//    {
//        QString msg;

//        msg = in.readLine();

//        if (msg == QString("stop"))
//        {
//            out << "Exit from qScada ......."<< endl;
//            emit finish();
//        }

//    }

}

void ThreadManager::sphera_thread()
{
    sqlDriver.push(Sphera24_1.read_data(&Modbus_Sphera, &Guid, ID_Sphera24_1));
}

void ThreadManager::mercury_thread()
{
    sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_1));
}

void ThreadManager::pulsar_thread()
{
    sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
}

void ThreadManager::sendToServer()
{
    /* Get data from queue*/
    Data sendData = sqlDriver.pop(100);

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

    sqlDriver.toDataTable(sendData);

    /* Get data from sqlite*/
    Data dataFromTable = sqlDriver.fromDataTable(200);

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

    sqlDriver.toDataTable(dataFromTable);

}

void ThreadManager::getSensorIntervalFromServer()
{
    Data receiveData;
    qint64 new_sensorTimeout;
    httpsDriver.Send( HttpsDriver::HTTPS_CMD_GET_SENSOR_PERIOD, &receiveData );

    if (!receiveData.isEmpty()) {

        QString qs = receiveData[0].at(0);
        new_sensorTimeout = qs.toInt() * 60 * 1000;//msec

        if (_sensorTimeout != new_sensorTimeout) {
            _sensorTimeout_mutex->lock();
            _sensorTimeout = new_sensorTimeout;
//            qDebug() << "Sensor Timeout = " << _sensorTimeout << endl;
            _sensorTimeout_mutex->unlock();
        }
    }
}

ThreadManager::~ThreadManager()
{
    const int dead_delay_ms = 50;
    thread1->quit();
    if(thread1->wait(dead_delay_ms))
    {
        thread1->terminate();
        thread1->wait();
    }
    thread2->quit();
    if(thread2->wait(dead_delay_ms))
    {
        thread2->terminate();
        thread2->wait();
    }
    thread3->quit();
    if(thread3->wait(dead_delay_ms))
    {
        thread3->terminate();
        thread3->wait();
    }
    thread4->quit();
    if(thread4->wait(dead_delay_ms))
    {
        thread4->terminate();
        thread4->wait();
    }
    thread5->quit();
    if(thread5->wait(dead_delay_ms))
    {
        thread5->terminate();
        thread5->wait();
    }
    thread6->quit();
    if(thread6->wait(dead_delay_ms))
    {
        thread6->terminate();
        thread6->wait();
    }

//    delete thread1;
//    delete thread2;
//    delete thread3;
//    delete thread4;
//    delete thread5;
//    delete thread6;
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
/*for sensor Tasks*/
void ThreadManager::doEvery(std::function<void()> myFunction)
{
    QElapsedTimer timer;

    while (1) {

        timer.restart();

        myFunction();

        qint64 time = timer.elapsed();

        _sensorTimeout_mutex->lock();
        qint64 diff = _sensorTimeout - time;
//        qDebug() << "diff = " << diff << endl;
        _sensorTimeout_mutex->unlock();

        if ( diff > 0)
            QThread::msleep(diff);
    }
}

void ThreadManager::deb()
{
    qDebug() << "DEBUG" << endl;
}


