#include "rmstransmitter.h"
#include "GlobalVars.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QtEndian>
#include <QThread>
#include <QNetworkInterface>
#include <QAbstractSocket>
RMSTransmitter::RMSTransmitter(QObject *parent)
    : QObject{parent}
{
    serverAddress = QHostAddress("8.138.101.239");
    serverPort = 41235;
    //    serverPort = 31235;
    // 原始 deviceId 前缀
    QString baseId = "SA1YA217DA0"; // 注意这里保留前 11 位，最后 2 位待替换

    QString macLastByte = "00";

    // 遍历网络接口，找到 eth0
    foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces())
    {
        if (iface.name() == "eth0")
        {
            QString mac = iface.hardwareAddress(); // 例如 "AA:BB:CC:DD:EE:F7"
            qDebug() << "eth0 mac:" << mac;

            if (!mac.isEmpty())
            {
                QStringList parts = mac.split(":");
                if (parts.size() == 6)
                {
                    macLastByte = parts.last().toUpper(); // "F7"
                }
            }
            break;
        }
    }

    // 拼成最终 deviceId
    deviceId = baseId + macLastByte;
    rmsSocket = new QUdpSocket(this);
    rmsSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, QVariant(512 * 1024));

    // qDebug() << "RMSTransmitter created (no thread management)";
}

RMSTransmitter::~RMSTransmitter()
{
    // stopTransmitting();

    if (rmsSocket)
    {
        rmsSocket->close();
        rmsSocket->deleteLater();
        rmsSocket = nullptr;
    }

    // qDebug() << "RMSTransmitter destroyed, total frames:" << m_totalFramesSent;
    qDebug() << "RMSTransmitter destroyed";
}

void RMSTransmitter::sendRMSData(const QVector<int16_t> &rmsData)
{
    // if (!m_isTransmitting || !m_udpSocket) {
    //     qWarning() << "RMSTransmitter not ready for sending";
    //     return;
    // }
    if (!GlobalVars::serverConnected)
        return;

    if (rmsData.isEmpty())
    {
        qWarning() << "RMSTransmitter: Empty RMS data";
        return;
    }

    if (serverAddress.isNull() || serverPort == 0)
    {
        qWarning() << "RMSTransmitter: Server address not configured";
        // emit errorOccurred("服务器地址未配置");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    // 发送数据
    splitAndSendData(rmsData);

    qDebug() << "RMSTransmitter: Sent" << rmsData.size() << "points in"
             << timer.elapsed() << "ms";

    // emit transmissionComplete();
}

void RMSTransmitter::updateReportTarget(const QString &ip, quint16 port)
{
    QHostAddress newAddress;
    if (!newAddress.setAddress(ip) || newAddress.protocol() != QAbstractSocket::IPv4Protocol || port == 0)
    {
        qWarning() << "RMSTransmitter: invalid UDP report target" << ip << port;
        return;
    }

    const quint32 ipValue = newAddress.toIPv4Address();
    if (ipValue == 0 || ipValue == 0xFFFFFFFFu)
    {
        qWarning() << "RMSTransmitter: invalid UDP report target" << ip << port;
        return;
    }

    serverAddress = newAddress;
    serverPort = port;
    qDebug() << "RMSTransmitter target updated to" << serverAddress.toString() << serverPort;
}

void RMSTransmitter::splitAndSendData(const QVector<int16_t> &data)
{
    const int totalPoints = data.size();
    int currentPos = 0;
    quint16 frameCount = 0;

    while (currentPos < totalPoints)
    {
        const int remainingPoints = totalPoints - currentPos;
        const int pointsToSend = qMin(remainingPoints, static_cast<int>(MAX_POINTS_PER_FRAME));

        // 创建数据帧
        QByteArray frame = createFrame(frameCount,
                                       (currentPos + pointsToSend >= totalPoints) ? 0x5A5A : 0x0000,
                                       data, currentPos, pointsToSend);

        // 发送数据帧
        qint64 bytesSent = rmsSocket->writeDatagram(frame, serverAddress, serverPort);
        if (bytesSent != frame.size())
        {
            QString errorMsg = QString("RMS data frame%1send fail: %2")
                                   .arg(frameCount)
                                   .arg(rmsSocket->errorString());
            qWarning() << errorMsg;
        }

        // qDebug() << "RMS帧" << frameCount << "发送成功，点数:" << pointsToSend;
        currentPos += pointsToSend;
        frameCount++;

        // 控制发送速率（RMS发送频率低，约25ms/帧）
        QThread::msleep(20);
    }
}

QByteArray RMSTransmitter::createFrame(quint16 frameNumber, quint16 endFlag,
                                       const QVector<int16_t> &data, int startIdx, int count)
{
    QByteArray frame(TOTAL_FRAME_SIZE, 0x00);
    int byteLen = GlobalVars::uploadDataByteLength;
    //  帧头标识
    frame[0] = 0xAA;

    // 设备ID
    const QByteArray devIdBytes = deviceId.toUtf8();
    const quint8 devIdLen = static_cast<quint8>(qMin(devIdBytes.size(), 40));
    frame[1] = devIdLen;
    memcpy(frame.data() + 2, devIdBytes.constData(), devIdLen);

    // 帧序号 (大端)
    qToBigEndian<quint16>(frameNumber, frame.data() + 42);

    // 结束标志 (大端)
    qToBigEndian<quint16>(endFlag, frame.data() + 44);

    // 数据类型标识 - RMS数据
    if (byteLen == 1)
    {
        // 单字节模式：原类型加上 0x10 (变成 0x11)
        frame[46] = static_cast<char>(DATA_TYPE_RMS + 0x10);
    }
    else
    {
        // 双字节模式：原样 (0x01)
        frame[46] = static_cast<char>(DATA_TYPE_RMS);
    }

    // 数据部分
    for (int i = 0; i < count; ++i)
    {
        quint16 bigEndianValue;
        qToBigEndian<quint16>(data[startIdx + i], &bigEndianValue);
        memcpy(frame.data() + 56 + i * 2, &bigEndianValue, 2);
    }

    return frame;
}
