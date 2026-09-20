/****************************************************************************
** Meta object code from reading C++ file 'systemgpiocontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/systemgpiocontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'systemgpiocontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SystemGPIOController_t {
    QByteArrayData data[15];
    char stringdata0[207];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SystemGPIOController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SystemGPIOController_t qt_meta_stringdata_SystemGPIOController = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SystemGPIOController"
QT_MOC_LITERAL(1, 21, 16), // "yellowLedChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 5), // "state"
QT_MOC_LITERAL(4, 45, 15), // "greenLedChanged"
QT_MOC_LITERAL(5, 61, 15), // "dvsPowerChanged"
QT_MOC_LITERAL(6, 77, 17), // "fourGPowerChanged"
QT_MOC_LITERAL(7, 95, 17), // "systemInitialized"
QT_MOC_LITERAL(8, 113, 7), // "success"
QT_MOC_LITERAL(9, 121, 11), // "systemError"
QT_MOC_LITERAL(10, 133, 5), // "error"
QT_MOC_LITERAL(11, 139, 19), // "systemStatusUpdated"
QT_MOC_LITERAL(12, 159, 6), // "status"
QT_MOC_LITERAL(13, 166, 20), // "onYellowBlinkTimeout"
QT_MOC_LITERAL(14, 187, 19) // "onGreenBlinkTimeout"

    },
    "SystemGPIOController\0yellowLedChanged\0"
    "\0state\0greenLedChanged\0dvsPowerChanged\0"
    "fourGPowerChanged\0systemInitialized\0"
    "success\0systemError\0error\0systemStatusUpdated\0"
    "status\0onYellowBlinkTimeout\0"
    "onGreenBlinkTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SystemGPIOController[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       5,    1,   65,    2, 0x06 /* Public */,
       6,    1,   68,    2, 0x06 /* Public */,
       7,    1,   71,    2, 0x06 /* Public */,
       9,    1,   74,    2, 0x06 /* Public */,
      11,    1,   77,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   80,    2, 0x08 /* Private */,
      14,    0,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SystemGPIOController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SystemGPIOController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->yellowLedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->greenLedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->dvsPowerChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->fourGPowerChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->systemInitialized((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->systemError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->systemStatusUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onYellowBlinkTimeout(); break;
        case 8: _t->onGreenBlinkTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SystemGPIOController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::yellowLedChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::greenLedChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::dvsPowerChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::fourGPowerChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::systemInitialized)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::systemError)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SystemGPIOController::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemGPIOController::systemStatusUpdated)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SystemGPIOController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SystemGPIOController.data,
    qt_meta_data_SystemGPIOController,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SystemGPIOController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SystemGPIOController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SystemGPIOController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SystemGPIOController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SystemGPIOController::yellowLedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SystemGPIOController::greenLedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SystemGPIOController::dvsPowerChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SystemGPIOController::fourGPowerChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SystemGPIOController::systemInitialized(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SystemGPIOController::systemError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SystemGPIOController::systemStatusUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
