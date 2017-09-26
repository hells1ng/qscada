#include "threadmanager.h"


ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent),
    Modbus(USB1, 115200, 'N', 8, 1, "192.168.88.100", 4006, ModbusClass::TCP),
    Can(USB0, 9600, 'N', 8, 1, "192.168.88.100", 4007, CanClass::TCP),
    Owen_16D_1(1, 16, "RS485"),
    Owen_8A_11(11, 8, "RS485"),
    Owen_8AC_41(41, 8, "RS485"),
    NL_8R_2(2, 8, "RS485"),
    Mercury_44(44, "CAN")
//    Pulse(11223344)
{
    print_map(Guid.Map.left, " with ", std::cout);

            /*Init OvenVector*/
    OwenVector.push_back(&Owen_16D_1);
    //OwenVector.push_back(&Owen_8A_11);
    //OwenVector.push_back(&Owen_8AC_41); // 8AC pogib
    //OwenVector.push_back(&Owen_KM_56);
    //OwenVector.push_back(&UNO_61);
    //OwenVector.push_back(&NL_8R_2);

    timer1 = new QTimer();
    timer1->setInterval(1000);
    connect(timer1, SIGNAL(timeout()), this, SLOT(mercury_thread()));
    timer1->start();

    timer2 = new QTimer();
    timer2->setInterval(1000);
    connect(timer2, SIGNAL(timeout()), this, SLOT(sendToServer()));
    timer2->start();
}

ThreadManager::~ThreadManager()
{
    delete timer1;
    delete timer2;
}

void ThreadManager::owen_thread()
{
    for (unsigned int i = 0; i < OwenVector.size(); i++) {
        Owen_ptr = OwenVector[i];
        Owen_ptr->read_data(&Modbus, &Guid);//TODO sqlDriver.toDataTable(Owen_ptr->read_data(&Modbus, &Guid);)
    }
}

void ThreadManager::mercury_thread()
{
    timer1->setInterval(2000);

//    QElapsedTimer timer;
//    timer.start();

    sqlDriver.push(Mercury_44.read_data(&Can, &Guid));

//    qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";

}

void ThreadManager::sendToServer()
{
    timer2->setInterval(1000);

    Data sendData = sqlDriver.pop(10);

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &sendData);

    sqlDriver.toDataTable(sendData);

    Data dataFromTable = sqlDriver.fromDataTable(50);
//    foreach (const QStringList &qsl, dataFromTable) {
//        qDebug() << "fromDataTable = " << qsl;
//    }

    httpsDriver.Send(HttpsDriver::HTTPS_CMD_POST_SENSOR_VALUE, &dataFromTable);

    sqlDriver.toDataTable(dataFromTable);


    //TODO do fromDataTable when pop() is empty
}

void ThreadManager::getSensorIntervalFromServer()
{
//    httpsDriver.Send( HttpsDriver::GET_SENSOR_PERIOD );
}

/*
void doEvery(const string& every, boost::function<void()> f, const string& taskname)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    time_duration interval(duration_from_string(every));
    ptime expired;
    time_duration msdiff;

    while (true) {
        expired = microsec_clock::local_time() + interval;
        //cout << "expired " << expired << endl;

        f();

        //cout << "now " << now << endl;
        msdiff = expired - microsec_clock::local_time();
        //cout << "diff = " << msdiff.total_milliseconds() << endl;
        if (!msdiff.is_negative()) {
            boost::this_thread::sleep( boost::posix_time::milliseconds(msdiff.total_milliseconds()) );
        }
        if (taskname != "no_name")
            cout << "\t\t\tTask " << taskname << " was done for " << interval - msdiff << endl;
    }
}

void doAtAndEvery(const string& when, const string& interval, boost::function<void()> f, const string& taskname)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    time_duration interval_( duration_from_string(interval) );

    time_duration when_( duration_from_string(when) );
    ptime expired( date(microsec_clock::local_time().date()), when_ ); // первое время выполнения

    time_duration msdiff = expired - microsec_clock::local_time();// время до полного выполнения

    while (msdiff.is_negative()) { //проверка на 1-е выполнение функции: назначенное время прошло?
        //cout << "diff = " << msdiff.total_milliseconds() << endl;
        expired = expired + interval_;// обновляем время выполнения
        msdiff = expired - microsec_clock::local_time();//обновляем время ДО нового выполнения
    }
    bool firstRun = true;

    while (true) {
        if (!msdiff.is_negative()) {
            boost::this_thread::sleep( boost::posix_time::milliseconds(msdiff.total_milliseconds()) );
        }

        if ((!firstRun) && (taskname != "no_name"))
            cout << "\t\t\tTask " << taskname << " was done for " << interval_ - msdiff << endl;//время выполнения задаи (пропуск 1 прохода)
        expired = microsec_clock::local_time() + interval_;// обновляем время выполнения

        f();//выполняем

        msdiff = expired - microsec_clock::local_time();//обновляем время ДО нового выполнени
        firstRun = false;
    }
}

void ThreadManager::addTaskEvery(const string& every, boost::function<void()> f, const string& taskname)
{
    threads.add_thread(new boost::thread(doEvery, every, f, taskname));
}

void ThreadManager::addTaskAtAndEvery(const string& when, const string& interval,
                                      boost::function<void()> f, const string& taskname)
{
    threads.add_thread(new boost::thread(doAtAndEvery, when, interval, f, taskname));
}

void ThreadManager::startAll()
{
    threads.join_all();
}
*/
