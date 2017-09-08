#ifndef THREADMANAGER_H_INCLUDED
#define THREADMANAGER_H_INCLUDED
#include <iostream>
#include <boost/function.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/thread/thread.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
void doEvery(const string& every, boost::function<void()> f, const string& taskname);
void doAtAndEvery(const string& when, const string& interval,
                  boost::function<void()> f, const string& taskname);

class ThreadManager
{
public:
    boost::thread_group threads;
    ThreadManager();
    ~ThreadManager();
    void addTaskEvery(const string& every, boost::function<void()> f, const string& taskname = "no_name");
    void addTaskAtAndEvery(const string& when, const string& interval,
                           boost::function<void()> f, const string& taskname = "no_name");
    void startAll();
};
#endif // INCLUDES_H_INCLUDED
