/****************************************************************************
** Meta object code from reading C++ file 'controller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/controller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Controller_t {
    QByteArrayData data[22];
    char stringdata0[275];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller_t qt_meta_stringdata_Controller = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Controller"
QT_MOC_LITERAL(1, 11, 13), // "specDataReady"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 16), // "QVector<int16_t>"
QT_MOC_LITERAL(4, 43, 8), // "specData"
QT_MOC_LITERAL(5, 52, 12), // "rmsDataReady"
QT_MOC_LITERAL(6, 65, 7), // "rmsData"
QT_MOC_LITERAL(7, 73, 12), // "rawDataReady"
QT_MOC_LITERAL(8, 86, 7), // "rawData"
QT_MOC_LITERAL(9, 94, 14), // "forwardFpgaCmd"
QT_MOC_LITERAL(10, 109, 11), // "startThread"
QT_MOC_LITERAL(11, 121, 10), // "stopThread"
QT_MOC_LITERAL(12, 132, 13), // "onSendFpgaCmd"
QT_MOC_LITERAL(13, 146, 15), // "onFpgaConnected"
QT_MOC_LITERAL(14, 162, 12), // "onFpgaInited"
QT_MOC_LITERAL(15, 175, 17), // "onServerConnected"
QT_MOC_LITERAL(16, 193, 20), // "onServerDisconnected"
QT_MOC_LITERAL(17, 214, 10), // "onAppLogIn"
QT_MOC_LITERAL(18, 225, 11), // "onAppLogOut"
QT_MOC_LITERAL(19, 237, 29), // "onUdpReportTargetSetRequested"
QT_MOC_LITERAL(20, 267, 2), // "ip"
QT_MOC_LITERAL(21, 270, 4) // "port"

    },
    "Controller\0specDataReady\0\0QVector<int16_t>\0"
    "specData\0rmsDataReady\0rmsData\0"
    "rawDataReady\0rawData\0forwardFpgaCmd\0"
    "startThread\0stopThread\0onSendFpgaCmd\0"
    "onFpgaConnected\0onFpgaInited\0"
    "onServerConnected\0onServerDisconnected\0"
    "onAppLogIn\0onAppLogOut\0"
    "onUdpReportTargetSetRequested\0ip\0port"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       5,    1,   87,    2, 0x06 /* Public */,
       7,    1,   90,    2, 0x06 /* Public */,
       9,    1,   93,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   96,    2, 0x0a /* Public */,
      11,    0,   97,    2, 0x0a /* Public */,
      12,    1,   98,    2, 0x0a /* Public */,
      13,    0,  101,    2, 0x0a /* Public */,
      14,    0,  102,    2, 0x0a /* Public */,
      15,    0,  103,    2, 0x0a /* Public */,
      16,    0,  104,    2, 0x0a /* Public */,
      17,    0,  105,    2, 0x0a /* Public */,
      18,    0,  106,    2, 0x0a /* Public */,
      19,    2,  107,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void, QMetaType::QByteArray,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,   20,   21,

       0        // eod
};

void Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Controller *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->specDataReady((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 1: _t->rmsDataReady((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 2: _t->rawDataReady((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 3: _t->forwardFpgaCmd((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->startThread(); break;
        case 5: _t->stopThread(); break;
        case 6: _t->onSendFpgaCmd((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 7: _t->onFpgaConnected(); break;
        case 8: _t->onFpgaInited(); break;
        case 9: _t->onServerConnected(); break;
        case 10: _t->onServerDisconnected(); break;
        case 11: _t->onAppLogIn(); break;
        case 12: _t->onAppLogOut(); break;
        case 13: _t->onUdpReportTargetSetRequested((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Controller::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Controller::specDataReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Controller::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Controller::rmsDataReady)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Controller::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Controller::rawDataReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Controller::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Controller::forwardFpgaCmd)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Controller.data,
    qt_meta_data_Controller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Controller::specDataReady(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Controller::rmsDataReady(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Controller::rawDataReady(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Controller::forwardFpgaCmd(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
