#include "fpgacommunicator.h"
#include "message.h"
#include "GlobalVars.h"
#include <QEventLoop>
#include <QHostAddress>
#include <QDebug>

FpgaCommunicator::FpgaCommunicator(QObject *parent)
    : QObject{parent}
{
    fpgaAddress = QHostAddress("192.168.137.2");
    fpgaSendCmdPort = 6789;
    fpgaReceiveCmdPort = 6787;
    //handshakeSendFrame = QByteArray::fromHex("A5 5A AA 55 55 AA 00 01 00 25 00 00 00 08 00 00 00 00 00 00 00 00 00 00");
    //handshakeReceiveFrame = QByteArray::fromHex("5A A5 55 AA AA 55 00 02 00 01 00 04 00 25 00 00");
    
    /*开始采样报文*/
    handshakeSendFrame = QByteArray::fromHex("A5 5A AA 55 55 AA 00 01 00 01 00 00 00 08 00 00 00 00 00 00 00 00 00 00");
    /*接收报文*/
    handshakeReceiveFrame = QByteArray::fromHex("5A A5 55 AA AA 55 00 02 00 01 00 04 00 01 00 00");

    fpgaSendCmdSocket = new QUdpSocket(this);
    fpgaReceiveCmdSocket = new QUdpSocket(this);
    fpgaReconnectTimer = new QTimer(this);
    //fpgaReconnectTimer->setSingleShot(true);

    connect(fpgaReceiveCmdSocket, &QUdpSocket::readyRead, this, &FpgaCommunicator::readFpgaCmd);
    connect(fpgaReconnectTimer, &QTimer::timeout, this, &FpgaCommunicator::onHandshakeTimeout);
}

/*连接FPGA*/
void FpgaCommunicator::connectToFpga()
{
    /**/
    if (!fpgaSendCmdSocket->bind(QHostAddress::Any, fpgaSendCmdPort))
    {
        qWarning() << "Failed to bind FPGA sendUDP socket";
        return;
    }
    qDebug() << "UDP receiver started on port 6789";
    if (!fpgaReceiveCmdSocket->bind(QHostAddress::Any, fpgaReceiveCmdPort))
    {
        qWarning() << "Failed to bind FPGA receiveUDP socket";
        return;
    }
    qDebug() << "UDP receiver started on port 6787";
    startFpgaHandshake();
}

/*to FPGA发送*/
void FpgaCommunicator::sendFpgaCmd(QByteArray packet)
{
    //qDebug()<<packet.toHex();
    fpgaSendCmdSocket->writeDatagram(packet, fpgaAddress, fpgaSendCmdPort);

    //statusLabel->setText("等待FPGA回应...");
    //QTimer::singleShot(3000, this, [this](){
    //    if (!responseReceived) {
    //        statusLabel->setText("FPGA响应超时");
    //    }
    //});
}

void FpgaCommunicator::readFpgaCmd()
{
    while (fpgaReceiveCmdSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(fpgaReceiveCmdSocket->pendingDatagramSize());
        QHostAddress senderAddress;
        quint16 senderPort;

        fpgaReceiveCmdSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);
        qDebug()<< "receive from fpga:" << datagram.toHex();
        ftopcmdstruct_t fpgacmd;
        QByteArray svrCmd(44, 0x00);

        if (handshakeFlag)
        {
            if (senderAddress.protocol() == QAbstractSocket::IPv6Protocol)
                senderAddress = QHostAddress(senderAddress.toIPv4Address());  // 转换为IPv4

            if (senderAddress == fpgaAddress && datagram == handshakeReceiveFrame)
            {
                qDebug() << "Handshake successful with FPGA!";
                handshakeFlag = false;
                fpgaReconnectTimer->stop();  // 停止重试
                emit handshakeSuccess();  // 发出成功信号
                initFpga();
            }
        }
        else if (fpgaInitFlag)
        {
            responseReceived = ::parseFpgaCommand(datagram, fpgacmd);
            switch (fpgacmd.cmdCode)
            {
            case CMD_CONFIG_PULSE_WIDTH:
                if (fpgacmd.result == 100)
                    pulseWidthConfigOK = true;
                break;

            case CMD_CONFIG_PULSE_FREQUENCY:
                if (fpgacmd.result == GlobalVars::signalSampleFrequency)
                //if (fpgacmd.result == 5000)
                    pulseFrequencyConfigOK = true;
                break;

            case CMD_CONFIG_AVERAGE_NUMBER:
                if (fpgacmd.result == GlobalVars::signalAverageNumber)
                    averageNumberConfigOK = true;
                break;

            case CMD_CONFIG_SAMPLEPOINTS:
                if (fpgacmd.result == GlobalVars::lineSamplePoints)
                    samplePointsConfigOK = true;
                break;

            case CMD_CONFIG_SAMPLE_DELAYPOINTS:
                if (fpgacmd.result == 33)
                //if (fpgacmd.result == 25)
                    sampleDelayPointsConfigOK = true;
                break;

            case CMD_CONFIG_SAMPLE_FREQUENCY:
                if (fpgacmd.result == 5)
                //if (fpgacmd.result == 4)
                    sampleFrequencyConfigOK = true;
                break;

            case CMD_CONFIG_ADC_BIAS_VOLTAGE:
                if (static_cast<uint16_t>(fpgacmd.result) == 0x0C18)    //设定ADC偏置电压为-1000mV，FPGA返回命令的参数错误，应该返回0xFC18，但实际返回0x0C18
                    biasVoltageConfigOK = true;
                break;

            default:
                break;
            }
            if (pulseWidthConfigOK == true && pulseFrequencyConfigOK == true && averageNumberConfigOK == true
                && samplePointsConfigOK == true && sampleDelayPointsConfigOK == true && sampleFrequencyConfigOK == true && biasVoltageConfigOK == true)
            {
                qDebug() << "FPGA init OK";
                fpgaInitFlag = false;
                emit fpgaInitSuccess();
            }
        }
        else
        {
            responseReceived = ::parseFpgaCommand(datagram, fpgacmd);

            if (responseReceived)
            {
                switch (fpgacmd.cmdCode)
                {
                case CMD_CONFIG_PULSE_WIDTH:
                    svrCmd = buildServerCommand(SCMD_CONFIG_PULSE_WIDTH_ACK, 1);
                    emit transferCommandToServer(svrCmd);
                    qDebug() << svrCmd.toHex();
                    break;

                case CMD_CONTROL_TEST:
                    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);

                    if(fpgacmd.result == 1)
                    {
                        if(GlobalVars::systemState == SERVER_START_TEST)
                        {
                            svrCmd = buildServerCommand(SCMD_START_TEST_ACK, 1);
                            emit transferCommandToServer(svrCmd);
                            qDebug() << svrCmd.toHex();
                        }
                        GlobalVars::systemState = SYSTEM_TEST;
                    }
                    else if(fpgacmd.result == 0)
                    {
                        if(GlobalVars::systemState == SERVER_STOP_TEST)
                        {
                            svrCmd = buildServerCommand(SCMD_STOP_TEST_ACK, 1);
                            emit transferCommandToServer(svrCmd);
                            qDebug() << svrCmd.toHex();
                        }
                        GlobalVars::systemState = SYSTEM_IDLE;
                    }
                    break;

                default:
                    return;
                }
            }
        }
    }
}

void FpgaCommunicator::startFpgaHandshake()
{
    sendHandshakeFrame();
    fpgaReconnectTimer->start(10000);  // 10s 重试
}

void FpgaCommunicator::sendHandshakeFrame()
{
    qint64 sent = fpgaSendCmdSocket->writeDatagram(handshakeSendFrame, fpgaAddress, fpgaSendCmdPort);
    if (sent != handshakeSendFrame.size())
    {
        qWarning() << "Failed to send handshake frame!";
        qDebug() << "send frame size:" << sent;
    }
}

void FpgaCommunicator::onHandshakeTimeout()
{
    qInfo() << "Handshake timeout, retrying...";
    sendHandshakeFrame();
}

void FpgaCommunicator::initFpga()
{
    qInfo() << "Initing FPGA...";

    QList<QByteArray> commands = {
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLEPOINTS, GlobalVars::lineSamplePoints),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_PULSE_FREQUENCY, GlobalVars::signalSampleFrequency),
        //buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_PULSE_FREQUENCY, 5000),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_PULSE_WIDTH, 100),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_AVERAGE_NUMBER, GlobalVars::signalAverageNumber),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_DELAYPOINTS, 33),
        //buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_DELAYPOINTS, 25),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_FREQUENCY, 5),
        //buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_FREQUENCY, 4),
        buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_ADC_BIAS_VOLTAGE, -1000)
    };

    // 创建局部事件循环
    QEventLoop eventLoop;
    QTimer delayTimer;
    delayTimer.setSingleShot(true);

    for (int i = 0; i < commands.size(); i++) {
        sendFpgaCmd(commands[i]);
        qDebug() << "Sent command" << i + 1 << "of" << commands.size();

        // 如果不是最后一个命令，则延时1ms
        if (i < commands.size() - 1) {
            QObject::connect(&delayTimer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
            delayTimer.start(2);
            eventLoop.exec();
            delayTimer.disconnect();
        }
    }
}
