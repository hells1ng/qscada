#include "ModbusClass.h"

void ModbusClass::initialize(const char *device, int baud, char parity,
                             int data_bit, int stop_bit)
{
    ctx = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);
}

ModbusClass::ModbusClass(const char *device, int baud, char parity,
                                int data_bit, int stop_bit,
                                const char *ip, int port, int mode_)
{
    mode = mode_;
    switch (mode) {
    case RTU:
        ctx = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);

        break;
    case TCP:
        ctx = modbus_new_rtutcp(ip,port);
        break;
    default:
        break;
    }

//    modbus_set_debug(ctx, TRUE);
}

ModbusClass::~ModbusClass()
{
    modbus_free(ctx);
}

void ModbusClass::lock()
{
    mtx_.lock();
}

void ModbusClass::unlock()
{
    mtx_.unlock();
}

bool ModbusClass::isConnected()
{
    lock();
    if (modbus_connect(ctx) == -1) {
        //fprintf(stderr, "Connection to Modbus failed: %s\n", modbus_strerror(errno));
        cout << "Connection to Modbus failed" << endl;
        return false;
    }
    return true;
}

void ModbusClass::close()
{
    modbus_close(ctx);
    unlock();
}
