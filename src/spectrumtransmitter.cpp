#include "spectrumtransmitter.h"
#include "GlobalVars.h"
#include <QtEndian>
#include <QDebug>
#include <QVariant>
// #include <vector>
#include <QElapsedTimer>
#include <QThread>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <algorithm>
SpectrumTransmitter::SpectrumTransmitter(QObject *parent)
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

    qDebug() << "Final deviceId:" << deviceId;

    specSocket = new QUdpSocket(this);
    specSocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, QVariant(8 * 1024 * 1024));

    qDebug() << "UDP transmitter started on port 41235";
}

SpectrumTransmitter::~SpectrumTransmitter()
{
    if (specSocket)
    {
        specSocket->close();
        specSocket->deleteLater();
        specSocket = nullptr;
    }

    qDebug() << "SpectrumTransmitter destroyed";
}

void SpectrumTransmitter::sendTransferedData(const QByteArray &packet)
{
    if (!GlobalVars::serverConnected)
        return;

    qint64 bytesSent = specSocket->writeDatagram(packet, serverAddress, serverPort);
    if (bytesSent == -1)
    {
        qWarning() << "Failed to send packet:" << specSocket->errorString();
    }
    else
    {
        qDebug() << "Forward packet";
    }
}

void SpectrumTransmitter::updateReportTarget(const QString &ip, quint16 port)
{
    QHostAddress newAddress;
    if (!newAddress.setAddress(ip) || newAddress.protocol() != QAbstractSocket::IPv4Protocol || port == 0)
    {
        qWarning() << "SpectrumTransmitter: invalid UDP report target" << ip << port;
        return;
    }

    const quint32 ipValue = newAddress.toIPv4Address();
    if (ipValue == 0 || ipValue == 0xFFFFFFFFu)
    {
        qWarning() << "SpectrumTransmitter: invalid UDP report target" << ip << port;
        return;
    }

    serverAddress = newAddress;
    serverPort = port;
    qDebug() << "SpectrumTransmitter target updated to" << serverAddress.toString() << serverPort;
}

/*频谱发送*/
void SpectrumTransmitter::sendSpectrumData(const QVector<int16_t> &spectrum)
{
    static const int kUploadPointsTarget = 20000;

    static QElapsedTimer lastcalltimer;
    static bool firstCall = true;

    qint64 interval = 0;

    if (firstCall)
    {
        lastcalltimer.start();
        firstCall = false;
    }
    else
    {
        interval = lastcalltimer.restart();
    }

    if (!GlobalVars::serverConnected)
        return;

    if (spectrum.isEmpty())
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

    // 采样点小于上传目标时，尾部补0，保证上报长度按20000点处理
    QVector<int16_t> uploadSpectrum = spectrum;
    if (uploadSpectrum.size() < kUploadPointsTarget)
    {
        uploadSpectrum.resize(kUploadPointsTarget);
        std::fill(uploadSpectrum.begin() + spectrum.size(), uploadSpectrum.end(), 0);
    }

    // 【新增】根据分辨率进行数据合并 (Max Pooling)
    // 获取当前模式参数
    int res = GlobalVars::spatialResolution;
    int truncDist = GlobalVars::truncateDistance;

    // 1. 防御性编程：计算实际的有效截断长度（防止超过数组真实大小）
    int effectiveSize = qMin(uploadSpectrum.size(), truncDist);

    QVector<int16_t> dataToSend;

    if (res <= 1)
    {
        // 分辨率为 1，直接利用 mid() 函数截取从 0 开始的 effectiveSize 个点
        dataToSend = uploadSpectrum.mid(0, effectiveSize);
    }
    else
    {
        // 降采样模式（因为互斥逻辑，此时 effectiveSize 一定是 20000）
        int newSize = (effectiveSize + res - 1) / res;
        dataToSend.reserve(newSize);

        for (int i = 0; i < newSize; ++i)
        {
            int startIdx = i * res;
            int currentChunkSize = qMin(res, effectiveSize - startIdx);

            int16_t maxVal = uploadSpectrum[startIdx];
            for (int j = 1; j < currentChunkSize; ++j)
            {
                if (uploadSpectrum[startIdx + j] > maxVal)
                {
                    maxVal = uploadSpectrum[startIdx + j];
                }
            }
            dataToSend.append(maxVal);
        }
    }

    // 将处理后的新数组（不管是截断的还是降采样的）交给 UDP 发送
    splitAndSendData(dataToSend);

    qDebug() << "SpectrumTransmitter:"
             << "Interval:" << interval << "ms"
             << "| Send:" << timer.elapsed() << "ms"
             << "| Points:" << dataToSend.size();
}

/*发送数据到服务器*/
void SpectrumTransmitter::splitAndSendData(const QVector<int16_t> &data)
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

        //serverAddress = QHostAddress("192.168.3.186"); 
        // 发送数据帧
        qint64 bytesSent = specSocket->writeDatagram(frame, serverAddress, serverPort); 
        if (bytesSent != frame.size())
        {
            QString errorMsg = QString("Spectrum data frame%1send fail: %2")
                                   .arg(frameCount)
                                   .arg(specSocket->errorString());
            qWarning() << errorMsg;
        }

        // qDebug() << "Spectrum帧" << frameCount << "发送成功，点数:" << pointsToSend;
        currentPos += pointsToSend;
        frameCount++;

        // 控制发送速率（Spectrum发送频率高，约5ms/帧）
        QThread::msleep(1);
    }
}

/*创建数据帧*/
QByteArray SpectrumTransmitter::createFrame(quint16 frameNumber, quint16 endFlag,
                                            const QVector<int16_t> &data, int startIdx, int count)
{
    // 【修改点 1】：根据全局模式，决定这一帧的固定总长度
    int byteLen = GlobalVars::uploadDataByteLength;
    int fixedFrameSize = (byteLen == 1) ? 568 : 1080;

    // 【修改点 2】：分配固定长度的内存，并全部初始化为 0x00
    // 这样，如果是尾包 (count < 512)，后面没写数据的部分就自动全是 0x00 填充了
    QByteArray frame(fixedFrameSize, 0x00);

    // 帧头标识
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

    // 数据类型标识 - 频谱数据
    frame[46] = DATA_TYPE_SPECTRUM;

    if (byteLen == 1)
    {
        // 单字节模式：原类型加上 0x10 (变成 0x10)
        frame[46] = static_cast<char>(DATA_TYPE_SPECTRUM + 0x10);
    }
    else
    {
        // 双字节模式：原样 (0x00)
        frame[46] = static_cast<char>(DATA_TYPE_SPECTRUM);
    }

    // 【修改点 3】：数据部分（只循环 count 次，写入有效数据）
    if (byteLen == 1)
    {
        // 1字节传输模式：将 [0, 4000] 线性映射到 [0, 255]
        for (int i = 0; i < count; ++i)
        {
            int rawValue = data[startIdx + i];

            // 1. 钳制 (Clamping)：小于0变为0，大于4000变为4000
            if (rawValue > 4000)
            {
                rawValue = 4000;
            }
            else if (rawValue < 0)
            {
                rawValue = 0;
            }

            // 2. 归一化映射：(rawValue - 0) * 255 / (4000 - 0)
            // 最大值计算 4000 * 255 = 1020000，完全在 32 位 int 安全范围内
            int mappedValue = (rawValue*255) / 4000;

            // 3. 强转为 无符号的 1 字节 (quint8) 并填入报文
            frame[56 + i] = static_cast<quint8>(mappedValue);
        }
    }
    else
    {
        // 默认的 2字节大端传输模式
        for (int i = 0; i < count; ++i)
        {
            quint16 bigEndianValue;
            qToBigEndian<quint16>(data[startIdx + i], &bigEndianValue);
            memcpy(frame.data() + 56 + i * 2, &bigEndianValue, 2);
        }
    }

    return frame;
}
