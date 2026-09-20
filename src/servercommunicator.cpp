#include "servercommunicator.h"
#include "message.h"
#include "GlobalVars.h"
#include <QHostAddress>
#include <QDebug>
#include <QtMath>
#include <QAbstractSocket>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <QThread>
#include <QTcpSocket>
#include <unistd.h>   // for ::sync()
/* 服务器通信器 */
ServerCommunicator::ServerCommunicator(QObject *parent)
    : QObject{parent}
{
    m_networkConfig = NetworkConfigManager::load();
    if (!NetworkConfigManager::isValidIpv4Endpoint(m_networkConfig.tcpUpstream.configured.ip,
                                                   m_networkConfig.tcpUpstream.configured.port))
    {
        m_networkConfig = NetworkConfigManager::defaultConfig();
        NetworkConfigManager::save(m_networkConfig);
    }

    svrAddress = QHostAddress(m_networkConfig.tcpUpstream.configured.ip);
    svrCmdPort = m_networkConfig.tcpUpstream.configured.port;
    startupFallbackAddress = QHostAddress(m_networkConfig.tcpUpstream.lastKnownGood.ip);
    startupFallbackPort = m_networkConfig.tcpUpstream.lastKnownGood.port;
    if (!NetworkConfigManager::isValidIpv4Endpoint(m_networkConfig.tcpUpstream.lastKnownGood.ip,
                                                   m_networkConfig.tcpUpstream.lastKnownGood.port))
    {
        startupFallbackAddress = svrAddress;
        startupFallbackPort = svrCmdPort;
    }
    startupPendingRecovery = (m_networkConfig.tcpUpstream.state == NetworkConfigManager::pendingState());

    pendingSwitchPort = 0;
    tcpSwitchPending = false;
    tcpProbeRunning = false;
    switchingByManager = false;

    svrCmdSocket = new QTcpSocket(this);
    probeSocket = nullptr;
    tcpReconnectTimer = new QTimer(this);
    tcpReconnectTimer->setSingleShot(true);
    probeTimer = new QTimer(this);
    probeTimer->setSingleShot(true);
    probeTimer->setInterval(3000);
    tcpSwitchCommitTimer = new QTimer(this);
    tcpSwitchCommitTimer->setSingleShot(true);
    tcpSwitchCommitTimer->setInterval(12000);
    startupPendingTimer = new QTimer(this);
    startupPendingTimer->setSingleShot(true);
    startupPendingTimer->setInterval(12000);
    // [新增] 初始化心跳定时器
    heartbeatTimer = new QTimer(this);
    heartbeatTimer->setInterval(5000); // 设置间隔 15000 毫秒 (15秒)

    // 连接超时信号 -> 发送函数
    connect(heartbeatTimer, &QTimer::timeout, this, &ServerCommunicator::sendHeartbeat);

    // 为了安全，确保 socket 断开信号也能停止定时器 (双重保险)
    /*断开连接时停止心跳*/
    connect(svrCmdSocket, &QTcpSocket::disconnected, heartbeatTimer, &QTimer::stop);
    /*连接时启动心跳*/
    connect(svrCmdSocket, &QTcpSocket::connected, this, &ServerCommunicator::onConnected);
    /*数据就绪时处理*/
    connect(svrCmdSocket, &QTcpSocket::readyRead, this, &ServerCommunicator::onReadyRead);
    /*断开连接时处理*/
    connect(svrCmdSocket, &QTcpSocket::disconnected, this, &ServerCommunicator::onDisconnected);
    /*错误发生时处理*/
    connect(svrCmdSocket, &QTcpSocket::errorOccurred, this, &ServerCommunicator::onError);
    /*重连定时器超时*/
    connect(tcpReconnectTimer, &QTimer::timeout, this, &ServerCommunicator::attemptReconnect);
    /*探针定时器超时*/
    connect(probeTimer, &QTimer::timeout, this, &ServerCommunicator::onProbeTimeout);
    /*TCP切换提交定时器超时*/
    connect(tcpSwitchCommitTimer, &QTimer::timeout, this, &ServerCommunicator::onTcpSwitchCommitTimeout);
    /*启动等待定时器超时*/
    connect(startupPendingTimer, &QTimer::timeout, this, &ServerCommunicator::onStartupPendingTimeout);

    m_rxBuf.clear();
}

/* 配置心跳 */
bool ServerCommunicator::configureKeepalive()
{
    if (svrCmdSocket->state() != QAbstractSocket::ConnectedState)
    {
        qWarning() << "Socket is not connected, cannot configure keepalive";
        return false;
    }

    int fd = svrCmdSocket->socketDescriptor();

    if (fd == -1)
    {
        qWarning() << "Invalid socket descriptor";
        return false;
    }

    int enable = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)) != 0)
    {
        perror("setsockopt SO_KEEPALIVE disable failed");
        return false;
    }

    // 60秒空闲，10秒间隔，3次探测
    int idle = 60;
    int interval = 10;
    int count = 3;

    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) != 0)
    {
        perror("setsockopt TCP_KEEPIDLE failed");
        return false;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) != 0)
    {
        perror("setsockopt TCP_KEEPINTVL failed");
        return false;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count)) != 0)
    {
        perror("setsockopt TCP_KEEPCNT failed");
        return false;
    }

    enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)) != 0)
    {
        perror("setsockopt SO_KEEPALIVE enable failed");
        return false;
    }

    return true;
}

bool ServerCommunicator::verifyConfiguration()
{
    int fd = svrCmdSocket->socketDescriptor();
    if (fd == -1)
        return false;
    int actual_idle, actual_interval, actual_count;
    socklen_t len = sizeof(actual_idle);
    if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &actual_idle, &len) != 0)
        return false;

    len = sizeof(actual_interval);
    if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &actual_interval, &len) != 0)
        return false;

    len = sizeof(actual_count);
    if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &actual_count, &len) != 0)
        return false;

    qDebug() << "Verified settings - Idle:" << actual_idle << "s, Interval:" << actual_interval << "s, Count:" << actual_count;
    return (actual_idle == 60 && actual_interval == 10 && actual_count == 3);
}

/*连接到服务器*/
void ServerCommunicator::connectToServer()
{
    if (svrCmdSocket->state() != QAbstractSocket::UnconnectedState)
    {
        return;
    }

    connectionTimer.start();
    qDebug() << "Connecting to server..." << svrAddress.toString() << ":" << svrCmdPort;

    if (startupPendingRecovery && !startupPendingTimer->isActive())
        startupPendingTimer->start();

    svrCmdSocket->connectToHost(svrAddress, svrCmdPort);
}

void ServerCommunicator::onConnected()
{
    if (configureKeepalive())
        qDebug() << "Keepalive configured successfully";
    else
        qWarning() << "Failed to configure keepalive";

    if (verifyConfiguration())
        qDebug() << "Keepalive set result verified right";
    else
        qWarning() << "Keepalive set result verified error";

    reconnectAttempts = 0;
    tcpReconnectTimer->stop();
    qDebug() << "Connected to server in:" << connectionTimer.elapsed() << "ms";

    // 连接成功后清空接收缓存，避免上次残留导致错帧
    m_rxBuf.clear();

    QByteArray svrCmd(44, 0x00);
    svrCmd = buildServerCommand(SCMD_DEV_LOG_REQ, 0);

    qDebug() << svrCmd.toHex();
    if(svrCmdSocket->write(svrCmd) != svrCmd.size())
    {
        qWarning() << "Failed to send complete data";
    }
    qDebug() << "Log to server!";
// [新增] 连接成功，开始发送心跳
    if (!heartbeatTimer->isActive()) {
        heartbeatTimer->start();
        qDebug() << "Heartbeat timer started (5s interval).";
    }
}

void ServerCommunicator::onReadyRead()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);

    if (svrCmdSocket->bytesAvailable() <= 0)
        return;

    // 1) 读取本次到达的所有字节，追加到缓存（处理粘包/半包）
    QByteArray chunk = svrCmdSocket->readAll();
    if (chunk.isEmpty())
        return;

    qDebug() << "Received" << chunk.size() << "bytes:" << chunk.toHex();
    m_rxBuf += chunk;

    // 2) 从缓存中循环拆帧：对齐帧头(0x5A 0x2C) + 固定长度 44 字节
    //    注意：TCP 可能半包，也可能一次到多帧（例如 176=44*4）
    while (true)
    {
        if (m_rxBuf.size() < 2)
            return;

        // 找帧头（避免错位）
        int headerPos = -1;
        for (int i = 0; i + 1 < m_rxBuf.size(); ++i)
        {
            if ((unsigned char)m_rxBuf[i] == (unsigned char)kFrameHeader0 &&
                (unsigned char)m_rxBuf[i + 1] == (unsigned char)kFrameHeader1)
            {
                headerPos = i;
                break;
            }
        }

        if (headerPos < 0)
        {
            // 缓冲里没有帧头：保留最后 1 字节（防止帧头跨边界）
            if (m_rxBuf.size() > 1)
                m_rxBuf = m_rxBuf.right(1);
            return;
        }

        // 丢弃帧头前的脏数据
        if (headerPos > 0)
            m_rxBuf.remove(0, headerPos);

        // 帧头对齐后，不够一帧则等待下次 readyRead 拼齐
        if (m_rxBuf.size() < kSvrFrameLen)
            return;

        // 取一帧
        QByteArray oneFrame = m_rxBuf.left(kSvrFrameLen);
        m_rxBuf.remove(0, kSvrFrameLen);

        // 3) 解析这一帧（这里传 oneFrame，而不是 chunk/readAll 的全部）
        svrcmdstruct_t svrcmd;
        quint32 cmdParam;
        bool responseReceived = ::parseServerCommand(oneFrame, svrcmd);

        if (!responseReceived)
        {
            // 解析失败：通常是协议错位/校验失败。此处打印并继续找下一帧头
            qWarning() << "parseServerCommand failed. frame =" << oneFrame.toHex();
            // 继续 while，看缓存里是否还有下一帧
            continue;
        }

        QByteArray fpgaCmd(24, 0x00);
        QByteArray svrCmd(44, 0x00);

        qDebug() << "command code:" << svrcmd.cmdCode;

        // ===== 以下 switch 逻辑保持你原样 =====
        switch (svrcmd.cmdCode)
        {
        case SCMD_DEV_LOG_OK:
            qDebug() << "log ok";

            if (tcpSwitchPending)
                commitTcpUpstreamSwitch();

            if (startupPendingRecovery)
            {
                startupPendingRecovery = false;
                startupPendingTimer->stop();

                m_networkConfig = NetworkConfigManager::load();
                m_networkConfig.tcpUpstream.configured.ip = svrAddress.toString();
                m_networkConfig.tcpUpstream.configured.port = svrCmdPort;
                m_networkConfig.tcpUpstream.lastKnownGood = m_networkConfig.tcpUpstream.configured;
                m_networkConfig.tcpUpstream.state = NetworkConfigManager::committedState();
                NetworkConfigManager::save(m_networkConfig);
            }

            emit svrConnected();
            // 注意：这里不要 return，否则会丢掉同一批到达的后续帧
            break;

        case SCMD_DEV_LOG_NOK:
            qDebug() << "log nok";
            break;

        case SCMD_APP_LOG_IN_NOTIFY:
            qDebug() << "APP log in";
            svrCmd = buildServerCommand(SCMD_APP_LOG_IN_ACK, 1);
            if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            qDebug() << svrCmd.toHex();
            qDebug() << "Command echoed back to server";
            emit appLogIn();
            break;

        case SCMD_APP_LOG_OUT_NOTIFY:
            qDebug() << "APP log out";
            svrCmd = buildServerCommand(SCMD_APP_LOG_OUT_ACK, 1);
            if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            qDebug() << svrCmd.toHex();
            qDebug() << "Command echoed back to server";
            emit appLogOut();
            break;

        case SCMD_CONFIG_PULSE_WIDTH_REQ:
            if(!GlobalVars::fpgaInited)
            {
                svrCmd = buildServerCommand(SCMD_CONFIG_PULSE_WIDTH_ACK, 0);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                {
                    qWarning() << "Failed to send complete data";
                }
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            else
            {
                if((svrcmd.param < 40) || (svrcmd.param > 1000))
                {
                    svrCmd = buildServerCommand(SCMD_CONFIG_PULSE_WIDTH_ACK, 0);
                    if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                    {
                        qWarning() << "Failed to send complete data";
                    }
                    qDebug() << svrCmd.toHex();
                    qDebug() << "Command echoed back to server";
                }
                else if(svrcmd.param % 10 != 0)
                {
                    svrCmd = buildServerCommand(SCMD_CONFIG_PULSE_WIDTH_ACK, 0);
                    if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                    {
                        qWarning() << "Failed to send complete data";
                    }
                    qDebug() << svrCmd.toHex();
                    qDebug() << "Command echoed back to server";
                }
                else
                {
                    cmdParam = svrcmd.param;
                    fpgaCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_PULSE_WIDTH, cmdParam);
                    qDebug() << fpgaCmd.toHex();
                    emit transferCommandToFpga(fpgaCmd);
                }
            }
            break;

        case SCMD_CONFIG_SPECTRUM_LOW_FREQUENCY_REQ:
            if((svrcmd.param < 10) || (svrcmd.param > 80))
            {
                svrCmd = buildServerCommand(SCMD_CONFIG_SPECTRUM_LOW_FREQUENCY_ACK, 0);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            else
            {
                GlobalVars::lowCutFrequency = svrcmd.param;
                svrCmd = buildServerCommand(SCMD_CONFIG_SPECTRUM_LOW_FREQUENCY_ACK, 1);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                        qWarning() << "Failed to send complete data";
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            break;

        case SCMD_CONFIG_SPECTRUM_HIGH_FREQUENCY_REQ:
            if((svrcmd.param < 110) || (svrcmd.param > 180))
            {
                svrCmd = buildServerCommand(SCMD_CONFIG_SPECTRUM_HIGH_FREQUENCY_ACK, 0);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            else
            {
                GlobalVars::highCutFrequency = svrcmd.param;
                svrCmd = buildServerCommand(SCMD_CONFIG_SPECTRUM_HIGH_FREQUENCY_ACK, 1);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            break;

        case SCMD_START_TEST_REQ:
            if(!GlobalVars::fpgaInited)
            {
                svrCmd = buildServerCommand(SCMD_START_TEST_ACK, 0);
                if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                {
                    qWarning() << "Failed to send complete data";
                }
                qDebug() << svrCmd.toHex();
                qDebug() << "Command echoed back to server";
            }
            else
            {
                if(GlobalVars::systemState == SYSTEM_IDLE)
                {
                    GlobalVars::systemState = SERVER_START_TEST;
                    cmdParam = 1;
                    fpgaCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_TEST, cmdParam);
                    qDebug()<<fpgaCmd.toHex();
                    emit transferCommandToFpga(fpgaCmd);
                    emit svrStartTest();
                }
            }
            break;

        case SCMD_STOP_TEST_REQ:

            if(!GlobalVars::fpgaInited)
            {
                svrCmd = buildServerCommand(SCMD_STOP_TEST_ACK, 0);
            }
            else
            {
                if(GlobalVars::systemState == SYSTEM_TEST)
                {
                    GlobalVars::systemState = SERVER_STOP_TEST;
                    cmdParam = 0;
                    fpgaCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_TEST, cmdParam);
                    qDebug()<<fpgaCmd.toHex();
                    emit transferCommandToFpga(fpgaCmd);
                    emit svrstopTest();
                }
            }
            break;

        case SCMD_ENABLE_GET_RMS_REQ:
            GlobalVars::rmsDataTransmitEnable = true;
            svrCmd = buildServerCommand(SCMD_ENABLE_GET_RMS_ACK, 1);
            if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            qDebug() << svrCmd.toHex();
            qDebug() << "Command echoed back to server";
            break;

        case SCMD_DISABLE_GET_RMS_REQ:
            GlobalVars::rmsDataTransmitEnable = false;
            svrCmd = buildServerCommand(SCMD_DISABLE_GET_RMS_ACK, 1);
            if(svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            qDebug() << svrCmd.toHex();
            qDebug() << "Command echoed back to server";
            break;

            // 【新增】处理 0x38 设置上报数据长度指令
        case SCMD_SET_UPLOAD_DATA_LEN_REQ:
            // 校验参数是否在 1-2 范围内
            if (svrcmd.param == 1 || svrcmd.param == 2)
            {
                GlobalVars::uploadDataByteLength = svrcmd.param;              // 更新全局变量
                svrCmd = buildServerCommand(SCMD_SET_UPLOAD_DATA_LEN_ACK, 1); // 1: 成功
                qDebug() << "Set upload data length to" << svrcmd.param << "bytes";
            }
            else
            {
                svrCmd = buildServerCommand(SCMD_SET_UPLOAD_DATA_LEN_ACK, 0); // 0: 失败
                qWarning() << "Invalid upload data length parameter:" << svrcmd.param;
            }
            if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            break;

            // 修改已有的 0x40 降采样指令，增加互斥重置逻辑
        case SCMD_SET_RESOLUTION_REQ:
            if (svrcmd.param >= 1 && svrcmd.param <= 5)
            {
                GlobalVars::spatialResolution = svrcmd.param;
                // 【互斥】开启降采样时，恢复满量程
                GlobalVars::truncateDistance = 20000;
                svrCmd = buildServerCommand(SCMD_SET_RESOLUTION_ACK, 1);
                qDebug() << "Set spatial resolution to" << svrcmd.param << "m, truncate distance reset to 20000m";
            }
            else
            {
                svrCmd = buildServerCommand(SCMD_SET_RESOLUTION_ACK, 0);
            }
            if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            break;

        // 【新增】处理 0x42 设置截断距离指令
        case SCMD_SET_TRUNCATE_DIST_REQ:
            if (svrcmd.param >= 10 && svrcmd.param <= 20000)
            {
                GlobalVars::truncateDistance = svrcmd.param;
                // 【互斥】设置截断距离时，自动恢复1m1个点
                GlobalVars::spatialResolution = 1;
                svrCmd = buildServerCommand(SCMD_SET_TRUNCATE_DIST_ACK, 1);
                qDebug() << "Set truncate distance to" << svrcmd.param << "m, resolution reset to 1m";
            }
            else
            {
                svrCmd = buildServerCommand(SCMD_SET_TRUNCATE_DIST_ACK, 0);
            }
            if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                qWarning() << "Failed to send complete data";
            break;

        case SCMD_SET_TCP_UPSTREAM_REQ:
        {
            if (svrcmd.paramLen != 6)
            {
                sendTcpUpstreamAck(0);
                break;
            }

            QHostAddress newAddr;
            quint16 newPort = 0;
            if (!parseIPv4AndPortParam(oneFrame, newAddr, newPort))
            {
                sendTcpUpstreamAck(0);
                break;
            }

            if (tcpProbeRunning || tcpSwitchPending)
            {
                sendTcpUpstreamAck(0);
                break;
            }

            startTcpUpstreamProbe(newAddr, newPort);
            break;
        }

        case SCMD_SET_UDP_REPORT_TARGET_REQ:
        {
            const quint8 paramLen = static_cast<quint8>(oneFrame.at(3));
            if (paramLen != 6)
            {
                svrCmd = buildServerCommand(SCMD_SET_UDP_REPORT_TARGET_ACK, 0);
                if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
                break;
            }

            const quint8 ip0 = static_cast<quint8>(oneFrame.at(4));
            const quint8 ip1 = static_cast<quint8>(oneFrame.at(5));
            const quint8 ip2 = static_cast<quint8>(oneFrame.at(6));
            const quint8 ip3 = static_cast<quint8>(oneFrame.at(7));
            const quint16 port = (static_cast<quint16>(static_cast<quint8>(oneFrame.at(8))) << 8) |
                                 static_cast<quint16>(static_cast<quint8>(oneFrame.at(9)));

            const QString ip = QString::number(ip0) + "." +
                               QString::number(ip1) + "." +
                               QString::number(ip2) + "." +
                               QString::number(ip3);

            QHostAddress addr;
            const bool validIp = addr.setAddress(ip) && addr.protocol() == QAbstractSocket::IPv4Protocol;
            const quint32 ipValue = validIp ? addr.toIPv4Address() : 0;
            const bool validPort = (port != 0);
            const bool validHost = validIp && (ipValue != 0) && (ipValue != 0xFFFFFFFFu);

            if (!validHost || !validPort)
            {
                svrCmd = buildServerCommand(SCMD_SET_UDP_REPORT_TARGET_ACK, 0);
                if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
                break;
            }

            emit udpReportTargetSetRequested(ip, port);
            break;
        }

            // [新增] 收到服务器的心跳应答 (0x51)
        case SCMD_HEARTBEAT_ACK:
            // 可以在这里重置一个“超时看门狗”，或者什么都不做
            // qDebug() << "Heartbeat ACK (0x51) received";
            break;
        case SCMD_DEVICE_REBOOT_REQ:   // 0x52
        {
            qDebug() << "Reboot command received";

            // （可选）先回一个 ACK，避免服务器以为没收到
            QByteArray svrCmd = buildServerCommand(SCMD_DEVICE_REBOOT_ACK, 1);
            svrCmdSocket->write(svrCmd);
            svrCmdSocket->flush();

            // 给 ACK 一点时间发出去（很重要）
            QThread::msleep(100);

            // 立刻重启系统（需要 root）
            ::sync();
            ::system("reboot");

            break; // 实际上执行不到
        }
        case SCMD_OTA_UPDATE_REQ:
        {
            qDebug() << "OTA Update command (0x54) received. Start downloading...";

            // 1. 下载到临时文件 (作为母本)
            // 使用您测试成功的 URL，并设置 120秒超时
            const char* downloadCmd = "wget -O /root/DVS_download http://8.138.101.239:47697/ota/DVS -T 120";

            qDebug() << "Executing:" << downloadCmd;
            int ret = ::system(downloadCmd);

            // 检查下载是否成功 (返回值 0 为成功)
            if (ret != 0)
            {
                qWarning() << "OTA Download failed! Ret:" << ret;
                // 回复失败 ACK (0x55, 参数 0)
                svrCmd = buildServerCommand(SCMD_OTA_UPDATE_ACK, 0);
                svrCmdSocket->write(svrCmd);
                break;
            }

            qDebug() << "Download success. Preparing files...";

            // 2. 赋予执行权限 (必须步骤)
            ::system("chmod +x /root/DVS_download");

            // 3. 制作一个副本用于覆盖安装
            // DVS_download 留作备份，DVS_installer 用来去覆盖系统文件
            ret = ::system("cp /root/DVS_download /root/DVS_installer");

            if (ret != 0) {
                 qWarning() << "Failed to create installer copy.";
                 svrCmd = buildServerCommand(SCMD_OTA_UPDATE_ACK, 0);
                 svrCmdSocket->write(svrCmd);
                 break;
            }

            // 4. 执行“热更新”覆盖
            // 使用 mv 命令覆盖正在运行的 /usr/bin/DVS
            ret = ::system("mv -f /root/DVS_installer /usr/bin/DVS");

            if (ret == 0)
            {
                // 5. 将母本改名为 DVS 保留在 /root 下 (作为备份)
                ::system("mv -f /root/DVS_download /root/DVS");

                qDebug() << "Update successful. Backup saved to /root/DVS. Rebooting...";

                // 6. 发送成功 ACK (0x55, 参数 1)
                svrCmd = buildServerCommand(SCMD_OTA_UPDATE_ACK, 1);
                svrCmdSocket->write(svrCmd);
                svrCmdSocket->flush();

                // 给 ACK 一点发送时间
                svrCmdSocket->waitForBytesWritten(1000);

                // 7. 强制落盘并重启
                ::sync();
                ::system("reboot");
            }
            else
            {
                qWarning() << "Failed to replace /usr/bin/DVS";
                // 回复失败 ACK
                svrCmd = buildServerCommand(SCMD_OTA_UPDATE_ACK, 0);
                svrCmdSocket->write(svrCmd);
            }
            break;
        }
        case SCMD_SET_SD_RAW_RECORD_REQ:
            if (svrcmd.param == 0 || svrcmd.param == 1) {
                emit sdRawRecordSetRequested(svrcmd.param);
            } else {
                svrCmd = buildServerCommand(SCMD_SET_SD_RAW_RECORD_ACK, 0);
                if (svrCmdSocket->write(svrCmd) != svrCmd.size())
                    qWarning() << "Failed to send complete data";
            }
            break;

        case SCMD_QUERY_SD_RAW_RECORD_REQ:
            emit sdRawRecordStatusRequested();
            break;

        case SCMD_QUERY_SD_RAW_FILE_LIST_REQ:
            emit sdRawFileListRequested(svrcmd.param);
            break;

        default:
            // 不要 return；继续 while 处理后续帧
            break;
        }
        // ===== switch end =====
    }
}

bool ServerCommunicator::parseIPv4AndPortParam(const QByteArray &frame, QHostAddress &addr, quint16 &port) const
{
    if (frame.size() < 10)
        return false;

    const quint8 ip0 = static_cast<quint8>(frame.at(4));
    const quint8 ip1 = static_cast<quint8>(frame.at(5));
    const quint8 ip2 = static_cast<quint8>(frame.at(6));
    const quint8 ip3 = static_cast<quint8>(frame.at(7));
    port = (static_cast<quint16>(static_cast<quint8>(frame.at(8))) << 8) |
           static_cast<quint16>(static_cast<quint8>(frame.at(9)));

    const QString ip = QString::number(ip0) + "." +
                       QString::number(ip1) + "." +
                       QString::number(ip2) + "." +
                       QString::number(ip3);

    if (!NetworkConfigManager::isValidIpv4Endpoint(ip, port))
        return false;

    addr = QHostAddress(ip);
    return true;
}

void ServerCommunicator::sendTcpUpstreamAck(quint32 result)
{
    QByteArray svrCmd = buildServerCommand(SCMD_SET_TCP_UPSTREAM_ACK, result ? 1 : 0);
    if (svrCmdSocket->write(svrCmd) != svrCmd.size())
        qWarning() << "Failed to send complete data";
}

void ServerCommunicator::clearProbeSocket()
{
    if (!probeSocket)
        return;

    probeSocket->disconnect(this);
    probeSocket->abort();
    probeSocket->deleteLater();
    probeSocket = nullptr;
}

void ServerCommunicator::startTcpUpstreamProbe(const QHostAddress &newAddr, quint16 newPort)
{
    tcpProbeRunning = true;
    pendingSwitchAddress = newAddr;
    pendingSwitchPort = newPort;

    clearProbeSocket();
    probeSocket = new QTcpSocket(this);
    connect(probeSocket, &QTcpSocket::connected, this, &ServerCommunicator::onProbeConnected);
    connect(probeSocket, &QTcpSocket::errorOccurred, this, &ServerCommunicator::onProbeError);

    probeTimer->start();
    probeSocket->connectToHost(newAddr, newPort);
}

void ServerCommunicator::applyTcpUpstreamAfterProbeSuccess()
{
    m_networkConfig = NetworkConfigManager::load();

    const QString oldIp = svrAddress.toString();
    const quint16 oldPort = svrCmdPort;
    if (!NetworkConfigManager::isValidIpv4Endpoint(oldIp, oldPort))
    {
        m_networkConfig = NetworkConfigManager::defaultConfig();
    }

    m_networkConfig.tcpUpstream.configured.ip = pendingSwitchAddress.toString();
    m_networkConfig.tcpUpstream.configured.port = pendingSwitchPort;
    m_networkConfig.tcpUpstream.lastKnownGood.ip = oldIp;
    m_networkConfig.tcpUpstream.lastKnownGood.port = oldPort;
    m_networkConfig.tcpUpstream.state = NetworkConfigManager::pendingState();

    if (!NetworkConfigManager::save(m_networkConfig))
    {
        sendTcpUpstreamAck(0);
        return;
    }

    sendTcpUpstreamAck(1);

    tcpSwitchPending = true;
    tcpSwitchCommitTimer->start();
    startSwitchToAddress(pendingSwitchAddress, pendingSwitchPort);
}

void ServerCommunicator::commitTcpUpstreamSwitch()
{
    tcpSwitchPending = false;
    tcpSwitchCommitTimer->stop();

    m_networkConfig = NetworkConfigManager::load();
    m_networkConfig.tcpUpstream.configured.ip = svrAddress.toString();
    m_networkConfig.tcpUpstream.configured.port = svrCmdPort;
    m_networkConfig.tcpUpstream.lastKnownGood = m_networkConfig.tcpUpstream.configured;
    m_networkConfig.tcpUpstream.state = NetworkConfigManager::committedState();

    if (!NetworkConfigManager::save(m_networkConfig))
        qWarning() << "Failed to persist committed TCP upstream";
}

void ServerCommunicator::rollbackTcpUpstreamSwitch(const QString &reason)
{
    qWarning() << "Rollback tcp upstream due to" << reason;

    tcpSwitchPending = false;
    tcpSwitchCommitTimer->stop();

    m_networkConfig = NetworkConfigManager::load();
    QString fallbackIp = m_networkConfig.tcpUpstream.lastKnownGood.ip;
    quint16 fallbackPort = m_networkConfig.tcpUpstream.lastKnownGood.port;

    if (!NetworkConfigManager::isValidIpv4Endpoint(fallbackIp, fallbackPort))
    {
        fallbackIp = startupFallbackAddress.toString();
        fallbackPort = startupFallbackPort;
    }

    if (!NetworkConfigManager::isValidIpv4Endpoint(fallbackIp, fallbackPort))
    {
        NetworkConfig defaults = NetworkConfigManager::defaultConfig();
        fallbackIp = defaults.tcpUpstream.lastKnownGood.ip;
        fallbackPort = defaults.tcpUpstream.lastKnownGood.port;
    }

    m_networkConfig.tcpUpstream.configured.ip = fallbackIp;
    m_networkConfig.tcpUpstream.configured.port = fallbackPort;
    m_networkConfig.tcpUpstream.lastKnownGood.ip = fallbackIp;
    m_networkConfig.tcpUpstream.lastKnownGood.port = fallbackPort;
    m_networkConfig.tcpUpstream.state = NetworkConfigManager::committedState();
    if (!NetworkConfigManager::save(m_networkConfig))
        qWarning() << "Failed to persist rollback tcp upstream";

    startSwitchToAddress(QHostAddress(fallbackIp), fallbackPort);
}

void ServerCommunicator::startSwitchToAddress(const QHostAddress &addr, quint16 port)
{
    svrAddress = addr;
    svrCmdPort = port;

    if (svrCmdSocket->state() == QAbstractSocket::UnconnectedState)
    {
        connectToServer();
        return;
    }

    switchingByManager = true;
    svrCmdSocket->disconnectFromHost();

    if (svrCmdSocket->state() == QAbstractSocket::UnconnectedState)
    {
        switchingByManager = false;
        connectToServer();
    }
}

void ServerCommunicator::onProbeConnected()
{
    if (!tcpProbeRunning)
        return;

    probeTimer->stop();
    tcpProbeRunning = false;
    clearProbeSocket();

    applyTcpUpstreamAfterProbeSuccess();
}

void ServerCommunicator::onProbeError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    if (!tcpProbeRunning)
        return;

    probeTimer->stop();
    tcpProbeRunning = false;
    clearProbeSocket();
    sendTcpUpstreamAck(0);
}

void ServerCommunicator::onProbeTimeout()
{
    if (!tcpProbeRunning)
        return;

    tcpProbeRunning = false;
    clearProbeSocket();
    sendTcpUpstreamAck(0);
}

void ServerCommunicator::onTcpSwitchCommitTimeout()
{
    if (!tcpSwitchPending)
        return;

    rollbackTcpUpstreamSwitch(QStringLiteral("commit timeout"));
}

void ServerCommunicator::onStartupPendingTimeout()
{
    if (!startupPendingRecovery)
        return;

    startupPendingRecovery = false;
    rollbackTcpUpstreamSwitch(QStringLiteral("startup pending timeout"));
}

void ServerCommunicator::sendSvrCmd(QByteArray packet)
{
    if(svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug()<< packet.toHex();
    qDebug() << "Command echoed back to server";
}

void ServerCommunicator::sendSdRawRecordSetResult(quint32 result)
{
    QByteArray packet = buildServerCommand(SCMD_SET_SD_RAW_RECORD_ACK, result ? 1 : 0);
    if (svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug() << "SD raw record set result:" << result;
}

void ServerCommunicator::sendSdRawRecordStatus(quint32 status)
{
    QByteArray packet = buildServerCommand(SCMD_QUERY_SD_RAW_RECORD_ACK, status);
    if (svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug() << "SD raw record status:" << status;
}

void ServerCommunicator::sendSdRawFileListItem(quint32 sizeMb, const QString& fileName)
{
    QByteArray packet = buildSdFileListCommand(sizeMb, fileName);
    if (svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug() << "SD raw file item:" << sizeMb << fileName;
}

void ServerCommunicator::sendSdRawFileListEnd()
{
    QByteArray packet = buildSdFileListCommand(0xFFFFFFFF, QStringLiteral("END"));
    if (svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug() << "SD raw file list end";
}

void ServerCommunicator::sendUdpReportTargetSetResult(quint32 result)
{
    QByteArray packet = buildServerCommand(SCMD_SET_UDP_REPORT_TARGET_ACK, result ? 1 : 0);
    if (svrCmdSocket->write(packet) != packet.size())
        qWarning() << "Failed to send complete data";
    qDebug() << "UDP report target set result:" << result;
}

QByteArray ServerCommunicator::buildSdFileListCommand(quint32 sizeMb, const QString& fileName)
{
    QByteArray command(44, 0x00);
    command[0] = 0x5A;
    command[1] = 44;
    command[2] = SCMD_QUERY_SD_RAW_FILE_LIST_ACK;
    command[3] = 4;
    command[4] = static_cast<char>((sizeMb >> 24) & 0xFF);
    command[5] = static_cast<char>((sizeMb >> 16) & 0xFF);
    command[6] = static_cast<char>((sizeMb >> 8) & 0xFF);
    command[7] = static_cast<char>(sizeMb & 0xFF);

    const QByteArray nameBytes = fileName.toUtf8();
    const int copyLen = qMin(nameBytes.size(), command.size() - 8);
    if (copyLen > 0)
        memcpy(command.data() + 8, nameBytes.constData(), copyLen);
    return command;
}

void ServerCommunicator::onDisconnected()
{
    qDebug() << "Disconnected from server";
    emit svrDisconnected();

    // 断开后清空缓存，避免残留数据导致下次错帧
    m_rxBuf.clear();

    if (switchingByManager)
    {
        switchingByManager = false;
        if (svrCmdSocket->state() == QAbstractSocket::UnconnectedState)
            connectToServer();
        return;
    }

    if (tcpSwitchPending)
    {
        rollbackTcpUpstreamSwitch(QStringLiteral("disconnected before commit"));
        return;
    }

    resetReconnectTimer();
}

void ServerCommunicator::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    QString errorStr = svrCmdSocket->errorString();
    qWarning() << "Connection error:" << errorStr;
    qDebug() << "TCP socket state:" << svrCmdSocket->state();

    if (svrCmdSocket->state() != QAbstractSocket::ConnectedState)
    {
        emit svrDisconnected();

        // 出错也清空缓存，避免残留
        m_rxBuf.clear();

        if (switchingByManager)
            return;

        if (tcpSwitchPending)
        {
            rollbackTcpUpstreamSwitch(QStringLiteral("socket error before commit"));
            return;
        }

        resetReconnectTimer();
    }
}

void ServerCommunicator::sendHeartbeat()
{
    if (svrCmdSocket->state() == QAbstractSocket::ConnectedState)
    {
        // 构建心跳包：命令 0x50，参数 1 (或者0，看服务器要求)
        QByteArray heartbeatCmd = buildServerCommand(SCMD_HEARTBEAT_REQ, 1);

        svrCmdSocket->write(heartbeatCmd);

        //QByteArray heartbeatCmd_1 = buildServerCommand2(SCMD_PZT_LOCATION_DATA, 200,10);

        //svrCmdSocket->write(heartbeatCmd_1);

        // 为了避免日志刷屏，可以注释掉这句
        // qDebug() << "Sent heartbeat (0x50)";
    }
    else
    {
        heartbeatTimer->stop();
    }
}

void ServerCommunicator::attemptReconnect()
{
    if (reconnectAttempts >= maxReconnectAttempts)
        reconnectAttempts = 0;
    else
        reconnectAttempts++;

    qDebug() << "Reconnect attempt" << reconnectAttempts << "in 10s";

    tcpReconnectTimer->start(10000);
    connectToServer();
}

void ServerCommunicator::resetReconnectTimer()
{
    if (!tcpReconnectTimer->isActive())
    {
        reconnectAttempts = 0;
        tcpReconnectTimer->start(10000);
        connectToServer();
    }
}

/*定位发送至服务器*/
void ServerCommunicator::Send_local(uint32_t hz,uint32_t location)
{
    Q_UNUSED(location);


    if (svrCmdSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "socket not connected:" << svrCmdSocket->errorString();
        return;
    }
    
    QByteArray svrCmd = buildServerCommand2(SCMD_PZT_LOCATION_DATA, hz,location);

    qint64 n = svrCmdSocket->write(svrCmd);
    qDebug() << "write ret =" << n << "bytesToWrite =" << svrCmdSocket->bytesToWrite();

    if (n != svrCmd.size()) {
        qWarning() << "Failed to send complete data:" << svrCmdSocket->errorString();
    }


    qDebug() << "location to server";
}