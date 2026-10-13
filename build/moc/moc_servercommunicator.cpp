/****************************************************************************
** Meta object code from reading C++ file 'servercommunicator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/servercommunicator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'servercommunicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ServerCommunicator_t {
    QByteArrayData data[47];
    char stringdata0[684];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ServerCommunicator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ServerCommunicator_t qt_meta_stringdata_ServerCommunicator = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ServerCommunicator"
QT_MOC_LITERAL(1, 19, 12), // "svrConnected"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 15), // "svrDisconnected"
QT_MOC_LITERAL(4, 49, 21), // "transferCommandToFpga"
QT_MOC_LITERAL(5, 71, 11), // "svrCmdReady"
QT_MOC_LITERAL(6, 83, 12), // "svrStartTest"
QT_MOC_LITERAL(7, 96, 11), // "svrstopTest"
QT_MOC_LITERAL(8, 108, 8), // "appLogIn"
QT_MOC_LITERAL(9, 117, 9), // "appLogOut"
QT_MOC_LITERAL(10, 127, 23), // "sdRawRecordSetRequested"
QT_MOC_LITERAL(11, 151, 7), // "enabled"
QT_MOC_LITERAL(12, 159, 26), // "sdRawRecordStatusRequested"
QT_MOC_LITERAL(13, 186, 22), // "sdRawFileListRequested"
QT_MOC_LITERAL(14, 209, 5), // "count"
QT_MOC_LITERAL(15, 215, 26), // "sdRawFileTransmitRequested"
QT_MOC_LITERAL(16, 242, 8), // "fileName"
QT_MOC_LITERAL(17, 251, 27), // "udpReportTargetSetRequested"
QT_MOC_LITERAL(18, 279, 2), // "ip"
QT_MOC_LITERAL(19, 282, 4), // "port"
QT_MOC_LITERAL(20, 287, 11), // "onConnected"
QT_MOC_LITERAL(21, 299, 11), // "onReadyRead"
QT_MOC_LITERAL(22, 311, 14), // "onDisconnected"
QT_MOC_LITERAL(23, 326, 7), // "onError"
QT_MOC_LITERAL(24, 334, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(25, 363, 5), // "error"
QT_MOC_LITERAL(26, 369, 16), // "attemptReconnect"
QT_MOC_LITERAL(27, 386, 13), // "sendHeartbeat"
QT_MOC_LITERAL(28, 400, 16), // "onProbeConnected"
QT_MOC_LITERAL(29, 417, 12), // "onProbeError"
QT_MOC_LITERAL(30, 430, 14), // "onProbeTimeout"
QT_MOC_LITERAL(31, 445, 24), // "onTcpSwitchCommitTimeout"
QT_MOC_LITERAL(32, 470, 23), // "onStartupPendingTimeout"
QT_MOC_LITERAL(33, 494, 10), // "sendSvrCmd"
QT_MOC_LITERAL(34, 505, 6), // "packet"
QT_MOC_LITERAL(35, 512, 24), // "sendSdRawRecordSetResult"
QT_MOC_LITERAL(36, 537, 6), // "result"
QT_MOC_LITERAL(37, 544, 21), // "sendSdRawRecordStatus"
QT_MOC_LITERAL(38, 566, 6), // "status"
QT_MOC_LITERAL(39, 573, 21), // "sendSdRawFileListItem"
QT_MOC_LITERAL(40, 595, 6), // "sizeMb"
QT_MOC_LITERAL(41, 602, 20), // "sendSdRawFileListEnd"
QT_MOC_LITERAL(42, 623, 28), // "sendUdpReportTargetSetResult"
QT_MOC_LITERAL(43, 652, 10), // "Send_local"
QT_MOC_LITERAL(44, 663, 8), // "uint32_t"
QT_MOC_LITERAL(45, 672, 2), // "hz"
QT_MOC_LITERAL(46, 675, 8) // "location"

    },
    "ServerCommunicator\0svrConnected\0\0"
    "svrDisconnected\0transferCommandToFpga\0"
    "svrCmdReady\0svrStartTest\0svrstopTest\0"
    "appLogIn\0appLogOut\0sdRawRecordSetRequested\0"
    "enabled\0sdRawRecordStatusRequested\0"
    "sdRawFileListRequested\0count\0"
    "sdRawFileTransmitRequested\0fileName\0"
    "udpReportTargetSetRequested\0ip\0port\0"
    "onConnected\0onReadyRead\0onDisconnected\0"
    "onError\0QAbstractSocket::SocketError\0"
    "error\0attemptReconnect\0sendHeartbeat\0"
    "onProbeConnected\0onProbeError\0"
    "onProbeTimeout\0onTcpSwitchCommitTimeout\0"
    "onStartupPendingTimeout\0sendSvrCmd\0"
    "packet\0sendSdRawRecordSetResult\0result\0"
    "sendSdRawRecordStatus\0status\0"
    "sendSdRawFileListItem\0sizeMb\0"
    "sendSdRawFileListEnd\0sendUdpReportTargetSetResult\0"
    "Send_local\0uint32_t\0hz\0location"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ServerCommunicator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  169,    2, 0x06 /* Public */,
       3,    0,  170,    2, 0x06 /* Public */,
       4,    1,  171,    2, 0x06 /* Public */,
       5,    1,  174,    2, 0x06 /* Public */,
       6,    0,  177,    2, 0x06 /* Public */,
       7,    0,  178,    2, 0x06 /* Public */,
       8,    0,  179,    2, 0x06 /* Public */,
       9,    0,  180,    2, 0x06 /* Public */,
      10,    1,  181,    2, 0x06 /* Public */,
      12,    0,  184,    2, 0x06 /* Public */,
      13,    1,  185,    2, 0x06 /* Public */,
      15,    1,  188,    2, 0x06 /* Public */,
      17,    2,  191,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      20,    0,  196,    2, 0x08 /* Private */,
      21,    0,  197,    2, 0x08 /* Private */,
      22,    0,  198,    2, 0x08 /* Private */,
      23,    1,  199,    2, 0x08 /* Private */,
      26,    0,  202,    2, 0x08 /* Private */,
      27,    0,  203,    2, 0x08 /* Private */,
      28,    0,  204,    2, 0x08 /* Private */,
      29,    1,  205,    2, 0x08 /* Private */,
      30,    0,  208,    2, 0x08 /* Private */,
      31,    0,  209,    2, 0x08 /* Private */,
      32,    0,  210,    2, 0x08 /* Private */,
      33,    1,  211,    2, 0x0a /* Public */,
      35,    1,  214,    2, 0x0a /* Public */,
      37,    1,  217,    2, 0x0a /* Public */,
      39,    2,  220,    2, 0x0a /* Public */,
      41,    0,  225,    2, 0x0a /* Public */,
      42,    1,  226,    2, 0x0a /* Public */,
      43,    2,  229,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,   18,   19,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   34,
    QMetaType::Void, QMetaType::UInt,   36,
    QMetaType::Void, QMetaType::UInt,   38,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,   40,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   36,
    QMetaType::Void, 0x80000000 | 44, 0x80000000 | 44,   45,   46,

       0        // eod
};

void ServerCommunicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ServerCommunicator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->svrConnected(); break;
        case 1: _t->svrDisconnected(); break;
        case 2: _t->transferCommandToFpga((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: _t->svrCmdReady((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->svrStartTest(); break;
        case 5: _t->svrstopTest(); break;
        case 6: _t->appLogIn(); break;
        case 7: _t->appLogOut(); break;
        case 8: _t->sdRawRecordSetRequested((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 9: _t->sdRawRecordStatusRequested(); break;
        case 10: _t->sdRawFileListRequested((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 11: _t->sdRawFileTransmitRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->udpReportTargetSetRequested((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 13: _t->onConnected(); break;
        case 14: _t->onReadyRead(); break;
        case 15: _t->onDisconnected(); break;
        case 16: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 17: _t->attemptReconnect(); break;
        case 18: _t->sendHeartbeat(); break;
        case 19: _t->onProbeConnected(); break;
        case 20: _t->onProbeError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 21: _t->onProbeTimeout(); break;
        case 22: _t->onTcpSwitchCommitTimeout(); break;
        case 23: _t->onStartupPendingTimeout(); break;
        case 24: _t->sendSvrCmd((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 25: _t->sendSdRawRecordSetResult((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 26: _t->sendSdRawRecordStatus((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 27: _t->sendSdRawFileListItem((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 28: _t->sendSdRawFileListEnd(); break;
        case 29: _t->sendUdpReportTargetSetResult((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 30: _t->Send_local((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::svrConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::svrDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::transferCommandToFpga)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::svrCmdReady)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::svrStartTest)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::svrstopTest)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::appLogIn)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::appLogOut)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(quint32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::sdRawRecordSetRequested)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::sdRawRecordStatusRequested)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(quint32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::sdRawFileListRequested)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::sdRawFileTransmitRequested)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ServerCommunicator::*)(const QString & , quint16 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::udpReportTargetSetRequested)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ServerCommunicator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ServerCommunicator.data,
    qt_meta_data_ServerCommunicator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ServerCommunicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ServerCommunicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ServerCommunicator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ServerCommunicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    return _id;
}

// SIGNAL 0
void ServerCommunicator::svrConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ServerCommunicator::svrDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ServerCommunicator::transferCommandToFpga(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ServerCommunicator::svrCmdReady(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ServerCommunicator::svrStartTest()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ServerCommunicator::svrstopTest()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ServerCommunicator::appLogIn()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ServerCommunicator::appLogOut()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ServerCommunicator::sdRawRecordSetRequested(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ServerCommunicator::sdRawRecordStatusRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ServerCommunicator::sdRawFileListRequested(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ServerCommunicator::sdRawFileTransmitRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ServerCommunicator::udpReportTargetSetRequested(const QString & _t1, quint16 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
