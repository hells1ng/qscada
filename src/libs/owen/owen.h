#ifndef OWENCLASS_H_INCLUDED
#define OWENCLASS_H_INCLUDED
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <termios.h>
#include "../sqldriver/sqldriver.h"
//#include "../modbus/modbus.h"
#include "../modbus/ModbusClass.h"
#include "../id/id.h"
#include "../../defines.h"

using std::string;
using std::vector;

typedef struct
{
    uint8_t* bits;
    uint16_t* regs;
} owenData;

class OwenClass
{
protected:
    static int counter;
    static char str_data[STR_DATA_LINE][STR_DATA_COLUMNS];
    static string data;
    enum {
        reg_size = 100,
        ADDRESS_START_DI = 51,
        ADDRESS_END_DI = 51,
            nb_DI = ADDRESS_END_DI - ADDRESS_START_DI + 1,
        ADDRESS_START_DI2 = 64,
        ADDRESS_END_DI2  = 79,
            nb_DI2 = ADDRESS_END_DI2 - ADDRESS_START_DI2 + 1,
        ADDRESS_START_AI = 0x100,
        ADDRESS_END_AI  = 0x107,
            nb_AI = ADDRESS_END_AI - ADDRESS_START_AI + 1,
        ADDRESS_START_RTD  = 0,
        ADDRESS_END_RTD = 43,
            nb_RTD = ADDRESS_END_RTD - ADDRESS_START_RTD + 1,
        ADDRESS_START_KM = 19,
        ADDRESS_END_KM = 50,
            nb_KM = ADDRESS_END_KM - ADDRESS_START_KM + 1,
        ADDRESS_START_SimDI = 0,
        ADDRESS_END_SimDI = 15,
            nb_SimDI = ADDRESS_END_SimDI - ADDRESS_START_SimDI + 1,
        ADDRESS_START_SimAI = 0,
        ADDRESS_END_SimAI = 15,
            nb_SimAI = ADDRESS_END_SimAI - ADDRESS_START_SimAI + 1,
        ADDRESS_START_8R = 0,
        ADDRESS_END_8R = 7,
            nb_8R = ADDRESS_END_8R - ADDRESS_START_8R + 1
    };
    static uint16_t tab_rp_registers[reg_size];

public:

    static string str_id;
    bool isFirstReading;
    static float real[reg_size];
    static int nb_fail;
    uint8_t address;
    uint8_t arr_size;
    string str_line;
    OwenClass();
    OwenClass(uint8_t address_ = 16, uint8_t arr_size_ = 16, const string str_line_="") :
               address(address_), arr_size(arr_size_), str_line(str_line_) {
        counter++;
        isFirstReading = true;
    }
    ~OwenClass() {
        counter--;
    }
    /* Prototype of read_data func */
    virtual Data read_data(ModbusClass* , GuidClass* , quint8) {
        Data ret;
        return ret;
    }

    virtual int write_data(ModbusClass*, vector<string>, int) {
        return 0;
    }

    void print_results();

    void clear_fail() {
        nb_fail = 0;
    }
};

class OwenClass_16D : public OwenClass
{
public:
    enum {
        nb_BITS = 16
    };
    owenData data_16D;//TODO test
    OwenClass_16D();
    OwenClass_16D(uint8_t address_ = 16, uint8_t arr_size_ = 16, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
        data_16D.bits = new uint8_t[nb_BITS];//TODO test
        data_16D.regs = new uint16_t[nb_BITS];//TODO test
    }
    ~OwenClass_16D() {
        counter--;
        delete [] data_16D.bits;//TODO test
        delete [] data_16D.regs;//TODO test
    }
    uint8_t tab_rp_bits[nb_BITS];
    uint8_t tab_rp_bits_prev[nb_BITS];
    uint16_t tab_rp_registers_prev[nb_BITS];
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_8A : public OwenClass
{
public:
    OwenClass_8A();
    OwenClass_8A(uint8_t address_ = 16, uint8_t arr_size_ = 8, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_8A () {
        counter--;
    }
    float real_prev[8];
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class Sphera_24CI : public OwenClass
{
public:
    enum {
        ADDRESS_START_DI = 0,
        nb_INPUTS = 24,
        nb_REGS = nb_INPUTS * 2 + 2 // 24 входа 32bit, 1 регист адреса 16bit, 1 регистр общий 16bit
    };
    uint16_t tab_registers[nb_REGS];

    Sphera_24CI();
    Sphera_24CI(uint8_t address_ = 255, uint8_t arr_size_ = nb_REGS, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~Sphera_24CI() {
        counter--;
    }
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_8AC : public OwenClass
{
public:
    OwenClass_8AC();
    OwenClass_8AC(uint8_t address_ = 16, uint8_t arr_size_ = 8, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_8AC () {
        counter--;
    }
    float real_prev[8];
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_KM : public OwenClass
{
public:
    OwenClass_KM();
    OwenClass_KM(uint8_t address_ = 16, uint8_t arr_size_ = 8, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_KM() {
        counter--;
        }
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_SimDI : public OwenClass
{
public:
    enum {
        nb_BITS=16
    };
    OwenClass_SimDI();
    OwenClass_SimDI(uint8_t address_ = 16, uint8_t arr_size_ = 16, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_SimDI() {
        counter--;
    }
    uint8_t tab_rp_bits[nb_BITS];
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_SimAI : public OwenClass
{
public:
    OwenClass_SimAI();
    OwenClass_SimAI(uint8_t address_ = 16, uint8_t arr_size_ = 16, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_SimAI() {
        counter--;
    }
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
};

class OwenClass_NL_8R : public OwenClass
{
public:
    OwenClass_NL_8R();
    OwenClass_NL_8R(uint8_t address_ = 16, uint8_t arr_size_ = 8, const string str_line_ = "") : OwenClass(address_, arr_size_, str_line_) {
    }
    ~OwenClass_NL_8R() {
        counter--;
    }
    uint8_t tab_rp_bits[nb_8R];
    uint8_t tab_rp_bits_prev[nb_8R];
    Data read_data(ModbusClass* modbus, GuidClass* guid, quint8 id);
    int write_data(ModbusClass* modbus, vector<string> stringguidforwrite, int valueforwrite);
};
#endif // OWENCLASS_H_INCLUDED

