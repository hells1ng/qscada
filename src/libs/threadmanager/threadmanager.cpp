#include "threadmanager.h"

ThreadManager::ThreadManager()
{

}

ThreadManager::~ThreadManager()
{
    threads.join_all();
}

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
