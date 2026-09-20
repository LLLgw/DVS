#ifndef FPGACOMMUNICATOR_H
#define FPGACOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QElapsedTimer>

class FpgaCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit FpgaCommunicator(QObject *parent = nullptr);
    void connectToFpga();

signals:
    void handshakeSuccess();
    void fpgaInitSuccess();
    void transferCommandToServer(QByteArray);

//private slots:
public slots:
    void sendFpgaCmd(QByteArray);
    void readFpgaCmd();

private:
    void startFpgaHandshake();
    void sendHandshakeFrame();
    void onHandshakeTimeout();
    void initFpga();

    QUdpSocket *fpgaSendCmdSocket;
    QUdpSocket *fpgaReceiveCmdSocket;
    QTimer *fpgaReconnectTimer;
    QElapsedTimer connectionTimer;
    QHostAddress fpgaAddress;
    quint16 fpgaSendCmdPort;
    quint16 fpgaReceiveCmdPort;
    QByteArray handshakeSendFrame;
    QByteArray handshakeReceiveFrame;

    bool handshakeFlag = true;
    bool fpgaInitFlag = true;
    bool responseReceived = false;

    bool pulseWidthConfigOK = false;
    bool pulseFrequencyConfigOK = false;
    bool samplePointsConfigOK = false;
    bool sampleDelayPointsConfigOK = false;
    bool sampleFrequencyConfigOK = false;
    bool averageNumberConfigOK = false;
    bool biasVoltageConfigOK = false;
    bool differenceConfigOK = false;
};

#endif // FPGACOMMUNITOR_H
