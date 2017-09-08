#ifndef MODBUSCLASS_H
#define MODBUSCLASS_H
#include <iostream>
#include "modbus.h"
#include <boost/thread/mutex.hpp>

using namespace std;

class ModbusClass
{
public:
    enum {
        RTU = 0,
        TCP = 1
    };
    modbus_t* ctx;
    int mode;
    ModbusClass(const char *device, int baud, char parity,
                int data_bit, int stop_bit,
                const char *ip, int port, int mode_);
    ~ModbusClass();
    void initialize(const char *device, int baud, char parity,
                    int data_bit, int stop_bit);
    bool isConnected();
    void close();
    void lock();
    void unlock();
private:
    boost::mutex mtx_;
};

#endif // HTTPS_H
