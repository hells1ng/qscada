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
    QByteArrayData data[20];
    char stringdata0[264];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ThreadManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ThreadManager_t qt_meta_stringdata_ThreadManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ThreadManager"
QT_MOC_LITERAL(1, 14, 6), // "finish"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 14), // "mercury_1_slot"
QT_MOC_LITERAL(4, 37, 14), // "mercury_2_slot"
QT_MOC_LITERAL(5, 52, 13), // "pulsar_1_slot"
QT_MOC_LITERAL(6, 66, 13), // "pulsar_2_slot"
QT_MOC_LITERAL(7, 80, 14), // "mercury_3_slot"
QT_MOC_LITERAL(8, 95, 14), // "mercury_4_slot"
QT_MOC_LITERAL(9, 110, 13), // "pulsar_3_slot"
QT_MOC_LITERAL(10, 124, 13), // "pulsar_4_slot"
QT_MOC_LITERAL(11, 138, 21), // "read_all_sensors_slot"
QT_MOC_LITERAL(12, 160, 9), // "owen_slot"
QT_MOC_LITERAL(13, 170, 9), // "send_slot"
QT_MOC_LITERAL(14, 180, 22), // "getInfoFromServer_slot"
QT_MOC_LITERAL(15, 203, 11), // "sphera_slot"
QT_MOC_LITERAL(16, 215, 23), // "QueueReqFromServer_slot"
QT_MOC_LITERAL(17, 239, 12), // "cmdline_slot"
QT_MOC_LITERAL(18, 252, 5), // "slot1"
QT_MOC_LITERAL(19, 258, 5) // "slot2"

    },
    "ThreadManager\0finish\0\0mercury_1_slot\0"
    "mercury_2_slot\0pulsar_1_slot\0pulsar_2_slot\0"
    "mercury_3_slot\0mercury_4_slot\0"
    "pulsar_3_slot\0pulsar_4_slot\0"
    "read_all_sensors_slot\0owen_slot\0"
    "send_slot\0getInfoFromServer_slot\0"
    "sphera_slot\0QueueReqFromServer_slot\0"
    "cmdline_slot\0slot1\0slot2"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThreadManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  104,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  105,    2, 0x0a /* Public */,
       4,    0,  106,    2, 0x0a /* Public */,
       5,    0,  107,    2, 0x0a /* Public */,
       6,    0,  108,    2, 0x0a /* Public */,
       7,    0,  109,    2, 0x0a /* Public */,
       8,    0,  110,    2, 0x0a /* Public */,
       9,    0,  111,    2, 0x0a /* Public */,
      10,    0,  112,    2, 0x0a /* Public */,
      11,    0,  113,    2, 0x0a /* Public */,
      12,    0,  114,    2, 0x0a /* Public */,
      13,    0,  115,    2, 0x0a /* Public */,
      14,    0,  116,    2, 0x0a /* Public */,
      15,    0,  117,    2, 0x0a /* Public */,
      16,    0,  118,    2, 0x0a /* Public */,
      17,    0,  119,    2, 0x0a /* Public */,
      18,    0,  120,    2, 0x0a /* Public */,
      19,    0,  121,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
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
        case 0: _t->finish(); break;
        case 1: _t->mercury_1_slot(); break;
        case 2: _t->mercury_2_slot(); break;
        case 3: _t->pulsar_1_slot(); break;
        case 4: _t->pulsar_2_slot(); break;
        case 5: _t->mercury_3_slot(); break;
        case 6: _t->mercury_4_slot(); break;
        case 7: _t->pulsar_3_slot(); break;
        case 8: _t->pulsar_4_slot(); break;
        case 9: _t->read_all_sensors_slot(); break;
        case 10: _t->owen_slot(); break;
        case 11: _t->send_slot(); break;
        case 12: _t->getInfoFromServer_slot(); break;
        case 13: _t->sphera_slot(); break;
        case 14: _t->QueueReqFromServer_slot(); break;
        case 15: _t->cmdline_slot(); break;
        case 16: _t->slot1(); break;
        case 17: _t->slot2(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ThreadManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ThreadManager::finish)) {
                *result = 0;
                return;
            }
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
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void ThreadManager::finish()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
