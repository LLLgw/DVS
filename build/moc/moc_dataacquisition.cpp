/****************************************************************************
** Meta object code from reading C++ file 'dataacquisition.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/dataacquisition.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dataacquisition.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataAcquisition_t {
    QByteArrayData data[31];
    char stringdata0[441];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataAcquisition_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataAcquisition_t qt_meta_stringdata_DataAcquisition = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DataAcquisition"
QT_MOC_LITERAL(1, 16, 16), // "specRawDataReady"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 27), // "QVector<QVector<int16_t> >*"
QT_MOC_LITERAL(4, 62, 10), // "rawDataPtr"
QT_MOC_LITERAL(5, 73, 11), // "bufferIndex"
QT_MOC_LITERAL(6, 85, 15), // "rmsRawDataReady"
QT_MOC_LITERAL(7, 101, 16), // "QVector<int16_t>"
QT_MOC_LITERAL(8, 118, 7), // "rawData"
QT_MOC_LITERAL(9, 126, 19), // "displayRawDataReady"
QT_MOC_LITERAL(10, 146, 19), // "transferedDataReady"
QT_MOC_LITERAL(11, 166, 6), // "packet"
QT_MOC_LITERAL(12, 173, 20), // "sdRawRecordSetResult"
QT_MOC_LITERAL(13, 194, 6), // "result"
QT_MOC_LITERAL(14, 201, 17), // "sdRawRecordStatus"
QT_MOC_LITERAL(15, 219, 6), // "status"
QT_MOC_LITERAL(16, 226, 17), // "sdRawFileListItem"
QT_MOC_LITERAL(17, 244, 6), // "sizeMb"
QT_MOC_LITERAL(18, 251, 8), // "fileName"
QT_MOC_LITERAL(19, 260, 16), // "sdRawFileListEnd"
QT_MOC_LITERAL(20, 277, 16), // "startAcquisition"
QT_MOC_LITERAL(21, 294, 15), // "stopAcquisition"
QT_MOC_LITERAL(22, 310, 15), // "processDatagram"
QT_MOC_LITERAL(23, 326, 15), // "onProcessorBusy"
QT_MOC_LITERAL(24, 342, 4), // "busy"
QT_MOC_LITERAL(25, 347, 19), // "pollSdRecordCommand"
QT_MOC_LITERAL(26, 367, 17), // "setSdRawRecording"
QT_MOC_LITERAL(27, 385, 7), // "enabled"
QT_MOC_LITERAL(28, 393, 22), // "querySdRawRecordStatus"
QT_MOC_LITERAL(29, 416, 18), // "querySdRawFileList"
QT_MOC_LITERAL(30, 435, 5) // "count"

    },
    "DataAcquisition\0specRawDataReady\0\0"
    "QVector<QVector<int16_t> >*\0rawDataPtr\0"
    "bufferIndex\0rmsRawDataReady\0"
    "QVector<int16_t>\0rawData\0displayRawDataReady\0"
    "transferedDataReady\0packet\0"
    "sdRawRecordSetResult\0result\0"
    "sdRawRecordStatus\0status\0sdRawFileListItem\0"
    "sizeMb\0fileName\0sdRawFileListEnd\0"
    "startAcquisition\0stopAcquisition\0"
    "processDatagram\0onProcessorBusy\0busy\0"
    "pollSdRecordCommand\0setSdRawRecording\0"
    "enabled\0querySdRawRecordStatus\0"
    "querySdRawFileList\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataAcquisition[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   94,    2, 0x06 /* Public */,
       6,    1,   99,    2, 0x06 /* Public */,
       9,    1,  102,    2, 0x06 /* Public */,
      10,    1,  105,    2, 0x06 /* Public */,
      12,    1,  108,    2, 0x06 /* Public */,
      14,    1,  111,    2, 0x06 /* Public */,
      16,    2,  114,    2, 0x06 /* Public */,
      19,    0,  119,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      20,    0,  120,    2, 0x0a /* Public */,
      21,    0,  121,    2, 0x0a /* Public */,
      22,    0,  122,    2, 0x0a /* Public */,
      23,    1,  123,    2, 0x0a /* Public */,
      25,    0,  126,    2, 0x0a /* Public */,
      26,    1,  127,    2, 0x0a /* Public */,
      28,    0,  130,    2, 0x0a /* Public */,
      29,    1,  131,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QByteArray,   11,
    QMetaType::Void, QMetaType::UInt,   13,
    QMetaType::Void, QMetaType::UInt,   15,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,   17,   18,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   27,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   30,

       0        // eod
};

void DataAcquisition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataAcquisition *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->specRawDataReady((*reinterpret_cast< QVector<QVector<int16_t> >*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->rmsRawDataReady((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 2: _t->displayRawDataReady((*reinterpret_cast< const QVector<int16_t>(*)>(_a[1]))); break;
        case 3: _t->transferedDataReady((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 4: _t->sdRawRecordSetResult((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 5: _t->sdRawRecordStatus((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 6: _t->sdRawFileListItem((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->sdRawFileListEnd(); break;
        case 8: _t->startAcquisition(); break;
        case 9: _t->stopAcquisition(); break;
        case 10: _t->processDatagram(); break;
        case 11: _t->onProcessorBusy((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->pollSdRecordCommand(); break;
        case 13: _t->setSdRawRecording((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 14: _t->querySdRawRecordStatus(); break;
        case 15: _t->querySdRawFileList((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataAcquisition::*)(QVector<QVector<int16_t>> * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::specRawDataReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::rmsRawDataReady)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(const QVector<int16_t> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::displayRawDataReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::transferedDataReady)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(quint32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::sdRawRecordSetResult)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(quint32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::sdRawRecordStatus)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)(quint32 , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::sdRawFileListItem)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (DataAcquisition::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataAcquisition::sdRawFileListEnd)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataAcquisition::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataAcquisition.data,
    qt_meta_data_DataAcquisition,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataAcquisition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataAcquisition::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataAcquisition.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataAcquisition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void DataAcquisition::specRawDataReady(QVector<QVector<int16_t>> * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataAcquisition::rmsRawDataReady(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DataAcquisition::displayRawDataReady(const QVector<int16_t> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DataAcquisition::transferedDataReady(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DataAcquisition::sdRawRecordSetResult(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DataAcquisition::sdRawRecordStatus(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DataAcquisition::sdRawFileListItem(quint32 _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DataAcquisition::sdRawFileListEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
