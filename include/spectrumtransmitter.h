#ifndef SPECTRUMTRANSMITTER_H
#define SPECTRUMTRANSMITTER_H

#include <QObject>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include "buffer_state.h"

class SpectrumTransmitter : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumTransmitter(std::shared_ptr<SharedBuffer> shared,QObject *parent = nullptr);
    ~SpectrumTransmitter();

public slots:
    void sendSpectrumData(const QVector<int16_t>& spectrum);
    void sendTransferedData(const QByteArray& packet);
    void updateReportTarget(const QString &ip, quint16 port);

signals:

private:
    // 数据分帧和发送
    void splitAndSendData(const QVector<int16_t>& data);
    QByteArray createFrame(quint16 frameNumber, quint16 endFlag,
                           const QVector<int16_t>& data, int startIdx, int count);

    QUdpSocket *specSocket;
    QHostAddress serverAddress;
    quint16 serverPort;
    QString deviceId;

    // 协议常量
    static constexpr quint16 MAX_POINTS_PER_FRAME = 512;
    static constexpr quint16 TOTAL_FRAME_SIZE = 1080;
    static constexpr quint8 DATA_TYPE_SPECTRUM = 0x00;

    std::shared_ptr<SharedBuffer> m_shared;     //buffer状态指针
};

#endif // SPECTRUMTRANSMITTER_H
