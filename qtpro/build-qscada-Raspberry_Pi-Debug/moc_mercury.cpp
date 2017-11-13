/****************************************************************************
** Meta object code from reading C++ file 'mercury.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libs/mercury/mercury.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mercury.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MercuryClass_t {
    QByteArrayData data[9];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MercuryClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MercuryClass_t qt_meta_stringdata_MercuryClass = {
    {
QT_MOC_LITERAL(0, 0, 12), // "MercuryClass"
QT_MOC_LITERAL(1, 13, 5), // "write"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 14), // "unsigned char*"
QT_MOC_LITERAL(4, 35, 3), // "cmd"
QT_MOC_LITERAL(5, 39, 7), // "sizecmd"
QT_MOC_LITERAL(6, 47, 8), // "received"
QT_MOC_LITERAL(7, 56, 4), // "buf_"
QT_MOC_LITERAL(8, 61, 7) // "timeout"

    },
    "MercuryClass\0write\0\0unsigned char*\0"
    "cmd\0sizecmd\0received\0buf_\0timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MercuryClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   34,    2, 0x0a /* Public */,
       8,    0,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void,

       0        // eod
};

void MercuryClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MercuryClass *_t = static_cast<MercuryClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->write((*reinterpret_cast< unsigned char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->received((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: _t->timeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MercuryClass::*_t)(unsigned char * , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MercuryClass::write)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MercuryClass::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MercuryClass.data,
      qt_meta_data_MercuryClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MercuryClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MercuryClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MercuryClass.stringdata0))
        return static_cast<void*>(const_cast< MercuryClass*>(this));
    return QObject::qt_metacast(_clname);
}

int MercuryClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void MercuryClass::write(unsigned char * _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
