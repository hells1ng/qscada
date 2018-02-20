#include "threadmanager.h"
#include <QTextStream>


ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent),
    Modbus(USB1, 115200, 'N', 8, 1, "192.168.88.100", 4006, ModbusClass::TCP),
    Modbus_Sphera(USB1, 9600, 'N', 8, 1, "192.168.88.100", 4005, ModbusClass::TCP)
//    Owen_16D_1(1, 16, "RS485"),
//    Owen_8A_11(11, 8, "RS485"),
//    Owen_8AC_41(41, 8, "RS485"),
//    NL_8R_2(2, 8, "RS485")
//  Pulsar_1(IODriver::TCP, "192.168.88.100", 4006),
//  Pulsar_2(IODriver::TCP, "192.168.88.100", 4001)
//    Pulsar_1(IODriver::RTU, USB0, IODriver::COM600_8N1),
//    Pulsar_2(IODriver::RTU, USB0, IODriver::COM600_8N1)
//  Pulsar_3(IODriver::TCP, "192.168.88.100", 4004),
//  Pulsar_4(IODriver::TCP, "192.168.88.100", 4003)
{
    Guid = new GuidClass;

    Guid_M1 = new GuidClass;
    Guid_M2 = new GuidClass;
    Guid_M3 = new GuidClass;
    Guid_M4 = new GuidClass;
    Guid_P1 = new GuidClass;
    Guid_P2 = new GuidClass;
    Guid_P3 = new GuidClass;
    Guid_P4 = new GuidClass;

    _sensorTimeout = DEFAULT_SENSOR_TIMEOUT;
    _sensorTimeout_mutex = new QMutex();

            /*Init OvenVector*/
//    OwenVector.push_back(&Owen_16D_1);
//    OwenVector.push_back(&Owen_8A_11);
    //OwenVector.push_back(&Owen_8AC_41); // 8AC pogib
    //OwenVector.push_back(&Owen_KM_56);
    //OwenVector.push_back(&UNO_61);
    //OwenVector.push_back(&NL_8R_2);


    Guid->init(&sqlDriver, QString("mercury_1"), &ID_Mercury_1);
//        Mercury_1               = new MercuryClass(IODriver::RTU, USB1, IODriver::Com9600_8N1);
        Mercury_1               = new MercuryClass(IODriver::TCP, "192.168.88.100", 4007);
//        thread_mercury_1        = new QThread;
//        QObject::connect(thread_mercury_1, SIGNAL(started()), this, SLOT(mercury_1_slot()), Qt::DirectConnection);

    Guid->init(&sqlDriver, QString("mercury_2"), &ID_Mercury_2);
//        Mercury_2               = new MercuryClass(IODriver::RTU, USB0, IODriver::Com9600_8N1);
        Mercury_2               = new MercuryClass(IODriver::TCP, "192.168.88.100", 4000);
//        thread_mercury_2        = new QThread;
//        QObject::connect(thread_mercury_2, SIGNAL(started()), this, SLOT(mercury_2_slot()), Qt::DirectConnection);


    Guid->init(&sqlDriver, QString("pulsar_1"), &ID_Pulsar_1);
        Pulsar_1                = new PulsarClass(IODriver::TCP, "192.168.88.100", 4006);
//                Pulsar_1                = new PulsarClass(IODriver::RTU, USB0, IODriver::COM600_8N1);
//        thread_pulsar_1         = new QThread;
//        QObject::connect(thread_pulsar_1, SIGNAL(started()), this, SLOT(pulsar_1_slot()),   Qt::DirectConnection);

    Guid->init(&sqlDriver, QString("pulsar_2"), &ID_Pulsar_2);
        Pulsar_2                = new PulsarClass(IODriver::TCP, "192.168.88.100", 4001);
//        Pulsar_2                = new PulsarClass(IODriver::RTU, USB0, IODriver::COM600_8N1);
//        thread_pulsar_2         = new QThread;
//        QObject::connect(thread_pulsar_2, SIGNAL(started()), this, SLOT(pulsar_2_slot()),   Qt::DirectConnection);

        Guid->init(&sqlDriver, QString("mercury_3"), &ID_Mercury_3);
        //        Mercury_3               = new MercuryClass(IODriver::RTU, USB1, IODriver::Com9600_8N1);
        Mercury_3               = new MercuryClass(IODriver::TCP, "192.168.88.100", 4005);
        //            thread_mercury_3        = new QThread;
        //            QObject::connect(thread_mercury_3, SIGNAL(started()), this, SLOT(mercury_3_slot()), Qt::DirectConnection);

        Guid->init(&sqlDriver, QString("mercury_4"), &ID_Mercury_4);
        //        Mercury_4               = new MercuryClass(IODriver::RTU, USB0, IODriver::Com9600_8N1);
        Mercury_4               = new MercuryClass(IODriver::TCP, "192.168.88.100", 4002);
        //            thread_mercury_4        = new QThread;
        //            QObject::connect(thread_mercury_4, SIGNAL(started()), this, SLOT(mercury_4_slot()), Qt::DirectConnection);

        Guid->init(&sqlDriver, QString("pulsar_3"), &ID_Pulsar_3);
        Pulsar_3                = new PulsarClass(IODriver::TCP, "192.168.88.100", 4004);
        //        Pulsar_3                = new PulsarClass(IODriver::RTU, USB0, IODriver::COM600_8N1);
        //            thread_pulsar_3         = new QThread;
        //            QObject::connect(thread_pulsar_3, SIGNAL(started()), this, SLOT(pulsar_3_slot()),   Qt::DirectConnection);

        Guid->init(&sqlDriver, QString("pulsar_4"), &ID_Pulsar_4);
        Pulsar_4                = new PulsarClass(IODriver::TCP, "192.168.88.100", 4003);
        //        Pulsar_4                = new PulsarClass(IODriver::RTU, USB0, IODriver::COM600_8N1);
        //            thread_pulsar_4         = new QThread;
        //            QObject::connect(thread_pulsar_4, SIGNAL(started()), this, SLOT(pulsar_4_slot()),   Qt::DirectConnection);

//        thread_mercury_1        = new QThread;
//        QObject::connect(thread_mercury_1, SIGNAL(started()), this, SLOT(mercury_1_slot()), Qt::DirectConnection);
//        thread_mercury_2        = new QThread;
//        QObject::connect(thread_mercury_2, SIGNAL(started()), this, SLOT(mercury_2_slot()), Qt::DirectConnection);
//        thread_pulsar_1         = new QThread;
//        QObject::connect(thread_pulsar_1, SIGNAL(started()), this, SLOT(pulsar_1_slot()),   Qt::DirectConnection);
//        thread_pulsar_2         = new QThread;
//        QObject::connect(thread_pulsar_2, SIGNAL(started()), this, SLOT(pulsar_2_slot()),   Qt::DirectConnection);

//        thread_mercury_3        = new QThread;
//        QObject::connect(thread_mercury_3, SIGNAL(started()), this, SLOT(mercury_3_slot()), Qt::DirectConnection);
//        thread_mercury_4        = new QThread;
//        QObject::connect(thread_mercury_4, SIGNAL(started()), this, SLOT(mercury_4_slot()), Qt::DirectConnection);
//        thread_pulsar_3         = new QThread;
//        QObject::connect(thread_pulsar_3, SIGNAL(started()), this, SLOT(pulsar_3_slot()),   Qt::DirectConnection);
//        thread_pulsar_4         = new QThread;
//        QObject::connect(thread_pulsar_4, SIGNAL(started()), this, SLOT(pulsar_4_slot()),   Qt::DirectConnection);

//        thread_read_all_sensors = new QThread;
//        QObject::connect(thread_read_all_sensors, SIGNAL(started()), this, SLOT(read_all_sensors_slot()), Qt::DirectConnection);


//    Guid.init(&sqlDriver, QString("owen_1"), &ID_Owen_1, GuidClass::GUID_TYPE_SUBTABLE);
//        Owen_16D_1  = new OwenClass_16D(1, 16, "RS485");
//        Owen_8A_11  = new OwenClass_8A(11, 8, "RS485");
//        thread2     = new QThread;
//        QObject::connect(thread2, SIGNAL(started()), this, SLOT(owen_slot()),    Qt::DirectConnection);

//    Guid.init(&sqlDriver, QString("sphera24_1"), &ID_Sphera24_1, GuidClass::GUID_TYPE_SUBTABLE);
//        Sphera24_1  = new Sphera_24CI(1, 50, "RS485");
//        thread4 = new QThread;
//        QObject::connect(thread4, SIGNAL(started()), this, SLOT(sphera_slot()),   Qt::DirectConnection);

//    thread_send = new QThread;
//    QObject::connect(thread_send, SIGNAL(started()), this, SLOT(send_slot()),    Qt::DirectConnection);
//    thread_read = new QThread;
//    QObject::connect(thread_read, SIGNAL(started()), this, SLOT(getInfoFromServer_slot()),   Qt::DirectConnection);
//    thread_queue = new QThread;
//    QObject::connect(thread_queue, SIGNAL(started()), this, SLOT(QueueReqFromServer_slot()),   Qt::DirectConnection);
//    thread_cmdline = new QThread;
//    QObject::connect(thread_cmdline, SIGNAL(started()), this, SLOT(cmdline_slot()),   Qt::DirectConnection);

    //test
//    Guid.addQueue(QString("1e146e46623044b1972078ba22ea6579"));
//    Guid.addQueue(QString("e06ef2480bca4b00a6bc718afc800a99"));
//    Guid.addQueue(QString("FD3F35BF-7881-495C-9E30-7C910352BE83"));
//    Guid.addQueue(QString("b0ac1d32e60e42f9a4beb99100e4b98q"));
//    Guid.addQueue(QString("1111"));
//    qDebug() << "ALL GUID : " << Guid.getAllGuid() << endl;

//    thread_mercury_1->start();
//    thread_mercury_2->start();

//    thread_pulsar_1->start();
//    thread_pulsar_2->start();

//    thread_mercury_3->start();
//    thread_mercury_4->start();

//    thread_pulsar_3->start();
//    thread_pulsar_4->start();

//    thread_read_all_sensors->start();

//    thread2->start();
//    thread4->start();

//    thread_send->start();
//    thread_read->start();
//    thread_queue->start();

//    thread_cmdline->start();


        //timers
//    timer1.setInterval(1000);
//    timer2.setInterval(1000);
////    QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(slot1()));

//    QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(slot1()),Qt::DirectConnection);
//    QObject::connect(&timer2, SIGNAL(timeout()), this, SLOT(slot2()),Qt::DirectConnection);
////    QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(mercury_3_slot()), Qt::DirectConnection);
////    QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(mercury_4_slot()), Qt::DirectConnection);

//    timer1.start();
//    timer2.start();
}

void ThreadManager::slot1()
{
    while (Guid->hasNext(ID_Mercury_1))
        sqlDriver.push(Mercury_1->read_data(Guid, ID_Mercury_1));

}
void ThreadManager::slot2()
{
    while (Guid->hasNext(ID_Mercury_2))
        sqlDriver.push(Mercury_2->read_data(Guid, ID_Mercury_2));
}

void ThreadManager::read_all_sensors()
{
    while (Guid->hasNext(ID_Mercury_1))
        sqlDriver.push(Mercury_1->read_data(Guid, ID_Mercury_1));
    while (Guid->hasNext(ID_Mercury_2))
        sqlDriver.push(Mercury_2->read_data(Guid, ID_Mercury_2));
    while (Guid->hasNext(ID_Mercury_3))
        sqlDriver.push(Mercury_3->read_data(Guid, ID_Mercury_3));
    while (Guid->hasNext(ID_Mercury_4))
        sqlDriver.push(Mercury_4->read_data(Guid, ID_Mercury_4));

    while (Guid->hasNext(ID_Pulsar_1))
        sqlDriver.push(Pulsar_1->read_data(Guid, ID_Pulsar_1));
    while (Guid->hasNext(ID_Pulsar_2))
        sqlDriver.push(Pulsar_2->read_data(Guid, ID_Pulsar_2));
    while (Guid->hasNext(ID_Pulsar_3))
        sqlDriver.push(Pulsar_3->read_data(Guid, ID_Pulsar_3));
    while (Guid->hasNext(ID_Pulsar_4))
        sqlDriver.push(Pulsar_4->read_data(Guid, ID_Pulsar_4));

}
void ThreadManager::mercury_1_thread()
{
    while (Guid->hasNext(ID_Mercury_1))
        sqlDriver.push(Mercury_1->read_data(Guid, ID_Mercury_1));

}
void ThreadManager::mercury_2_thread()
{
    while (Guid->hasNext(ID_Mercury_2))
        sqlDriver.push(Mercury_2->read_data(Guid, ID_Mercury_2));
}

void ThreadManager::pulsar_1_thread()
{
    while (Guid->hasNext(ID_Pulsar_1))
        sqlDriver.push(Pulsar_1->read_data(Guid, ID_Pulsar_1));
}

void ThreadManager::pulsar_2_thread()
{
    while (Guid->hasNext(ID_Pulsar_2))
        sqlDriver.push(Pulsar_2->read_data(Guid, ID_Pulsar_2));
}

void ThreadManager::mercury_3_thread()
{
    while (Guid->hasNext(ID_Mercury_3))
        sqlDriver.push(Mercury_3->read_data(Guid, ID_Mercury_3));

}
void ThreadManager::mercury_4_thread()
{
    while (Guid->hasNext(ID_Mercury_4))
        sqlDriver.push(Mercury_4->read_data(Guid, ID_Mercury_4));

}

void ThreadManager::pulsar_3_thread()
{
    while (Guid->hasNext(ID_Pulsar_3))
        sqlDriver.push(Pulsar_3->read_data(Guid, ID_Pulsar_3));
}

void ThreadManager::pulsar_4_thread()
{
    while (Guid->hasNext(ID_Pulsar_4))
        sqlDriver.push(Pulsar_4->read_data(Guid, ID_Pulsar_4));
}


void ThreadManager::owen_thread()
{
    sqlDriver.push(Owen_16D_1->read_data(&Modbus, Guid, ID_Owen_1));
    sqlDriver.push(Owen_8A_11->read_data(&Modbus, Guid, ID_Owen_1));
}

void ThreadManager::sphera_thread()
{
    while (Guid->hasNext(ID_Sphera24_1))
        sqlDriver.push(Sphera24_1->read_data(&Modbus_Sphera, Guid, ID_Sphera24_1));
}



void ThreadManager::sendToServer()
{
    /* Get data from queue*/
    Data sendData = sqlDriver.pop(100);
//    qDebug()<<"size = " << sendData.size();

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

//    sqlDriver.toDataTable(sendData);

    /* Get data from sqlite*/
//    Data dataFromTable = sqlDriver.fromDataTable(200);

//    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

//    sqlDriver.toDataTable(dataFromTable);

}

void ThreadManager::getInfoFromServer_thread()
{
    Data receiveData;
    qint64 new_sensorTimeout;

    /* Get sensor interval from server*/
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

        receiveData.clear();
    }

    /*Get Request from server */
    receiveData.append(Guid->getAllGuid());

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_GET_SENSOR_REQUESTS, &receiveData);

    //в 0 стринглисте все гуайдишники малины, в 1 - гуайдишники полученные на сервере
    if (receiveData.size() == 2)
        Guid->addQueue(receiveData.at(1));

}


void ThreadManager::QueueReqFromServer_thread()
{
    if (!Guid->isEmptyQueue(ID_Mercury_1))
        sqlDriver.push(Mercury_1->read_data(Guid, ID_Mercury_1));
    if (!Guid->isEmptyQueue(ID_Mercury_2))
        sqlDriver.push(Mercury_2->read_data(Guid, ID_Mercury_2));

//    if (!Guid.isEmptyQueue(ID_Pulsar_1))
//        sqlDriver.push(Pulsar_1->read_data(&Guid, ID_Pulsar_1));
//    if (!Guid.isEmptyQueue(ID_Pulsar_2))
//        sqlDriver.push(Pulsar_2->read_data(&Guid, ID_Pulsar_2));

    //    if (!Guid.isEmptyQueue(ID_Mercury_3))
    //        sqlDriver.push(Mercury_3->read_data(&Guid, ID_Mercury_3));
    //    if (!Guid.isEmptyQueue(ID_Mercury_4))
    //        sqlDriver.push(Mercury_4->read_data(&Guid, ID_Mercury_4));

    //    if (!Guid.isEmptyQueue(ID_Pulsar_3))
    //        sqlDriver.push(Pulsar_3->read_data(&Guid, ID_Pulsar_3));
    //    if (!Guid.isEmptyQueue(ID_Pulsar_4))
    //        sqlDriver.push(Pulsar_4->read_data(&Guid, ID_Pulsar_4));

//    if (!Guid.isEmptyQueue(ID_Sphera24_1))
//        sqlDriver.push(Sphera24_1->read_data(&Modbus_Sphera, &Guid, ID_Sphera24_1));
}

void ThreadManager::cmdline_thread()
{
    QTextStream out(stdout);
    QTextStream in(stdin);
    while (1)
    {
        QString msg;

        msg = in.readLine();

        if (msg == QString("stop"))
        {
            out << "Exit from qScada ......."<< endl;
            emit finish();
        }
//        else if (msg == QString("1")) //mercury
//            Guid.addQueue("1e146e46623044b1972078ba22ea6579");

//        else if (msg == QString("2")) //sphera
//            Guid.addQueue(QString("b0ac1d32e60e42f9a4beb99100e4b98q"));

//        else if (msg == QString("3")) //pulsar
//            Guid.addQueue(QString("FD3F35BF-7881-495C-9E30-7C910352BE83"));

//        else if (msg == QString("4")) //sphera
//        {
//            QStringList qsl;
//            qsl << "b0ac1d32e60e42f9a4beb99100e4b98a" << "b0ac1d32e60e42f9a4beb99100e4b98q" << "b0ac1d32e60e42f9a4beb99100e4b98q";
//            Guid.addQueue(qsl);
//        }
//        else if (msg == QString("5")) //mercury
//        {
//            QStringList qsl;
//            qsl << "1e146e46623044b1972078ba22ea6579" << "1e146e46623044b1972078ba22ea6579";
//            Guid.addQueue(qsl);
//        }
        else
            Guid->addQueue(msg);

    }
}

ThreadManager::~ThreadManager()
{
    const int dead_delay_ms = 50;
    //TODO разобраться с правильным выходом из тредов
//    thread_mercury_1->quit();
//    if(thread_mercury_1->wait(dead_delay_ms))
//    {
//        thread_mercury_1->terminate();
//        thread_mercury_1->wait();
//    }
    thread_pulsar_1->quit();
    if(thread_pulsar_1->wait(dead_delay_ms))
    {
        thread_pulsar_1->terminate();
        thread_pulsar_1->wait();
    }


//    thread2->quit();
//    if(thread2->wait(dead_delay_ms))
//    {
//        thread2->terminate();
//        thread2->wait();
//    }

//    thread4->quit();
//    if(thread4->wait(dead_delay_ms))
//    {
//        thread4->terminate();
//        thread4->wait();
//    }
//    thread_send->quit();
//    if(thread_send->wait(dead_delay_ms))
//    {
//        thread_send->terminate();
//        thread_send->wait();
//    }
//    thread_read->quit();
//    if(thread_read->wait(dead_delay_ms))
//    {
//        thread_read->terminate();
//        thread_read->wait();
//    }

//    delete thread_mercury_1;
//    delete thread2;
//    delete thread_pulsar_1;
//    delete thread4;
//    delete thread_send;
//    delete thread_read;
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


