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
    QByteArrayData data[46];
    char stringdata0[657];
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
QT_MOC_LITERAL(15, 215, 27), // "udpReportTargetSetRequested"
QT_MOC_LITERAL(16, 243, 2), // "ip"
QT_MOC_LITERAL(17, 246, 4), // "port"
QT_MOC_LITERAL(18, 251, 11), // "onConnected"
QT_MOC_LITERAL(19, 263, 11), // "onReadyRead"
QT_MOC_LITERAL(20, 275, 14), // "onDisconnected"
QT_MOC_LITERAL(21, 290, 7), // "onError"
QT_MOC_LITERAL(22, 298, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(23, 327, 5), // "error"
QT_MOC_LITERAL(24, 333, 16), // "attemptReconnect"
QT_MOC_LITERAL(25, 350, 13), // "sendHeartbeat"
QT_MOC_LITERAL(26, 364, 16), // "onProbeConnected"
QT_MOC_LITERAL(27, 381, 12), // "onProbeError"
QT_MOC_LITERAL(28, 394, 14), // "onProbeTimeout"
QT_MOC_LITERAL(29, 409, 24), // "onTcpSwitchCommitTimeout"
QT_MOC_LITERAL(30, 434, 23), // "onStartupPendingTimeout"
QT_MOC_LITERAL(31, 458, 10), // "sendSvrCmd"
QT_MOC_LITERAL(32, 469, 6), // "packet"
QT_MOC_LITERAL(33, 476, 24), // "sendSdRawRecordSetResult"
QT_MOC_LITERAL(34, 501, 6), // "result"
QT_MOC_LITERAL(35, 508, 21), // "sendSdRawRecordStatus"
QT_MOC_LITERAL(36, 530, 6), // "status"
QT_MOC_LITERAL(37, 537, 21), // "sendSdRawFileListItem"
QT_MOC_LITERAL(38, 559, 6), // "sizeMb"
QT_MOC_LITERAL(39, 566, 8), // "fileName"
QT_MOC_LITERAL(40, 575, 20), // "sendSdRawFileListEnd"
QT_MOC_LITERAL(41, 596, 28), // "sendUdpReportTargetSetResult"
QT_MOC_LITERAL(42, 625, 10), // "Send_local"
QT_MOC_LITERAL(43, 636, 8), // "uint32_t"
QT_MOC_LITERAL(44, 645, 2), // "hz"
QT_MOC_LITERAL(45, 648, 8) // "location"

    },
    "ServerCommunicator\0svrConnected\0\0"
    "svrDisconnected\0transferCommandToFpga\0"
    "svrCmdReady\0svrStartTest\0svrstopTest\0"
    "appLogIn\0appLogOut\0sdRawRecordSetRequested\0"
    "enabled\0sdRawRecordStatusRequested\0"
    "sdRawFileListRequested\0count\0"
    "udpReportTargetSetRequested\0ip\0port\0"
    "onConnected\0onReadyRead\0onDisconnected\0"
    "onError\0QAbstractSocket::SocketError\0"
    "error\0attemptReconnect\0sendHeartbeat\0"
    "onProbeConnected\0onProbeError\0"
    "onProbeTimeout\0onTcpSwitchCommitTimeout\0"
    "onStartupPendingTimeout\0sendSvrCmd\0"
    "packet\0sendSdRawRecordSetResult\0result\0"
    "sendSdRawRecordStatus\0status\0"
    "sendSdRawFileListItem\0sizeMb\0fileName\0"
    "sendSdRawFileListEnd\0sendUdpReportTargetSetResult\0"
    "Send_local\0uint32_t\0hz\0location"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ServerCommunicator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  164,    2, 0x06 /* Public */,
       3,    0,  165,    2, 0x06 /* Public */,
       4,    1,  166,    2, 0x06 /* Public */,
       5,    1,  169,    2, 0x06 /* Public */,
       6,    0,  172,    2, 0x06 /* Public */,
       7,    0,  173,    2, 0x06 /* Public */,
       8,    0,  174,    2, 0x06 /* Public */,
       9,    0,  175,    2, 0x06 /* Public */,
      10,    1,  176,    2, 0x06 /* Public */,
      12,    0,  179,    2, 0x06 /* Public */,
      13,    1,  180,    2, 0x06 /* Public */,
      15,    2,  183,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    0,  188,    2, 0x08 /* Private */,
      19,    0,  189,    2, 0x08 /* Private */,
      20,    0,  190,    2, 0x08 /* Private */,
      21,    1,  191,    2, 0x08 /* Private */,
      24,    0,  194,    2, 0x08 /* Private */,
      25,    0,  195,    2, 0x08 /* Private */,
      26,    0,  196,    2, 0x08 /* Private */,
      27,    1,  197,    2, 0x08 /* Private */,
      28,    0,  200,    2, 0x08 /* Private */,
      29,    0,  201,    2, 0x08 /* Private */,
      30,    0,  202,    2, 0x08 /* Private */,
      31,    1,  203,    2, 0x0a /* Public */,
      33,    1,  206,    2, 0x0a /* Public */,
      35,    1,  209,    2, 0x0a /* Public */,
      37,    2,  212,    2, 0x0a /* Public */,
      40,    0,  217,    2, 0x0a /* Public */,
      41,    1,  218,    2, 0x0a /* Public */,
      42,    2,  221,    2, 0x0a /* Public */,

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
    QMetaType::Void, QMetaType::QString, QMetaType::UShort,   16,   17,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   32,
    QMetaType::Void, QMetaType::UInt,   34,
    QMetaType::Void, QMetaType::UInt,   36,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,   38,   39,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   34,
    QMetaType::Void, 0x80000000 | 43, 0x80000000 | 43,   44,   45,

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
        case 11: _t->udpReportTargetSetRequested((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 12: _t->onConnected(); break;
        case 13: _t->onReadyRead(); break;
        case 14: _t->onDisconnected(); break;
        case 15: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 16: _t->attemptReconnect(); break;
        case 17: _t->sendHeartbeat(); break;
        case 18: _t->onProbeConnected(); break;
        case 19: _t->onProbeError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 20: _t->onProbeTimeout(); break;
        case 21: _t->onTcpSwitchCommitTimeout(); break;
        case 22: _t->onStartupPendingTimeout(); break;
        case 23: _t->sendSvrCmd((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 24: _t->sendSdRawRecordSetResult((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 25: _t->sendSdRawRecordStatus((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 26: _t->sendSdRawFileListItem((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 27: _t->sendSdRawFileListEnd(); break;
        case 28: _t->sendUdpReportTargetSetResult((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 29: _t->Send_local((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 19:
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
            using _t = void (ServerCommunicator::*)(const QString & , quint16 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ServerCommunicator::udpReportTargetSetRequested)) {
                *result = 11;
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
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
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
void ServerCommunicator::udpReportTargetSetRequested(const QString & _t1, quint16 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
