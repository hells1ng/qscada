#include "main.h"
#include <QtConcurrent/QtConcurrent>

GuidClass       Guid;

quint8          ID_Mercury_1,
                ID_Mercury_2,
                ID_Mercury_3,
                ID_Mercury_4,
                ID_Pulsar_1,
                ID_Pulsar_2,
                ID_Pulsar_3,
                ID_Pulsar_4;

MercuryClass    Mercury_1(IODriver::TCP, "192.168.88.100", 4007),
                Mercury_2(IODriver::TCP, "192.168.88.100", 4000),
                Mercury_3(IODriver::TCP, "192.168.88.100", 4005),
                Mercury_4(IODriver::TCP, "192.168.88.100", 4002);

PulsarClass     Pulsar_1(IODriver::TCP, "192.168.88.100", 4006),
                Pulsar_2(IODriver::TCP, "192.168.88.100", 4001),
                Pulsar_3(IODriver::TCP, "192.168.88.100", 4004),
                Pulsar_4(IODriver::TCP, "192.168.88.100", 4003);

SqlDriver       sqlDriver;
HttpsDriver     httpsDriver;

qint64  interval = 60*1000*30; //30 min
QMutex  IntervalMutex;
QMutex  Pulsar_1_mutex, Pulsar_2_mutex, Pulsar_3_mutex, Pulsar_4_mutex;
QMutex  Mercury_1_mutex, Mercury_2_mutex, Mercury_3_mutex, Mercury_4_mutex;


void sendToServer()
{
    /* Get data from queue*/
    if (SEND)
    {
        Data sendData = sqlDriver.pop(200);
        qDebug()<<"Size of sensors for sending from queue = " << sendData.size();

        httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

        sqlDriver.toDataTable(sendData);

        /* Get data from sqlite*/
        Data dataFromTable = sqlDriver.fromDataTable(500);
        qDebug()<<"Size of sensors for sending from base = " << dataFromTable.size();

        httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

        sqlDriver.toDataTable(dataFromTable);

//        QThread::msleep(1000);
    }
}

void getInfoFromServer()
{
    Data receiveData;
    qint64 new_interval;

    /* Get sensor interval from server*/
    if (SEND)
        httpsDriver.Send( HttpsDriver::HTTPS_CMD_GET_SENSOR_PERIOD, &receiveData );

    if (!receiveData.isEmpty()) {

        QString qs = receiveData[0].at(0);
        new_interval = qs.toInt() * 60 * 1000;//msec

        if (interval != new_interval ) {
            IntervalMutex.lock();
            interval = new_interval;
            qDebug() << "Sensor new Interval  = " << interval << endl;
            IntervalMutex.unlock();
        }

        receiveData.clear();
    }

    /*Get Request from server */
    receiveData.append(Guid.getAllGuid());

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_GET_SENSOR_REQUESTS, &receiveData);

    //в 0 стринглисте все гуайдишники малины, в 1 - гуайдишники полученные на сервере
    if (receiveData.size() == 2)
        Guid.addQueue(receiveData.at(1));

}
void time_out(QString threadname, QElapsedTimer * timer,  qint64 interval_, qint64 time)
{
    while ( interval_ > time)
    {
        qDebug()<< threadname <<" thread sleep for " << interval_ - time << " msec" << endl;
        QThread::msleep(10000);//10sec

        IntervalMutex.lock();
        interval_ = interval;
        IntervalMutex.unlock();
        time = timer->elapsed();
    }
}

void read_all_sensors()
{
    QElapsedTimer timer;
    while (1)
    {
        timer.restart();

        getInfoFromServer();
        sendToServer();

        while (Guid.hasNext(ID_Mercury_1))
            sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_1));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_2))
            sqlDriver.push(Mercury_2.read_data(&Guid, ID_Mercury_2));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_3))
            sqlDriver.push(Mercury_3.read_data(&Guid, ID_Mercury_3));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_4))
            sqlDriver.push(Mercury_4.read_data(&Guid, ID_Mercury_4));
        sendToServer();

        while (Guid.hasNext(ID_Pulsar_1))
            sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_2))
            sqlDriver.push(Pulsar_2.read_data(&Guid, ID_Pulsar_2));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_3))
            sqlDriver.push(Pulsar_3.read_data(&Guid, ID_Pulsar_3));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_4))
            sqlDriver.push(Pulsar_4.read_data(&Guid, ID_Pulsar_4));
        sendToServer();


        qint64 time = timer.elapsed();

        IntervalMutex.lock();
        qint64 interval_ = interval;
        IntervalMutex.unlock();

        time_out("Read All sensors", &timer, interval_, time);

    }
}
void server_connection()
{
    while (1)
    {
        getInfoFromServer();
        sendToServer();
        QThread::msleep(5000);//wait 5 sec
    }
}
void read_mercury()
{
    QElapsedTimer timer;
    while (1)
    {
        timer.restart();

        if (READ_MERCURY1)
            while (Guid.hasNext(ID_Mercury_1)) {
                Mercury_1_mutex.lock();
                sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_1));
                Mercury_1_mutex.unlock();
            }
        if (READ_MERCURY2)
            while (Guid.hasNext(ID_Mercury_2)) {
                Mercury_2_mutex.lock();
                sqlDriver.push(Mercury_2.read_data(&Guid, ID_Mercury_2));
                Mercury_2_mutex.unlock();
            }
        if (READ_MERCURY3)
            while (Guid.hasNext(ID_Mercury_3)) {
                Mercury_3_mutex.lock();
                sqlDriver.push(Mercury_3.read_data(&Guid, ID_Mercury_3));
                Mercury_3_mutex.unlock();
            }
        if (READ_MERCURY4)
            while (Guid.hasNext(ID_Mercury_4)) {
                Mercury_4_mutex.lock();
                sqlDriver.push(Mercury_4.read_data(&Guid, ID_Mercury_4));
                Mercury_4_mutex.unlock();
            }

        qint64 time = timer.elapsed();

        IntervalMutex.lock();
        qint64 interval_ = interval;
        IntervalMutex.unlock();

        time_out("Mercury", &timer, interval_, time);
    }
}

void read_pulsar()
{
    QElapsedTimer timer;
    while (1)
    {
        timer.restart();

        if (READ_PULSAR1)
            while (Guid.hasNext(ID_Pulsar_1))
            {
                Pulsar_1_mutex.lock();
                sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
                Pulsar_1_mutex.unlock();
            }
        if (READ_PULSAR2)
            while (Guid.hasNext(ID_Pulsar_2))
            {
                Pulsar_2_mutex.lock();
                sqlDriver.push(Pulsar_2.read_data(&Guid, ID_Pulsar_2));
                Pulsar_2_mutex.unlock();
            }
        if (READ_PULSAR3)
            while (Guid.hasNext(ID_Pulsar_3))
            {
                Pulsar_3_mutex.lock();
                sqlDriver.push(Pulsar_3.read_data(&Guid, ID_Pulsar_3));
                Pulsar_3_mutex.unlock();
            }
        if (READ_PULSAR4)
            while (Guid.hasNext(ID_Pulsar_4))
            {
                Pulsar_4_mutex.lock();
                sqlDriver.push(Pulsar_4.read_data(&Guid, ID_Pulsar_4));
                Pulsar_4_mutex.unlock();
            }

        qint64 time = timer.elapsed();

        IntervalMutex.lock();
        qint64 interval_ = interval;
        IntervalMutex.unlock();

        time_out("Pulsar", &timer, interval_, time);
    }
}
/*
 * func for reading 1 line of Pulsar
 * */
void read_pulsar(bool enable, QString threadname, PulsarClass * Pulsar, quint8 id, QMutex * mutex)
{
    QElapsedTimer timer;
    while (1)
    {
        timer.restart();

        if (enable)
            while (Guid.hasNext(id))
            {
                mutex->lock();
                sqlDriver.push(Pulsar->read_data(&Guid, id));
                mutex->unlock();
            }
        qint64 time = timer.elapsed();

        IntervalMutex.lock();
        qint64 interval_ = interval;
        IntervalMutex.unlock();

        time_out(threadname, &timer, interval_, time);
    }
}
void read_mercury(bool enable, QString threadname, MercuryClass * Mercury, quint8 id, QMutex * mutex)
{
    QElapsedTimer timer;
    while (1)
    {
        timer.restart();

        if (enable)
            while (Guid.hasNext(id))
            {
                mutex->lock();
                sqlDriver.push(Mercury->read_data(&Guid, id));
                mutex->unlock();
            }
        qint64 time = timer.elapsed();

        IntervalMutex.lock();
        qint64 interval_ = interval;
        IntervalMutex.unlock();

        time_out(threadname, &timer, interval_, time);
    }
}
void read_queue()
{
    while (1)
    {
        if (READ_MERCURY1)
            if (!Guid.isEmptyQueue(ID_Mercury_1)) {
                Mercury_1_mutex.lock();
                sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_1));
                Mercury_1_mutex.unlock();
            }
        if (READ_MERCURY2)
            if (!Guid.isEmptyQueue(ID_Mercury_2)) {
                Mercury_2_mutex.lock();
                sqlDriver.push(Mercury_2.read_data(&Guid, ID_Mercury_2));
                Mercury_2_mutex.unlock();
            }
        if (READ_MERCURY3)
            if (!Guid.isEmptyQueue(ID_Mercury_3)) {
                Mercury_3_mutex.lock();
                sqlDriver.push(Mercury_3.read_data(&Guid, ID_Mercury_3));
                Mercury_3_mutex.unlock();
            }
        if (READ_MERCURY4)
            if (!Guid.isEmptyQueue(ID_Mercury_4)) {
                Mercury_4_mutex.lock();
                sqlDriver.push(Mercury_4.read_data(&Guid, ID_Mercury_4));
                Mercury_4_mutex.unlock();
            }


        if (READ_PULSAR1)
            if (!Guid.isEmptyQueue(ID_Pulsar_1)) {
                Pulsar_1_mutex.lock();
                sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
                Pulsar_1_mutex.unlock();
            }
        if (READ_PULSAR2)
            if (!Guid.isEmptyQueue(ID_Pulsar_2)) {
                Pulsar_2_mutex.lock();
                sqlDriver.push(Pulsar_2.read_data(&Guid, ID_Pulsar_2));
                Pulsar_2_mutex.unlock();
            }
        if (READ_PULSAR3)
            if (!Guid.isEmptyQueue(ID_Pulsar_3)) {
                Pulsar_3_mutex.lock();
                sqlDriver.push(Pulsar_3.read_data(&Guid, ID_Pulsar_3));
                Pulsar_3_mutex.unlock();
            }

        if (READ_PULSAR4)
            if (!Guid.isEmptyQueue(ID_Pulsar_4)) {
                Pulsar_4_mutex.lock();
                sqlDriver.push(Pulsar_4.read_data(&Guid, ID_Pulsar_4));
                Pulsar_4_mutex.unlock();
            }

        QThread::msleep(1000);
    }
}

void cmdline()
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
//            emit finish();
        }
        else
            Guid.addQueue(msg);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ACBC SCADA");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;

    QString errorMessage;
    switch (parseCommandLine(parser, &errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case CommandLineVersionRequested:
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    case CommandLineHelpRequested:
        parser.showHelp();
        Q_UNREACHABLE();
    }

    if (READ_MERCURY1)
        Guid.init(&sqlDriver, QString("mercury_1"), &ID_Mercury_1);
    if (READ_MERCURY2)
        Guid.init(&sqlDriver, QString("mercury_2"), &ID_Mercury_2);
    if (READ_MERCURY3)
        Guid.init(&sqlDriver, QString("mercury_3"), &ID_Mercury_3);
    if (READ_MERCURY4)
        Guid.init(&sqlDriver, QString("mercury_4"), &ID_Mercury_4);

    if (READ_PULSAR1)
        Guid.init(&sqlDriver, QString("pulsar_1"), &ID_Pulsar_1);
    if (READ_PULSAR2)
        Guid.init(&sqlDriver, QString("pulsar_2"), &ID_Pulsar_2);
    if (READ_PULSAR3)
        Guid.init(&sqlDriver, QString("pulsar_3"), &ID_Pulsar_3);
    if (READ_PULSAR4)
        Guid.init(&sqlDriver, QString("pulsar_4"), &ID_Pulsar_4);

//    read_all_sensors();

    QFuture<void> future1 = QtConcurrent::run( read_pulsar );
    QFuture<void> future2 = QtConcurrent::run( read_mercury );
    QFuture<void> future_queue = QtConcurrent::run( read_queue );
    QFuture<void> future_server = QtConcurrent::run( server_connection );
//    QFuture<void> future5 = QtConcurrent::run( cmdline );

//    if (READ_PULSAR1)
//        QFuture<void> future_p1 = QtConcurrent::run( read_pulsar, READ_PULSAR1, QString("Pulsar1"), &Pulsar_1, ID_Pulsar_1, &Pulsar_1_mutex );
//    if (READ_PULSAR1)
//        QFuture<void> future_p2 = QtConcurrent::run( read_pulsar, READ_PULSAR2, QString("Pulsar2"), &Pulsar_2, ID_Pulsar_2, &Pulsar_2_mutex );
//    if (READ_PULSAR3)
//        QFuture<void> future_p3 = QtConcurrent::run( read_pulsar, READ_PULSAR3, QString("Pulsar3"), &Pulsar_3, ID_Pulsar_3, &Pulsar_3_mutex );
//    if (READ_PULSAR4)
//        QFuture<void> future_p4 = QtConcurrent::run( read_pulsar, READ_PULSAR4, QString("Pulsar4"), &Pulsar_4, ID_Pulsar_4, &Pulsar_4_mutex );

//    if (READ_MERCURY1)
//        QFuture<void> future_m1 = QtConcurrent::run( read_mercury, READ_MERCURY1, QString("Mercury1"), &Mercury_1, ID_Mercury_1, &Mercury_1_mutex );
//    if (READ_MERCURY2)
//        QFuture<void> future_m2 = QtConcurrent::run( read_mercury, READ_MERCURY2, QString("Mercury2"), &Mercury_2, ID_Mercury_2, &Mercury_2_mutex );
//    if (READ_MERCURY3)
//        QFuture<void> future_m3 = QtConcurrent::run( read_mercury, READ_MERCURY3, QString("Mercury3"), &Mercury_3, ID_Mercury_3, &Mercury_3_mutex );
//    if (READ_MERCURY4)
//        QFuture<void> future_m4 = QtConcurrent::run( read_mercury, READ_MERCURY4, QString("Mercury4"), &Mercury_4, ID_Mercury_4, &Mercury_4_mutex );


    return app.exec();

}

