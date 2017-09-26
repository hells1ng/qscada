#include "includes.h"
#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ThreadManager Manager;

//    Manager.addTaskEvery("00:00:5", owen_thread, "OWEN_TASK");
    	//Manager.addTaskEvery("0:0:10", mercury_wait, "MERCURY_WAIT");
//    Manager.addTaskEvery("00:00:1", mercury_thread, "MERCURY");
    	//Manager.addTaskAtAndEvery("15:21", "00:00:15", mercury_thread);
    //Manager.addTaskEvery("00:00:01", FromPost);
    	//Manager.addTaskEvery("00:00:01", scripts);//TODO test
    	//sleep(100*1000);
    	//Manager.addTaskEvery("00:00:01", sendToServer);

//    Manager.addTaskAtAndEvery("10:00", "00:00:1", sendToServer/*, "SENDING"*/);

//    Manager.addTaskAtAndEvery("10:00","00:00:30", getSensorIntervalFromServer, "GET_SENSOR_PERIOD");

    return a.exec();
}

