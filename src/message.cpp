#include "message.h"
#include "GlobalVars.h"
#include <QDebug>
#include <QtEndian>
#include <QNetworkInterface>
QByteArray buildFpgaCommand(func_t funcCode, cmd_t cmdCode, int16_t param)
{
    QByteArray command(24, 0x00);

    // 帧头 (0xA55AAA5555AA)
    command[0] = 0xA5;
    command[1] = 0x5A;
    command[2] = 0xAA;
    command[3] = 0x55;
    command[4] = 0x55;
    command[5] = 0xAA;

    // 功能码 (2字节)
    command[6] = (funcCode>>8) & 0xFF;
    command[7] = funcCode & 0xFF;

    // 命令码 (2字节)
    command[8] = (cmdCode>>8) & 0xFF;
    command[9] = cmdCode & 0xFF;

    // 指令参数长度 (4字节，0x00000008)
    command[13] = 0x08;

    // 指令参数 (8字节)
    //command[22] = (param>>8) & 0xFF;
    //command[23] = param & 0xFF;

    qint64 param64 = static_cast<qint64>(param);  // 符号扩展

    // 大端序存储
    command[16] = static_cast<char>((param64 >> 56) & 0xFF);  // 最高字节
    command[17] = static_cast<char>((param64 >> 48) & 0xFF);
    command[18] = static_cast<char>((param64 >> 40) & 0xFF);
    command[19] = static_cast<char>((param64 >> 32) & 0xFF);
    command[20] = static_cast<char>((param64 >> 24) & 0xFF);
    command[21] = static_cast<char>((param64 >> 16) & 0xFF);
    command[22] = static_cast<char>((param64 >> 8) & 0xFF);
    command[23] = static_cast<char>(param64 & 0xFF);          // 最低字节

    Q_ASSERT(command.size() == 24);  // 验证长度
    return command;
}

bool parseFpgaCommand(const QByteArray &data, ftopcmdstruct_t &cmd)
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    //ftopcmdstruct_t command;

    //qDebug() << "Parsing fpga command";
    // 验证长度和帧头
    if (data.size() != 16 || data.mid(0, 6) != QByteArray("\x5A\xA5\x55\xAA\xAA\x55"))
        return false;

    // 提取功能码
    cmd.funcCode = static_cast<uint16_t>(data.at(6))<<8 | static_cast<uint16_t>(data.at(7));
    //qDebug() << "function code:" << command.funcCode;
    if(cmd.funcCode != 0x0002)
        return false;

    // 提取参数长度
    cmd.dataLen = static_cast<uint16_t>(data.at(10))<<8 | static_cast<uint16_t>(data.at(11));
    if(cmd.dataLen != 0x0004)
        return false;

    // 提取命令码
    cmd.cmdCode = static_cast<uint16_t>(data.at(12))<<8 | static_cast<uint16_t>(data.at(13));
    cmd.result = static_cast<uint16_t>(static_cast<uint8_t>(data.at(14))) << 8 |
                     static_cast<uint16_t>(static_cast<uint8_t>(data.at(15)));
    switch(cmd.cmdCode)
    {
        case CMD_CONFIG_SAMPLEPOINTS:
            GlobalVars::lineSamplePoints = cmd.result;
            qDebug() << "lineSamplePoints" << GlobalVars::lineSamplePoints;
            break;

        case CMD_CONFIG_PULSE_FREQUENCY:
            GlobalVars::signalSampleFrequency = cmd.result;
            qDebug() << "signalSampleFrequency" << GlobalVars::signalSampleFrequency;
            break;

        case CMD_CONTROL_AVERAGE:
            if(cmd.result == 1)
                GlobalVars::averageEnable = true;
            else
                GlobalVars::averageEnable = false;
            qDebug() << "Average state" << cmd.result;
            break;

        case CMD_CONFIG_SAMPLE_CHANNELS:
            qDebug() << "Sample channels" << cmd.result;
            break;

        case CMD_CONFIG_SAMPLE_DELAYPOINTS:
            GlobalVars::lineSampleDelayPoints = cmd.result;
            qDebug() << "Sample delay points" << cmd.result;
            break;

        case CMD_CONFIG_PULSE_WIDTH:
            qDebug() << "Pulse width" << cmd.result;
            break;

        case CMD_CONFIG_AVERAGE_NUMBER:
            GlobalVars::signalAverageNumber = cmd.result;
            qDebug() << "signalAverageNumber" << GlobalVars::signalAverageNumber;
            break;

        case CMD_CONTROL_DIFFERENCE:
            if(cmd.result == 1)
                GlobalVars::differenceEnable = true;
            else
                GlobalVars::differenceEnable = false;
            qDebug() << "Difference state" << cmd.result;
            break;

        case CMD_CONFIG_SAMPLE_FREQUENCY:
            switch(cmd.result)
            {
                case 1:
                    GlobalVars::adcSampleFrequency = 10;
                    break;
                case 2:
                    GlobalVars::adcSampleFrequency = 20;
                    break;
                case 3:
                    GlobalVars::adcSampleFrequency = 40;
                    break;
                case 4:
                    GlobalVars::adcSampleFrequency = 50;
                    break;
                case 5:
                    GlobalVars::adcSampleFrequency = 100;
                    break;
                default:
                    break;
            }
            qDebug() << "adcSampleFrequency" << GlobalVars::adcSampleFrequency;
            break;

        case CMD_CONFIG_ADC_BIAS_VOLTAGE:
            qDebug() << "ADC bias voltage" << cmd.result;
            break;

        case CMD_CONTROL_TEST:
            if (cmd.result == 1)
            {
                GlobalVars::kFullFrames = GlobalVars::lineSamplePoints / GlobalVars::kMaxSamplesPerFrame;  // 完整帧数
                GlobalVars::kRemainingSamples = GlobalVars::lineSamplePoints % GlobalVars::kMaxSamplesPerFrame; // 剩余采样点
                GlobalVars::kTotalFramesPerCycle = GlobalVars::kRemainingSamples ? (GlobalVars::kFullFrames + 1) : GlobalVars::kFullFrames;

                int totalAverage = GlobalVars::signalAverageNumber * GlobalVars::signalSoftwareAverageNumber;
                if (totalAverage <= 0)
                    totalAverage = 1;

                GlobalVars::deltaFrequency = (double)GlobalVars::signalSampleFrequency / GlobalVars::signalSamplePoints / totalAverage;
                if (GlobalVars::deltaFrequency < 0.01)
                    GlobalVars::deltaFrequency = 0.01;
                GlobalVars::minFrequencyPoint = GlobalVars::lowCutFrequency / GlobalVars::deltaFrequency;
                GlobalVars::maxFrequencyPoint = GlobalVars::highCutFrequency / GlobalVars::deltaFrequency;
                int maxFreq = GlobalVars::signalSamplePoints / 2;
                if (GlobalVars::minFrequencyPoint >= maxFreq)
                    GlobalVars::minFrequencyPoint = maxFreq -1;
                if (GlobalVars::maxFrequencyPoint >= maxFreq)
                    GlobalVars::maxFrequencyPoint = maxFreq -1;
                //qDebug() << "delta freq:" << GlobalVars::deltaFrequency;
                //qDebug() << "High freq:" << GlobalVars::maxFrequencyPoint;
                //qDebug() << "Low freq:" << GlobalVars::minFrequencyPoint;
            }

            qDebug() << "test start or stop" << cmd.result;
            break;

        default:
            break;
    }

    return true;
}

QByteArray buildServerCommand(svrcmd_t cmdCode, quint32 param)
{
    QByteArray command(44, 0x00);

    //帧头 (0x5A)
    command[0] = 0x5A;

    //帧长 (1Byte)
    command[1] = 44;

    //指令码 (1Byte)
    command[2] = cmdCode;

    //指令参数长度及参数 (1Byte)
    if (cmdCode == SCMD_DEV_LOG_REQ)
    {
        QString baseKey = "SA1YA217DA0";   // 固定前缀（前 11 位）
        QString macLastByte = "00";

        // 找 eth0 的 MAC
        foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
            if (iface.name() == "eth0") {
                QString mac = iface.hardwareAddress();   // 例如 "AA:BB:CC:DD:EE:F7"
                qDebug() << "eth0 mac:" << mac;

                if (!mac.isEmpty()) {
                    QStringList parts = mac.split(":");
                    if (parts.size() == 6) {
                        macLastByte = parts.last().toUpper();  // "F7"
                    }
                }
                break;
            }
        }

        // 拼完整 product_key
        QString productKey = baseKey + macLastByte;

        // 生成 JSON 字符串
        QString deviceId = QString("{\"product_key\":\"%1\"}").arg(productKey);

        const QByteArray devIdBytes = deviceId.toUtf8();
        const quint8 devIdLen = static_cast<quint8>(qMin(devIdBytes.size(), 40));
        command[3] = devIdLen;
        memcpy(command.data() + 4, devIdBytes.constData(), devIdLen);
    }
    else
    {
        command[3] = 4;
        command[4] = (param>>24) & 0xFF;
        command[5] = (param>>16) & 0xFF;
        command[6] = (param>>8) & 0xFF;
        command[7] = param & 0xFF;
    }

    return command;
}

bool parseServerCommand(const QByteArray &data, svrcmdstruct_t &cmd)
{
    //qDebug() << data.toHex();
    // 验证长度和帧头
    if (data.size() != 44 || data[0] != 0x5A)
    {
        return false;
    }

    // 提取指令码和指令参数
    cmd.cmdCode = static_cast<uint8_t>(data.at(2));
    cmd.paramLen = static_cast<uint8_t>(data.at(3));
    cmd.param = static_cast<uint32_t>(static_cast<uint8_t>(data.at(4))) << 24 |
                static_cast<uint32_t>(static_cast<uint8_t>(data.at(5))) << 16 |
                static_cast<uint32_t>(static_cast<uint8_t>(data.at(6))) << 8 |
                static_cast<uint8_t>(data.at(7));

    return true;
}

QByteArray buildServerCommand2(svrcmd_t cmdCode, quint32 param1, quint32 param2)
{
    QByteArray command(44, 0x00);

    command[0] = 0x5A;
    command[1] = 44;
    command[2] = cmdCode;
    command[3] = 8;

    command[4]  = (param1 >> 24) & 0xFF;
    command[5]  = (param1 >> 16) & 0xFF;
    command[6]  = (param1 >> 8) & 0xFF;
    command[7]  = param1 & 0xFF;

    command[8]  = (param2 >> 24) & 0xFF;
    command[9]  = (param2 >> 16) & 0xFF;
    command[10] = (param2 >> 8) & 0xFF;
    command[11] = param2 & 0xFF;

    return command;
}
