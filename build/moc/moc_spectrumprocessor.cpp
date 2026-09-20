/****************************************************************************
** Meta object code from reading C++ file 'spectrumprocessor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/spectrumprocessor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumprocessor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SpectrumProcessor_t {
    QByteArrayData data[15];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpectrumProcessor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpectrumProcessor_t qt_meta_stringdata_SpectrumProcessor = {
    {
QT_MOC_LITERAL(0, 0, 17), // "SpectrumProcessor"
QT_MOC_LITERAL(1, 18, 17), // "specDataProcessed"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 16), // "QVector<int16_t>"
QT_MOC_LITERAL(4, 54, 8), // "spectrum"
QT_MOC_LITERAL(5, 63, 13), // "processorBusy"
QT_MOC_LITERAL(6, 77, 4), // "busy"
QT_MOC_LITERAL(7, 82, 9), // "Fkp_local"
QT_MOC_LITERAL(8, 92, 8), // "uint32_t"
QT_MOC_LITERAL(9, 101, 2), // "dz"
QT_MOC_LITERAL(10, 104, 9), // "freqIndex"
QT_MOC_LITERAL(11, 114, 19), // "processSpectrumData"
QT_MOC_LITERAL(12, 134, 27), // "QVector<QVector<int16_t> >*"
QT_MOC_LITERAL(13, 162, 9), // "bufferPtr"
QT_MOC_LITERAL(14, 172, 11) // "bufferIndex"

    },
    "SpectrumProcessor\0specDataProcessed\0"
    "\0QVector<int16_t>\0spectrum\0processorBusy\0"
    "busy\0Fkp_local\0uint32_t\0dz\0freqIndex\0"
    "processSpectrumData\0QVector<QVector<int16_t> >*\0"
    "bufferPtr\0bufferIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpectrumProcessor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,
       7,    2,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    2,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 8,    9,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,   14,

       0        // eod
};

void SpectrumProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpectrumProcessor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->specDataProcessed((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 1: _t->processorBusy((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->Fkp_local((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 3: _t->processSpectrumData((*reinterpret_cast< QVector<QVector<int16_t> >*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SpectrumProcessor::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SpectrumProcessor::specDataProcessed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SpectrumProcessor::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SpectrumProcessor::processorBusy)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SpectrumProcessor::*)(uint32_t , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SpectrumProcessor::Fkp_local)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpectrumProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SpectrumProcessor.data,
    qt_meta_data_SpectrumProcessor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpectrumProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpectrumProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpectrumProcessor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SpectrumProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SpectrumProcessor::specDataProcessed(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SpectrumProcessor::processorBusy(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SpectrumProcessor::Fkp_local(uint32_t _t1, uint32_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
