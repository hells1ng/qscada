/*
 * MAIN Program
 */
#include "includes.h"
using namespace std;

GuidClass       Guid;
ModbusClass     Modbus(USB1, 115200, 'N', 8, 1, "192.168.88.100", 4006, ModbusClass::TCP);
CanClass        Can(USB0, 9600, 'N', 8, 1, "192.168.88.100", 4007, CanClass::TCP);

//------------------OWEN MODULES---------------------------------------//
OwenClass_16D Owen_16D_1(1, 16, "RS485");
OwenClass_8A Owen_8A_11(11, 8, "RS485");
OwenClass_8AC Owen_8AC_41(41, 8, "RS485");
//OwenClass_KM Owen_KM_56(56);
//OwenClass_SimAI UNO_61(61, 16);
OwenClass_NL_8R NL_8R_2(2, 8, "RS485");

vector<OwenClass*> OwenVector;
OwenClass* Owen_ptr;
//------------------Mercury MODULES---------------------------------------//
MercuryClass Mercury_44(44, "CAN");
//------------------Global DATA---------------------------------------//
uint8_t* DI = Owen_16D_1.data_16D.bits;
//------------------Pulsar---------------------------------------//
PulsarClass Pulse(11223344);

#include "main.h"

int main()
{
    //system("su chmod 777 /dev/ttyUSB0/");
    print_map(Guid.Map.left, " with ", std::cout);
    usleep(1*1000*1000);

    initOwenVector();

    ThreadManager Manager;

//    Manager.addTaskEvery("00:00:5", owen_thread, "OWEN_TASK");
    	//Manager.addTaskEvery("0:0:10", mercury_wait, "MERCURY_WAIT");
    Manager.addTaskEvery("00:00:1", mercury_thread, "MERCURY");
    	//Manager.addTaskAtAndEvery("15:21", "00:00:15", mercury_thread);
    //Manager.addTaskEvery("00:00:01", FromPost);
    	//Manager.addTaskEvery("00:00:01", scripts);//TODO test
    	//sleep(100*1000);
    	//Manager.addTaskEvery("00:00:01", sendToServer);

    Manager.addTaskAtAndEvery("10:00", "00:00:5", sendToServer/*, "SENDING"*/);

//    Manager.addTaskAtAndEvery("10:00","00:00:30", getSensorIntervalFromServer, "GET_SENSOR_PERIOD");


    Manager.startAll();


    return 0;
}

