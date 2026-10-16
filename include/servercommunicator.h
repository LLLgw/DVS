#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
//#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QElapsedTimer>
#include <QByteArray>
#include <QString>
#include "networkconfigmanager.h"

class ServerCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommunicator(QObject *parent = nullptr);
    void connectToServer();
    bool configureKeepalive();
    bool verifyConfiguration();

signals:
    void svrConnected();
    void svrDisconnected();
    void transferCommandToFpga(QByteArray);
    void svrCmdReady(QByteArray);
    void svrStartTest();
    void svrstopTest();
    void appLogIn();
    void appLogOut();
    void sdRawRecordSetRequested(quint32 enabled);
    void sdRawRecordStatusRequested();
    void sdRawFileListRequested(quint32 count);
    void udpReportTargetSetRequested(const QString &ip, quint16 port);
    
    
private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void attemptReconnect();
    void sendHeartbeat();
    void onProbeConnected();
    void onProbeError(QAbstractSocket::SocketError error);
    void onProbeTimeout();
    void onTcpSwitchCommitTimeout();
    void onStartupPendingTimeout();

public slots:
    void sendSvrCmd(QByteArray packet);
    void sendSdRawRecordSetResult(quint32 result);
    void sendSdRawRecordStatus(quint32 status);
    void sendSdRawFileListItem(quint32 sizeMb, const QString& fileName);
    void sendSdRawFileListEnd();
    void sendUdpReportTargetSetResult(quint32 result);
    void Send_local(uint32_t hz,uint32_t location); // 改

private:
    QByteArray buildSdFileListCommand(quint32 sizeMb, const QString& fileName);
    bool parseIPv4AndPortParam(const QByteArray &frame, QHostAddress &addr, quint16 &port) const;
    void sendTcpUpstreamAck(quint32 result);
    void startTcpUpstreamProbe(const QHostAddress &newAddr, quint16 newPort);
    void clearProbeSocket();
    void applyTcpUpstreamAfterProbeSuccess();
    void commitTcpUpstreamSwitch();
    void rollbackTcpUpstreamSwitch(const QString &reason);
    void startSwitchToAddress(const QHostAddress &addr, quint16 port);
    void resetReconnectTimer();
    QTimer *heartbeatTimer;
    // === 新增：TCP 接收缓存，用于处理粘包/半包 ===
    QByteArray m_rxBuf;

    // === 协议常量（按你现有代码 & 日志：固定 44 字节/帧，帧头 0x5A 0x2C）===
    static constexpr int kSvrFrameLen = 44;
    static constexpr char kFrameHeader0 = 0x5A;
    static constexpr char kFrameHeader1 = 0x2C;

    QTcpSocket *svrCmdSocket;
    QTcpSocket *probeSocket;
    QTimer *tcpReconnectTimer;
    QTimer *probeTimer;
    QTimer *tcpSwitchCommitTimer;
    QTimer *startupPendingTimer;
    QElapsedTimer connectionTimer;

    QHostAddress svrAddress;
    quint16 svrCmdPort;
    QHostAddress pendingSwitchAddress;
    quint16 pendingSwitchPort;
    QHostAddress startupFallbackAddress;
    quint16 startupFallbackPort;
    bool tcpSwitchPending;
    bool startupPendingRecovery;
    bool tcpProbeRunning;
    bool switchingByManager;

    NetworkConfig m_networkConfig;
    int reconnectAttempts;
    const int maxReconnectAttempts = 360;
};

#endif // SERVERCOMMUNICATOR_H
