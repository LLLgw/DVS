/****************************************************************************
** Meta object code from reading C++ file 'spectrumtransmitter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/spectrumtransmitter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumtransmitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SpectrumTransmitter_t {
    QByteArrayData data[10];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpectrumTransmitter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpectrumTransmitter_t qt_meta_stringdata_SpectrumTransmitter = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SpectrumTransmitter"
QT_MOC_LITERAL(1, 20, 16), // "sendSpectrumData"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QVector<int16_t>"
QT_MOC_LITERAL(4, 55, 8), // "spectrum"
QT_MOC_LITERAL(5, 64, 18), // "sendTransferedData"
QT_MOC_LITERAL(6, 83, 6), // "packet"
QT_MOC_LITERAL(7, 90, 18), // "updateReportTarget"
QT_MOC_LITERAL(8, 109, 2), // "ip"
QT_MOC_LITERAL(9, 112, 4) // "port"

    },
    "SpectrumTransmitter\0sendSpectrumData\0"
    "\0QVector<int16_t>\0spectrum\0"
    "sendTransferedData\0packet\0updateReportTarget\0"
    "ip\0port"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpectrumTransmitter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       5,    1,   32,    2, 0x0a /* Public */,
       7,    2,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,    8,    9,

       0        // eod
};

void SpectrumTransmitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpectrumTransmitter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sendSpectrumData((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 1: _t->sendTransferedData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 2: _t->updateReportTarget((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpectrumTransmitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SpectrumTransmitter.data,
    qt_meta_data_SpectrumTransmitter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpectrumTransmitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpectrumTransmitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpectrumTransmitter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SpectrumTransmitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
