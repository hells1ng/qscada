#include "includes.h"
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

//IODriver        ioPulsar_1(IODriver::TCP, "192.168.88.100", 4006);
//IODriver        ioPulsar_2(IODriver::TCP, "192.168.88.100", 4001);
//IODriver        ioPulsar_3(IODriver::TCP, "192.168.88.100", 4004);
//IODriver        ioPulsar_4(IODriver::TCP, "192.168.88.100", 4003);

SqlDriver       sqlDriver;
HttpsDriver     httpsDriver;

void sendToServer()
{
    /* Get data from queue*/
//    while (1)
//    {
        Data sendData = sqlDriver.pop(100);
        qDebug()<<"Size of sensors for sending = " << sendData.size();

        httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

        sqlDriver.toDataTable(sendData);

        /* Get data from sqlite*/
        Data dataFromTable = sqlDriver.fromDataTable(200);

        httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

        sqlDriver.toDataTable(dataFromTable);

        QThread::msleep(1000);
//    }
}

void read_all_sensors()
{
    while (1)
    {
        while (Guid.hasNext(ID_Mercury_1))
            sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_1));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_2))
            sqlDriver.push(Mercury_2.read_data(&Guid, ID_Mercury_2));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_3))
            sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_3));
        sendToServer();
        while (Guid.hasNext(ID_Mercury_4))
            sqlDriver.push(Mercury_1.read_data(&Guid, ID_Mercury_4));
        sendToServer();

        while (Guid.hasNext(ID_Pulsar_1))
            sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_2))
            sqlDriver.push(Pulsar_2.read_data(&Guid, ID_Pulsar_2));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_3))
            sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_3));
        sendToServer();
        while (Guid.hasNext(ID_Pulsar_4))
            sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_4));
        sendToServer();
    }
}

//void read_pulsar_1()
//{
//    while (1)
//    {
//        while (Guid.hasNext(ID_Pulsar_1))
//            sqlDriver.push(Pulsar_1.read_data(&Guid, ID_Pulsar_1));
//    }
//}
//void read_pulsar_2()
//{
//    while (1)
//    {
//        while (Guid.hasNext(ID_Pulsar_2))
//            sqlDriver.push(Pulsar_2.read_data(&Guid, ID_Pulsar_2));
//    }
//}


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

//    QObject::connect(&Pulsar_1, SIGNAL(write(QByteArray )), &ioPulsar_1, SLOT(write(QByteArray)));
//    QObject::connect(&ioPulsar_1, SIGNAL(response(QByteArray)), &Pulsar_1, SLOT(received(QByteArray)));
//    QObject::connect(&ioPulsar_1, SIGNAL(timeout()), &Pulsar_1, SLOT(timeout()));

//    QObject::connect(&Pulsar_2, SIGNAL(write(QByteArray )), &ioPulsar_2, SLOT(write(QByteArray)));
//    QObject::connect(&ioPulsar_2, SIGNAL(response(QByteArray)), &Pulsar_2, SLOT(received(QByteArray)));
//    QObject::connect(&ioPulsar_2, SIGNAL(timeout()), &Pulsar_2, SLOT(timeout()));

//    QObject::connect(&Pulsar_3, SIGNAL(write(QByteArray )), &ioPulsar_3, SLOT(write(QByteArray)));
//    QObject::connect(&ioPulsar_3, SIGNAL(response(QByteArray)), &Pulsar_3, SLOT(received(QByteArray)));
//    QObject::connect(&ioPulsar_3, SIGNAL(timeout()), &Pulsar_3, SLOT(timeout()));

//    QObject::connect(&Pulsar_4, SIGNAL(write(QByteArray )), &ioPulsar_4, SLOT(write(QByteArray)));
//    QObject::connect(&ioPulsar_4, SIGNAL(response(QByteArray)), &Pulsar_4, SLOT(received(QByteArray)));
//    QObject::connect(&ioPulsar_4, SIGNAL(timeout()), &Pulsar_4, SLOT(timeout()));

    Guid.init(&sqlDriver, QString("mercury_1"), &ID_Mercury_1);
    Guid.init(&sqlDriver, QString("mercury_2"), &ID_Mercury_2);
    Guid.init(&sqlDriver, QString("mercury_3"), &ID_Mercury_3);
    Guid.init(&sqlDriver, QString("mercury_4"), &ID_Mercury_4);

    Guid.init(&sqlDriver, QString("pulsar_1"), &ID_Pulsar_1);
    Guid.init(&sqlDriver, QString("pulsar_2"), &ID_Pulsar_2);
    Guid.init(&sqlDriver, QString("pulsar_3"), &ID_Pulsar_3);
    Guid.init(&sqlDriver, QString("pulsar_4"), &ID_Pulsar_4);

    read_all_sensors();

//    QFuture<void> future1 = QtConcurrent::run( read_all_sensors );
//    QFuture<void> future2 = QtConcurrent::run( sendToServer );
//    QFuture<void> future3 = QtConcurrent::run( read_pulsar_1 );
//    QFuture<void> future4 = QtConcurrent::run( read_pulsar_2 );

//    read_all_sensors();

//    Mercury_1               = new MercuryClass(IODriver::TCP, "192.168.88.100", 4007);

//    ThreadManager Manager;
//    QObject::connect(&Manager, &ThreadManager::finish, &app, &QCoreApplication::quit);

//    ThreadManager * Manager = new ThreadManager;
//    QObject::connect(Manager, &ThreadManager::finish, &app, &QCoreApplication::quit);

//    qDebug() << parser.values(targetDirectoryOption);

//    app.exec();


    return app.exec();

}

