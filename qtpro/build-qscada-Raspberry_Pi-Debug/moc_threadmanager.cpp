/****************************************************************************
** Meta object code from reading C++ file 'threadmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libs/threadmanager/threadmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'threadmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ThreadManager_t {
    QByteArrayData data[7];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ThreadManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ThreadManager_t qt_meta_stringdata_ThreadManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ThreadManager"
QT_MOC_LITERAL(1, 14, 12), // "mercury_slot"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 9), // "owen_slot"
QT_MOC_LITERAL(4, 38, 11), // "pulsar_slot"
QT_MOC_LITERAL(5, 50, 9), // "send_slot"
QT_MOC_LITERAL(6, 60, 24) // "get_sensor_interval_slot"

    },
    "ThreadManager\0mercury_slot\0\0owen_slot\0"
    "pulsar_slot\0send_slot\0get_sensor_interval_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThreadManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a /* Public */,
       3,    0,   40,    2, 0x0a /* Public */,
       4,    0,   41,    2, 0x0a /* Public */,
       5,    0,   42,    2, 0x0a /* Public */,
       6,    0,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ThreadManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ThreadManager *_t = static_cast<ThreadManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mercury_slot(); break;
        case 1: _t->owen_slot(); break;
        case 2: _t->pulsar_slot(); break;
        case 3: _t->send_slot(); break;
        case 4: _t->get_sensor_interval_slot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ThreadManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ThreadManager.data,
      qt_meta_data_ThreadManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ThreadManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThreadManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ThreadManager.stringdata0))
        return static_cast<void*>(const_cast< ThreadManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ThreadManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
