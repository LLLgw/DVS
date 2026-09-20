#include "controller.h"
#include "message.h"
#include "GlobalVars.h"
#include <QTimer>
#include <QHostAddress>
#include <QAbstractSocket>

Controller::Controller(QObject *parent)
    : QObject{parent}
    , m_networkConfig(NetworkConfigManager::load())
{
    threadStart = true;

    /*初始化接收FPGA buffer状态*/
    m_sharedBuffer = std::make_shared<SharedBuffer>();
    m_sharedBuffer->state[0]= bufferState::Free;
    m_sharedBuffer->state[1]= bufferState::Free;
    m_sharedBuffer->state[2]= bufferState::Free;
    //m_sharedBuffer->state[3]= bufferState::Free;
    
    /* 初始化GPIO控制器 */
    gpioController = new SystemGPIOController();        
    gpioController->initialize();
    
    /*设置LED不同频率的闪烁*/
    gpioController->setGreenLedBlink(BLINK_FAST);
    gpioController->setYellowLedBlink(BLINK_FAST);

    acqWorker = new DataAcquisition(m_sharedBuffer);                /*初始化数据采集状态*/
    specProcessorWorker = new SpectrumProcessor(m_sharedBuffer);    /*初始化频谱处理*/
    specTransmitterWorker = new SpectrumTransmitter();              /*初始化频谱发送*/
    rmsProcessorWorker = new RMSProcessor();                        /*初始化RMS处理*/
    rmsTransmitterWorker = new RMSTransmitter();                    /*初始化RMS发送*/

    svrComm = new ServerCommunicator();                             /*配置服务器通信器*/
    fpgaComm = new FpgaCommunicator();                              /*配置FPGA通信器*/

    // 将Worker移动到各自线程
    acqWorker->moveToThread(&m_acqThread);                          /*数据采集线程*/
    specProcessorWorker->moveToThread(&m_specProcessorThread);      /*数据采集线程*/
    specTransmitterWorker->moveToThread(&m_specTransmitterThread);  /*频谱发送线程*/
    rmsProcessorWorker->moveToThread(&m_rmsProcessorThread);        /*RMS处理线程*/
    rmsTransmitterWorker->moveToThread(&m_rmsTransmitterThread);    /*RMS发送线程*/

    // 连接信号槽
    /*完成后删除*/
    connect(&m_acqThread, &QThread::finished, acqWorker, &QObject::deleteLater);
    connect(&m_specProcessorThread, &QThread::finished, specProcessorWorker, &QObject::deleteLater);
    connect(&m_specTransmitterThread, &QThread::finished, specTransmitterWorker, &QObject::deleteLater);
    connect(&m_rmsProcessorThread, &QThread::finished, rmsProcessorWorker, &QObject::deleteLater);
    connect(&m_rmsTransmitterThread, &QThread::finished, rmsTransmitterWorker, &QObject::deleteLater);

    /*数据接收udp接收到原始数据*/
    connect(acqWorker->dataSocket, &QUdpSocket::readyRead, acqWorker, &DataAcquisition::processDatagram);
    /*原始数据采集完成之后进行频谱处理*/
    connect(acqWorker, &DataAcquisition::specRawDataReady, specProcessorWorker, &SpectrumProcessor::processSpectrumData, Qt::QueuedConnection);
    //connect(acqWorker, &DataAcquisition::displayRawDataReady, this, &Controller::rawDataReady, Qt::QueuedConnection);
    //connect(specProcessorWorker, &SpectrumProcessor::specDataProcessed, this, &Controller::specDataReady, Qt::QueuedConnection);
    //发送定位数据
    connect(specProcessorWorker, &SpectrumProcessor::Fkp_local, svrComm, &ServerCommunicator::Send_local, Qt::QueuedConnection);
    /*频谱数据处理完成之后进行发送*/
    connect(specProcessorWorker, &SpectrumProcessor::specDataProcessed, specTransmitterWorker, &SpectrumTransmitter::sendSpectrumData, Qt::QueuedConnection);
    /*忙*/
    connect(specProcessorWorker, &SpectrumProcessor::processorBusy, acqWorker, &DataAcquisition::onProcessorBusy, Qt::QueuedConnection);

    connect(acqWorker, &DataAcquisition::rmsRawDataReady, rmsProcessorWorker, &RMSProcessor::processRMSData, Qt::QueuedConnection);
    connect(rmsProcessorWorker, &RMSProcessor::rmsDataProcessed, rmsTransmitterWorker, &RMSTransmitter::sendRMSData, Qt::QueuedConnection);
    //connect(rmsProcessorWorker, &RMSProcessor::rmsDataProcessed, this, &Controller::rmsDataReady, Qt::QueuedConnection);
    /*握手成功*/
    connect(fpgaComm, &FpgaCommunicator::handshakeSuccess, this, &Controller::onFpgaConnected);
    /*FPGA初始化成功*/
    connect(fpgaComm, &FpgaCommunicator::fpgaInitSuccess, this, &Controller::onFpgaInited);
    /*传输命令到服务器*/
    connect(fpgaComm, &FpgaCommunicator::transferCommandToServer, svrComm, &ServerCommunicator::sendSvrCmd);
    /*向FPGA发送命令*/
    connect(this, &Controller::forwardFpgaCmd, fpgaComm, &FpgaCommunicator::sendFpgaCmd);
    /*服务器已连接*/
    connect(svrComm, &ServerCommunicator::svrConnected, this, &Controller::onServerConnected);
    /*与服务器断开连接*/
    connect(svrComm, &ServerCommunicator::svrDisconnected, this, &Controller::onServerDisconnected);
    
    connect(svrComm, &ServerCommunicator::appLogIn,this, &Controller::onAppLogIn);
    connect(svrComm, &ServerCommunicator::appLogOut, this, &Controller::onAppLogOut);
    /*向FPGA发送命令*/
    connect(svrComm, &ServerCommunicator::transferCommandToFpga, fpgaComm, &FpgaCommunicator::sendFpgaCmd);
    /*开始测试*/
    connect(svrComm, &ServerCommunicator::svrStartTest, this, &Controller::startThread);
    /*停止测试*/
    connect(svrComm, &ServerCommunicator::svrstopTest, this, &Controller::stopThread);
    /*sd卡设置*/
    connect(svrComm, &ServerCommunicator::sdRawRecordSetRequested, acqWorker, &DataAcquisition::setSdRawRecording, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::sdRawRecordStatusRequested, acqWorker, &DataAcquisition::querySdRawRecordStatus, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::sdRawFileListRequested, acqWorker, &DataAcquisition::querySdRawFileList, Qt::QueuedConnection);

    connect(acqWorker, &DataAcquisition::sdRawRecordSetResult, svrComm, &ServerCommunicator::sendSdRawRecordSetResult, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawRecordStatus, svrComm, &ServerCommunicator::sendSdRawRecordStatus, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawFileListItem, svrComm, &ServerCommunicator::sendSdRawFileListItem, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawFileListEnd, svrComm, &ServerCommunicator::sendSdRawFileListEnd, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::udpReportTargetSetRequested, this, &Controller::onUdpReportTargetSetRequested);

    // 启动线程
    m_acqThread.start();
    m_specProcessorThread.start();
    m_specTransmitterThread.start();
    m_rmsProcessorThread.start();      // 新增
    m_rmsTransmitterThread.start();    // 新增
   // m_serverThread.start(); //改
    /*设置线程优先级*/
    m_acqThread.setPriority(QThread::TimeCriticalPriority);
    m_specProcessorThread.setPriority(QThread::NormalPriority);
    m_specTransmitterThread.setPriority(QThread::LowPriority);
    m_rmsProcessorThread.setPriority(QThread::LowPriority);
    m_rmsTransmitterThread.setPriority(QThread::LowPriority);

    queueUdpReportTargetToTransmitters(m_networkConfig.udpReportTarget.ip,
                                       m_networkConfig.udpReportTarget.port);
    //qDebug() << "accWorker thread:" << acqWorker->thread();
    //qDebug() << "processorWorker thread:" << processorWorker->thread();

    fpgaComm->connectToFpga();  /*连接FPGA*/
    svrComm->connectToServer(); /*连接服务器*/
}

void Controller::startThread()
{
    if (threadStart)
    {
        // 线程已在运行时，仅启动采集状态
        QMetaObject::invokeMethod(acqWorker, "startAcquisition", Qt::QueuedConnection);
        return;
    }

    acqWorker = new DataAcquisition(m_sharedBuffer);
    specProcessorWorker = new SpectrumProcessor(m_sharedBuffer);
    specTransmitterWorker = new SpectrumTransmitter();
    rmsProcessorWorker = new RMSProcessor();
    rmsTransmitterWorker = new RMSTransmitter();

    // 将Worker移动到各自线程
    acqWorker->moveToThread(&m_acqThread);
    specProcessorWorker->moveToThread(&m_specProcessorThread);
    specTransmitterWorker->moveToThread(&m_specTransmitterThread);
    rmsProcessorWorker->moveToThread(&m_rmsProcessorThread);
    rmsTransmitterWorker->moveToThread(&m_rmsTransmitterThread);

    // 连接信号槽
    connect(&m_acqThread, &QThread::finished, acqWorker, &QObject::deleteLater);
    connect(&m_specProcessorThread, &QThread::finished, specProcessorWorker, &QObject::deleteLater);
    connect(&m_specTransmitterThread, &QThread::finished, specTransmitterWorker, &QObject::deleteLater);
    connect(&m_rmsProcessorThread, &QThread::finished, rmsProcessorWorker, &QObject::deleteLater);
    connect(&m_rmsTransmitterThread, &QThread::finished, rmsTransmitterWorker, &QObject::deleteLater);
    connect(acqWorker->dataSocket, &QUdpSocket::readyRead, acqWorker, &DataAcquisition::processDatagram);
    connect(acqWorker, &DataAcquisition::specRawDataReady, specProcessorWorker, &SpectrumProcessor::processSpectrumData, Qt::QueuedConnection);
    //connect(acqWorker, &DataAcquisition::displayRawDataReady, this, &Controller::rawDataReady, Qt::QueuedConnection);
    //connect(specProcessorWorker, &SpectrumProcessor::specDataProcessed, this, &Controller::specDataReady, Qt::QueuedConnection);
    connect(specProcessorWorker, &SpectrumProcessor::specDataProcessed, specTransmitterWorker, &SpectrumTransmitter::sendSpectrumData, Qt::QueuedConnection);

    // RMS数据流连接
    connect(acqWorker, &DataAcquisition::rmsRawDataReady, rmsProcessorWorker, &RMSProcessor::processRMSData, Qt::QueuedConnection);
    connect(rmsProcessorWorker, &RMSProcessor::rmsDataProcessed, rmsTransmitterWorker, &RMSTransmitter::sendRMSData, Qt::QueuedConnection);
    //connect(rmsProcessorWorker, &RMSProcessor::rmsDataProcessed, this, &Controller::rmsDataReady, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::sdRawRecordSetRequested, acqWorker, &DataAcquisition::setSdRawRecording, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::sdRawRecordStatusRequested, acqWorker, &DataAcquisition::querySdRawRecordStatus, Qt::QueuedConnection);
    connect(svrComm, &ServerCommunicator::sdRawFileListRequested, acqWorker, &DataAcquisition::querySdRawFileList, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawRecordSetResult, svrComm, &ServerCommunicator::sendSdRawRecordSetResult, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawRecordStatus, svrComm, &ServerCommunicator::sendSdRawRecordStatus, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawFileListItem, svrComm, &ServerCommunicator::sendSdRawFileListItem, Qt::QueuedConnection);
    connect(acqWorker, &DataAcquisition::sdRawFileListEnd, svrComm, &ServerCommunicator::sendSdRawFileListEnd, Qt::QueuedConnection);

    QMetaObject::invokeMethod(acqWorker, "startAcquisition", Qt::QueuedConnection);

    m_acqThread.start();
    m_specProcessorThread.start();
    m_specTransmitterThread.start();
    m_rmsProcessorThread.start();
    m_rmsTransmitterThread.start();
    m_acqThread.setPriority(QThread::TimeCriticalPriority);
    m_specProcessorThread.setPriority(QThread::NormalPriority);
    m_specTransmitterThread.setPriority(QThread::LowPriority);
    m_rmsProcessorThread.setPriority(QThread::LowPriority);
    m_rmsTransmitterThread.setPriority(QThread::LowPriority);

    queueUdpReportTargetToTransmitters(m_networkConfig.udpReportTarget.ip,
                                       m_networkConfig.udpReportTarget.port);

    threadStart = true;
}

void Controller::stopThread()
{
    if (!threadStart)
        return;

    QMetaObject::invokeMethod(acqWorker, "stopAcquisition", Qt::BlockingQueuedConnection);

    m_acqThread.quit();
    m_specProcessorThread.quit();
    m_specTransmitterThread.quit();
    m_rmsProcessorThread.quit();      // 新增
    m_rmsTransmitterThread.quit();    // 新增

    //m_processorThread.requestInterruption();
    bool acqStopped = m_acqThread.wait(2000);
    bool specProcStopped = m_specProcessorThread.wait(2000);
    bool specTransStopped = m_specTransmitterThread.wait(2000);
    bool rmsProcStopped = m_rmsProcessorThread.wait(2000);    // 新增
    bool rmsTransStopped = m_rmsTransmitterThread.wait(2000); // 新增

    qDebug() << "acqThread is finished:" << m_acqThread.isFinished();
    qDebug() << "specProcessorThread is finished:" << m_specProcessorThread.isFinished();
    qDebug() << "specTransmitterThread is finished:" << m_specTransmitterThread.isFinished();
    qDebug() << "rmsProcessorThread is finished:" << m_rmsProcessorThread.isFinished();
    qDebug() << "rmsTransmitterThread is finished:" << m_rmsTransmitterThread.isFinished();
    if (!acqStopped || !specProcStopped || !specTransStopped || !rmsProcStopped || !rmsTransStopped)
    {
        qCritical() << "Threads not stopped properly!";
    }
    threadStart = false;
}

void Controller::onSendFpgaCmd(QByteArray packet)
{
    //qDebug()<<packet.toHex();
    emit forwardFpgaCmd(packet);
    //statusLabel->setText("等待FPGA回应...");
    //QTimer::singleShot(3000, this, [this](){
    //    if (!responseReceived) {
    //        statusLabel->setText("FPGA响应超时");
    //    }
    //});
}

void Controller::onFpgaConnected()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::fpgaConnected = true;
}

void Controller::onFpgaInited()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::fpgaInited = true;
    gpioController->setGreenLed(true);
}

void Controller::onServerConnected()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::serverConnected = true;
    gpioController->setYellowLed(false);
    //gpioController->setDvsPower(true);
}

void Controller::onServerDisconnected()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::serverConnected = false;
    gpioController->setYellowLedBlink(BLINK_FAST);
}

void Controller::onAppLogIn()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::appLogIn = true;
    gpioController->setDvsPower(true);
}

void Controller::onAppLogOut()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    GlobalVars::appLogIn = false;
    gpioController->setDvsPower(false);
}

bool Controller::queueUdpReportTargetToTransmitters(const QString &ip, quint16 port)
{
    bool specPosted = false;
    bool rmsPosted = false;

    if (specTransmitterWorker)
    {
        specPosted = QMetaObject::invokeMethod(specTransmitterWorker,
                                               "updateReportTarget",
                                               Qt::QueuedConnection,
                                               Q_ARG(QString, ip),
                                               Q_ARG(quint16, port));
    }

    if (rmsTransmitterWorker)
    {
        rmsPosted = QMetaObject::invokeMethod(rmsTransmitterWorker,
                                              "updateReportTarget",
                                              Qt::QueuedConnection,
                                              Q_ARG(QString, ip),
                                              Q_ARG(quint16, port));
    }

    return specPosted && rmsPosted;
}

void Controller::onUdpReportTargetSetRequested(const QString &ip, quint16 port)
{
    QHostAddress addr;
    const bool validIp = addr.setAddress(ip) && addr.protocol() == QAbstractSocket::IPv4Protocol;
    const quint32 ipValue = validIp ? addr.toIPv4Address() : 0;
    if (!validIp || ipValue == 0 || ipValue == 0xFFFFFFFFu || port == 0)
    {
        svrComm->sendUdpReportTargetSetResult(0);
        return;
    }

    NetworkConfig latestConfig = NetworkConfigManager::load();
    latestConfig.udpReportTarget.ip = ip;
    latestConfig.udpReportTarget.port = port;

    if (!NetworkConfigManager::save(latestConfig))
    {
        svrComm->sendUdpReportTargetSetResult(0);
        return;
    }

    m_networkConfig = latestConfig;

    bool posted = true;
    if (threadStart)
        posted = queueUdpReportTargetToTransmitters(ip, port);

    svrComm->sendUdpReportTargetSetResult(posted ? 1 : 0);
}

Controller::~Controller()
{
    m_acqThread.quit();
    m_specProcessorThread.quit();
    m_specTransmitterThread.quit();
    m_rmsProcessorThread.quit();      // 新增
    m_rmsTransmitterThread.quit();    // 新增

    m_acqThread.wait();
    m_specProcessorThread.wait();
    m_specTransmitterThread.wait();
    m_rmsProcessorThread.wait();      // 新增
    m_rmsTransmitterThread.wait();    // 新增

    acqWorker = nullptr;
    specProcessorWorker= nullptr;
    specTransmitterWorker = nullptr;
    rmsProcessorWorker = nullptr;     // 新增
    rmsTransmitterWorker = nullptr;   // 新增
    qDebug() << "Controller destroyed";
}


