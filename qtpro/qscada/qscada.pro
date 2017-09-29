#-------------------------------------------------
#
# Project created by QtCreator 2017-04-11T12:37:59
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = qscada
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#CONFIG += warn_off
#QMAKE_CXXFLAGS += -Wno-unused-parameter
#CXXFLAGS += -Wno-unused-parameter

SOURCES += \
    ../../src/main.cpp \
    ../../src/libs/https/https.cpp \
    ../../src/libs/id/id.cpp \
    ../../src/libs/mercury/mercury.cpp \
    ../../src/libs/modbus/ModbusClass.cpp \
    ../../src/libs/modbus/modbus-data.c \
    ../../src/libs/modbus/modbus-rtu.c \
    ../../src/libs/modbus/modbus-tcp.c \
    ../../src/libs/modbus/modbus.c \
    ../../src/libs/owen/owen.cpp \
    ../../src/libs/threadmanager/threadmanager.cpp \
    ../../src/libs/network/networkclass.cpp \
    ../../src/libs/json/json.cpp \
    ../../src/libs/pulsar/pulsarclass.cpp \
    ../../src/libs/sqldriver/sqldriver.cpp
INSTALLS        = target
target.path     = /home/pi

HEADERS += \
    ../../src/defines.h \
    ../../src/includes.h \
    ../../src/main.h \
    ../../src/libs/https/https.h \
    ../../src/libs/id/id.h \
    ../../src/libs/id/init_sensorvector.h \
    ../../src/libs/mercury/mercury.h \
    ../../src/libs/modbus/config.h \
    ../../src/libs/modbus/modbus-private.h \
    ../../src/libs/modbus/modbus-rtu-private.h \
    ../../src/libs/modbus/modbus-rtu.h \
    ../../src/libs/modbus/modbus-tcp-private.h \
    ../../src/libs/modbus/modbus-tcp.h \
    ../../src/libs/modbus/modbus-version.h \
    ../../src/libs/modbus/modbus.h \
    ../../src/libs/modbus/ModbusClass.h \
    ../../src/libs/owen/owen.h \
    ../../src/libs/threadmanager/threadmanager.h \
    ../../src/libs/network/networkclass.h \
    ../../src/libs/json/json.h \
    ../../src/libs/pulsar/pulsarclass.h \
    ../../src/libs/sqldriver/sqldriver.h
INCLUDEPATH += ../../../../../sysroot/usr/local/include/
LIBS += -lcurl -lssl -lcrypto
INCLUDEPATH += /home/hells1ng/raspi/sysroot/usr/local/qt5pi/include/

