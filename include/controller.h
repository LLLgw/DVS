#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>
#include "systemgpiocontroller.h"
#include "dataacquisition.h"
#include "spectrumprocessor.h"
#include "spectrumtransmitter.h"
#include "rmsprocessor.h"
#include "rmstransmitter.h"
#include "servercommunicator.h"
#include "fpgacommunicator.h"
#include "networkconfigmanager.h"
#include "buffer_state.h"
#include <cstdint>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

private:
    bool threadStart;
    bool responseReceived = false;
    SystemGPIOController *gpioController;
    ServerCommunicator *svrComm;
    FpgaCommunicator *fpgaComm;

    QThread m_acqThread;
    QThread m_specProcessorThread;
    QThread m_specTransmitterThread;
    QThread m_rmsProcessorThread;
    QThread m_rmsTransmitterThread;
    DataAcquisition* acqWorker;
    SpectrumProcessor* specProcessorWorker;
    SpectrumTransmitter* specTransmitterWorker;
    RMSProcessor* rmsProcessorWorker;
    RMSTransmitter* rmsTransmitterWorker;
    NetworkConfig m_networkConfig;

    std::shared_ptr<SharedBuffer> m_sharedBuffer;

    void setupFpgaCmdSocket();
    bool queueUdpReportTargetToTransmitters(const QString &ip, quint16 port);

signals:
    void specDataReady(const QVector<int16_t>& specData);
    void rmsDataReady(const QVector<int16_t>& rmsData);
    void rawDataReady(const QVector<int16_t>& rawData);
    void forwardFpgaCmd(QByteArray);

//private slots:
public slots:
    void startThread();
    void stopThread();
    void onSendFpgaCmd(QByteArray);
    void onFpgaConnected();
    void onFpgaInited();
    void onServerConnected();
    void onServerDisconnected();
    void onAppLogIn();
    void onAppLogOut();
    void onUdpReportTargetSetRequested(const QString &ip, quint16 port);
};

#endif // CONTROLLER_H
