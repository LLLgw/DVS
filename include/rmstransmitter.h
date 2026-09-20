#ifndef RMSTRANSMITTER_H
#define RMSTRANSMITTER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QVector>
#include <QVariant>
#include <QMutex>
#include <QElapsedTimer>

class RMSTransmitter : public QObject
{
    Q_OBJECT
public:
    explicit RMSTransmitter(QObject *parent = nullptr);
    ~RMSTransmitter();

public slots:
    void sendRMSData(const QVector<int16_t>& rmsData);
    void updateReportTarget(const QString &ip, quint16 port);

signals:

private:
    // 数据分帧和发送
    void splitAndSendData(const QVector<int16_t>& data);
    QByteArray createFrame(quint16 frameNumber, quint16 endFlag,
                           const QVector<int16_t>& data, int startIdx, int count);

    // 线程和Socket
    QUdpSocket* rmsSocket;

    // 配置参数
    QHostAddress serverAddress;
    quint16 serverPort;
    QString deviceId;

    // 统计信息
    QMutex m_statMutex;
    quint64 m_totalFramesSent;
    quint64 m_totalBytesSent;
    QElapsedTimer m_statTimer;

    // 协议常量
    static constexpr quint16 MAX_POINTS_PER_FRAME = 512;
    static constexpr quint16 TOTAL_FRAME_SIZE = 1080;
    static constexpr quint8 DATA_TYPE_RMS = 0x01;  // 唯一不同：数据类型标识
};

#endif // RMSTRANSMITTER_H
