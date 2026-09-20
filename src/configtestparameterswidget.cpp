#include "configtestparameterswidget.h"
#include "message.h"
#include "GlobalVars.h"
#include <QGridLayout>
#include <QDebug>

ConfigTestParametersWidget::ConfigTestParametersWidget(QWidget *parent)
    : QWidget{parent}
{
    samplePoints = new QLabel(this);
    samplePoints->setText(tr("采样点数"));
    inputSamplePoints = new QComboBox(this);
    inputSamplePoints->addItem(tr("1024"), static_cast<int>(1024));
    inputSamplePoints->addItem(tr("2048"), static_cast<int>(2048));
    inputSamplePoints->addItem(tr("4096"), static_cast<int>(4096));
    inputSamplePoints->addItem(tr("10000"), static_cast<int>(10000));
    inputSamplePoints->addItem(tr("20000"), static_cast<int>(20000));
    inputSamplePoints->addItem(tr("32000"), static_cast<int>(32000));
    inputSamplePoints->setCurrentIndex(4);
    samplePointsUnit = new QLabel(this);
    samplePointsUnit->setText(tr("点"));
    configSamplePointsButton = new QPushButton(this);
    configSamplePointsButton->setText(tr("下发"));

    sampleDelayPoints = new QLabel(this);
    sampleDelayPoints->setText(tr("采样延时点数"));
    inputSampleDelayPoints = new QComboBox(this);
    inputSampleDelayPoints->addItem(tr("0"), static_cast<int>(0));
    inputSampleDelayPoints->addItem(tr("20"), static_cast<int>(20));
    inputSampleDelayPoints->addItem(tr("50"), static_cast<int>(50));
    inputSampleDelayPoints->addItem(tr("100"), static_cast<int>(100));
    inputSampleDelayPoints->addItem(tr("200"), static_cast<int>(200));
    inputSampleDelayPoints->addItem(tr("500"), static_cast<int>(500));
    inputSampleDelayPoints->setCurrentIndex(1);
    sampleDelayPointsUnit = new QLabel(this);
    sampleDelayPointsUnit->setText(tr("点"));
    configSampleDelayPointsButton = new QPushButton(this);
    configSampleDelayPointsButton->setText(tr("下发"));

    pulseFrequency = new QLabel(this);
    pulseFrequency->setText(tr("同步脉冲频率："));
    inputPulseFrequency = new QComboBox(this);
    inputPulseFrequency->addItem(tr("128"), static_cast<int>(128));
    inputPulseFrequency->addItem(tr("256"), static_cast<int>(256));
    inputPulseFrequency->addItem(tr("512"), static_cast<int>(512));
    inputPulseFrequency->addItem(tr("1024"), static_cast<int>(1024));
    inputPulseFrequency->addItem(tr("2048"), static_cast<int>(2048));
    inputPulseFrequency->addItem(tr("4096"), static_cast<int>(4096));
    inputPulseFrequency->addItem(tr("8192"), static_cast<int>(8192));
    inputPulseFrequency->addItem(tr("10000"), static_cast<int>(10000));
    inputPulseFrequency->setCurrentIndex(5);
    pulseFrequencyUnit = new QLabel(this);
    pulseFrequencyUnit->setText(tr("Hz"));
    configPulseFrequencyButton = new QPushButton(this);
    configPulseFrequencyButton->setText(tr("下发"));

    pulseWidth = new QLabel(this);
    pulseWidth->setText(tr("同步脉冲宽度："));
    inputPulseWidth = new QComboBox(this);
    inputPulseWidth->addItem(tr("40"), static_cast<int>(40));
    inputPulseWidth->addItem(tr("100"), static_cast<int>(100));
    inputPulseWidth->addItem(tr("200"), static_cast<int>(200));
    inputPulseWidth->addItem(tr("500"), static_cast<int>(500));
    inputPulseWidth->addItem(tr("1000"), static_cast<int>(1000));
    inputPulseWidth->setCurrentIndex(1);
    pulseWidthUnit = new QLabel(this);
    pulseWidthUnit->setText(tr("ns"));
    configPulseWidthButton = new QPushButton(this);
    configPulseWidthButton->setText(tr("下发"));

    sampleFrequency = new QLabel(this);
    sampleFrequency->setText(tr("采样频率；"));
    inputSampleFrequency = new QComboBox(this);
    inputSampleFrequency->addItem(tr("10MHz"), static_cast<int>(1));
    inputSampleFrequency->addItem(tr("20MHz"), static_cast<int>(2));
    inputSampleFrequency->addItem(tr("40MHz"), static_cast<int>(3));
    inputSampleFrequency->addItem(tr("50MHz"), static_cast<int>(4));
    inputSampleFrequency->addItem(tr("100MHz"), static_cast<int>(5));
    inputSampleFrequency->setCurrentIndex(4);
    configSampleFrequencyButton = new QPushButton(this);
    configSampleFrequencyButton->setText(tr("下发"));

    averageNumber = new QLabel(this);
    averageNumber->setText(tr("平均次数："));
    inputAverageNumber = new QComboBox(this);
    inputAverageNumber->addItem(tr("1"), static_cast<int>(1));
    inputAverageNumber->addItem(tr("2"), static_cast<int>(2));
    inputAverageNumber->addItem(tr("4"), static_cast<int>(4));
    inputAverageNumber->addItem(tr("8"), static_cast<int>(8));
    inputAverageNumber->addItem(tr("16"), static_cast<int>(16));
    inputAverageNumber->addItem(tr("32"), static_cast<int>(32));
    inputAverageNumber->addItem(tr("64"), static_cast<int>(64));
    inputAverageNumber->addItem(tr("128"), static_cast<int>(128));
    inputAverageNumber->setCurrentIndex(3);
    averageNumberUnit = new QLabel(this);
    averageNumberUnit->setText(tr("次"));
    configAverageNumberButton = new QPushButton(this);
    configAverageNumberButton->setText(tr("下发"));

    sampleChannels = new QLabel(this);
    sampleChannels->setText(tr("采样通道数"));
    inputSampleChannels = new QComboBox(this);
    inputSampleChannels->addItem(tr("单通道"), static_cast<int>(0));
    inputSampleChannels->addItem(tr("双通道"), static_cast<int>(1));
    inputSampleChannels->setCurrentIndex(0);
    configSampleChannelsButton = new QPushButton(this);
    configSampleChannelsButton->setText(tr("下发"));

    biasVoltage = new QLabel(this);
    biasVoltage->setText(tr("偏置电压"));
    inputBiasVoltage = new QComboBox(this);
    inputBiasVoltage->addItem(tr("0mV"), static_cast<int>(0));
    inputBiasVoltage->addItem(tr("+100mV"), static_cast<int>(100));
    inputBiasVoltage->addItem(tr("-100mV"), static_cast<int>(-100));
    inputBiasVoltage->addItem(tr("+200mV"), static_cast<int>(200));
    inputBiasVoltage->addItem(tr("-200mV"), static_cast<int>(-200));
    inputBiasVoltage->addItem(tr("+300mV"), static_cast<int>(300));
    inputBiasVoltage->addItem(tr("-300mV"), static_cast<int>(-300));
    inputBiasVoltage->addItem(tr("+400mV"), static_cast<int>(400));
    inputBiasVoltage->addItem(tr("-400mV"), static_cast<int>(-400));
    inputBiasVoltage->addItem(tr("+500mV"), static_cast<int>(500));
    inputBiasVoltage->addItem(tr("-500mV"), static_cast<int>(-500));
    inputBiasVoltage->addItem(tr("+600mV"), static_cast<int>(600));
    inputBiasVoltage->addItem(tr("-600mV"), static_cast<int>(-600));
    inputBiasVoltage->addItem(tr("+700mV"), static_cast<int>(700));
    inputBiasVoltage->addItem(tr("-700mV"), static_cast<int>(-700));
    inputBiasVoltage->addItem(tr("+800mV"), static_cast<int>(800));
    inputBiasVoltage->addItem(tr("-800mV"), static_cast<int>(-800));
    inputBiasVoltage->addItem(tr("+900mV"), static_cast<int>(900));
    inputBiasVoltage->addItem(tr("-900mV"), static_cast<int>(-900));
    inputBiasVoltage->addItem(tr("+1000mV"), static_cast<int>(1000));
    inputBiasVoltage->addItem(tr("-1000mV"), static_cast<int>(-1000));
    inputBiasVoltage->setCurrentIndex(20);
    configBiasVoltageButton = new QPushButton(this);
    configBiasVoltageButton->setText(tr("下发"));

    averageStatus = new QLabel(this);
    averageStatus->setText(tr("设置平均"));
    inputAverageStatus = new QComboBox(this);
    inputAverageStatus->addItem(tr("关闭"), static_cast<int>(0));
    inputAverageStatus->addItem(tr("开启"), static_cast<int>(1));
    inputAverageStatus->setCurrentIndex(0);
    configAverageStatusButton = new QPushButton(this);
    configAverageStatusButton->setText(tr("下发"));

    differenceStatus = new QLabel(this);
    differenceStatus->setText(tr("设置差分"));
    inputDifferenceStatus = new QComboBox(this);
    inputDifferenceStatus->addItem(tr("关闭"), static_cast<int>(0));
    inputDifferenceStatus->addItem(tr("开启"), static_cast<int>(1));
    inputDifferenceStatus->setCurrentIndex(0);
    configDifferenceStatusButton = new QPushButton(this);
    configDifferenceStatusButton->setText(tr("下发"));

    triggerSource = new QLabel(this);
    triggerSource->setText(tr("设置ADC采样触发模式"));
    inputTriggerSource = new QComboBox(this);
    inputTriggerSource->addItem(tr("内部触发"), static_cast<int>(0));
    inputTriggerSource->addItem(tr("外部触发"), static_cast<int>(1));
    inputTriggerSource->setCurrentIndex(0);
    configTriggerSourceButton = new QPushButton(this);
    configTriggerSourceButton->setText(tr("下发"));

    spectrumLowFrequency = new QLabel(this);
    spectrumLowFrequency->setText(tr("低频截止频率："));
    inputSpectrumLowFrequency = new QComboBox(this);
    inputSpectrumLowFrequency->addItem(tr("10"), static_cast<int>(10));
    inputSpectrumLowFrequency->addItem(tr("20"), static_cast<int>(20));
    inputSpectrumLowFrequency->addItem(tr("30"), static_cast<int>(30));
    inputSpectrumLowFrequency->addItem(tr("40"), static_cast<int>(40));
    inputSpectrumLowFrequency->addItem(tr("50"), static_cast<int>(50));
    inputSpectrumLowFrequency->addItem(tr("60"), static_cast<int>(60));
    inputSpectrumLowFrequency->addItem(tr("70"), static_cast<int>(70));
    inputSpectrumLowFrequency->addItem(tr("80"), static_cast<int>(80));
    inputSpectrumLowFrequency->setCurrentIndex(2);
    spectrumLowFrequencyUnit = new QLabel(this);
    spectrumLowFrequencyUnit->setText(tr("Hz"));
    configSpectrumLowFrequencyButton = new QPushButton(this);
    configSpectrumLowFrequencyButton->setText(tr("下发"));

    spectrumHighFrequency = new QLabel(this);
    spectrumHighFrequency->setText(tr("低频截止频率："));
    inputSpectrumHighFrequency = new QComboBox(this);
    inputSpectrumHighFrequency->addItem(tr("110"), static_cast<int>(110));
    inputSpectrumHighFrequency->addItem(tr("120"), static_cast<int>(120));
    inputSpectrumHighFrequency->addItem(tr("130"), static_cast<int>(130));
    inputSpectrumHighFrequency->addItem(tr("140"), static_cast<int>(140));
    inputSpectrumHighFrequency->addItem(tr("150"), static_cast<int>(150));
    inputSpectrumHighFrequency->addItem(tr("160"), static_cast<int>(160));
    inputSpectrumHighFrequency->addItem(tr("170"), static_cast<int>(170));
    inputSpectrumHighFrequency->addItem(tr("180"), static_cast<int>(180));
    inputSpectrumHighFrequency->setCurrentIndex(4);
    spectrumHighFrequencyUnit = new QLabel(this);
    spectrumHighFrequencyUnit->setText(tr("Hz"));
    configSpectrumHighFrequencyButton = new QPushButton(this);
    configSpectrumHighFrequencyButton->setText(tr("下发"));

    rmsDataStatus = new QLabel(this);
    rmsDataStatus->setText(tr("设置RMS数据"));
    inputRMSDataStatus = new QComboBox(this);
    inputRMSDataStatus->addItem(tr("关闭"), static_cast<int>(0));
    inputRMSDataStatus->addItem(tr("开启"), static_cast<int>(1));
    inputRMSDataStatus->setCurrentIndex(0);
    configRMSDataStatusButton = new QPushButton(this);
    configRMSDataStatusButton->setText(tr("下发"));

    QGridLayout *configLayout = new QGridLayout(this);

    configLayout->addWidget(samplePoints, 0, 0);
    configLayout->addWidget(inputSamplePoints, 0, 1);
    configLayout->addWidget(samplePointsUnit, 0, 2);
    configLayout->addWidget(configSamplePointsButton, 0, 3);

    configLayout->addWidget(sampleDelayPoints, 1, 0);
    configLayout->addWidget(inputSampleDelayPoints, 1, 1);
    configLayout->addWidget(sampleDelayPointsUnit, 1, 2);
    configLayout->addWidget(configSampleDelayPointsButton, 1, 3);

    configLayout->addWidget(pulseFrequency, 2, 0);
    configLayout->addWidget(inputPulseFrequency, 2, 1);
    configLayout->addWidget(pulseFrequencyUnit, 2, 2);
    configLayout->addWidget(configPulseFrequencyButton, 2, 3);

    configLayout->addWidget(pulseWidth, 3, 0);
    configLayout->addWidget(inputPulseWidth, 3, 1);
    configLayout->addWidget(pulseWidthUnit, 3, 2);
    configLayout->addWidget(configPulseWidthButton, 3, 3);

    configLayout->addWidget(sampleFrequency, 4, 0);
    configLayout->addWidget(inputSampleFrequency, 4, 1);
    configLayout->addWidget(configSampleFrequencyButton, 4, 3);

    configLayout->addWidget(averageNumber, 5, 0);
    configLayout->addWidget(inputAverageNumber, 5, 1);
    configLayout->addWidget(averageNumberUnit, 5, 2);
    configLayout->addWidget(configAverageNumberButton, 5, 3);

    configLayout->addWidget(sampleChannels, 6, 0);
    configLayout->addWidget(inputSampleChannels, 6, 1);
    configLayout->addWidget(configSampleChannelsButton, 6, 3);

    configLayout->addWidget(biasVoltage, 7, 0);
    configLayout->addWidget(inputBiasVoltage, 7, 1);
    configLayout->addWidget(configBiasVoltageButton, 7, 3);

    configLayout->addWidget(averageStatus, 8, 0);
    configLayout->addWidget(inputAverageStatus, 8, 1);
    configLayout->addWidget(configAverageStatusButton, 8, 3);

    configLayout->addWidget(differenceStatus, 9, 0);
    configLayout->addWidget(inputDifferenceStatus, 9, 1);
    configLayout->addWidget(configDifferenceStatusButton, 9, 3);

    configLayout->addWidget(triggerSource, 10, 0);
    configLayout->addWidget(inputTriggerSource, 10, 1);
    configLayout->addWidget(configTriggerSourceButton, 10, 3);

    configLayout->addWidget(spectrumLowFrequency, 11, 0);
    configLayout->addWidget(inputSpectrumLowFrequency, 11, 1);
    configLayout->addWidget(spectrumLowFrequencyUnit, 11, 2);
    configLayout->addWidget(configSpectrumLowFrequencyButton, 11, 3);

    configLayout->addWidget(spectrumHighFrequency, 12, 0);
    configLayout->addWidget(inputSpectrumHighFrequency, 12, 1);
    configLayout->addWidget(spectrumHighFrequencyUnit, 12, 2);
    configLayout->addWidget(configSpectrumHighFrequencyButton, 12, 3);

    configLayout->addWidget(rmsDataStatus, 13, 0);
    configLayout->addWidget(inputRMSDataStatus, 13, 1);
    configLayout->addWidget(configRMSDataStatusButton, 13, 3);

    connect(configSamplePointsButton, SIGNAL(clicked()), this, SLOT(configSamplePoints()));
    connect(configSampleDelayPointsButton, SIGNAL(clicked()), this, SLOT(configSampleDelayPoints()));
    connect(configPulseFrequencyButton, SIGNAL(clicked()), this, SLOT(configPulseFrequency()));
    connect(configPulseWidthButton, SIGNAL(clicked()), this, SLOT(configPulseWidth()));
    connect(configSampleFrequencyButton, SIGNAL(clicked()), this, SLOT(configSampleFrequency()));
    connect(configAverageNumberButton, SIGNAL(clicked()), this, SLOT(configAverageNumber()));
    connect(configSampleChannelsButton, SIGNAL(clicked()), this, SLOT(configSampleChannels()));
    connect(configBiasVoltageButton, SIGNAL(clicked()), this, SLOT(configBiasVoltage()));
    connect(configAverageStatusButton, SIGNAL(clicked()), this, SLOT(configAverageStatus()));
    connect(configDifferenceStatusButton, SIGNAL(clicked()), this, SLOT(configDifferenceStatus()));
    connect(configTriggerSourceButton, SIGNAL(clicked()), this, SLOT(configTriggerSource()));
    connect(configSpectrumLowFrequencyButton, SIGNAL(clicked()), this, SLOT(configSpectrumLowFrequency()));
    connect(configSpectrumHighFrequencyButton, SIGNAL(clicked()), this, SLOT(configSpectrumHighFrequency()));
    connect(configRMSDataStatusButton, SIGNAL(clicked()), this, SLOT(configRMSDataStatus()));
}

void ConfigTestParametersWidget::configSamplePoints()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputSamplePoints->itemData(inputSamplePoints->currentIndex(), Qt::UserRole).toInt();
    //GlobalVars::lineSamplePoints = cmdParam;
    //qDebug() << "lineSamplePoints" << cmdParam;

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLEPOINTS, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configSampleDelayPoints()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputSampleDelayPoints->itemData(inputSampleDelayPoints->currentIndex(), Qt::UserRole).toInt();
    //GlobalVars::lineSamplePoints = cmdParam;
    //qDebug() << "lineSamplePoints" << cmdParam;

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_DELAYPOINTS, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configPulseFrequency()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputPulseFrequency->itemData(inputPulseFrequency->currentIndex(), Qt::UserRole).toInt();
    //GlobalVars::signalSampleFrequency = 1000000000 / cmdParam / 10;

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL,CMD_CONFIG_PULSE_FREQUENCY, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configPulseWidth()
{
    int cmdParam = inputPulseWidth->itemData(inputPulseWidth->currentIndex(), Qt::UserRole).toInt();

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_PULSE_WIDTH, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configSampleFrequency()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputSampleFrequency->itemData(inputSampleFrequency->currentIndex(), Qt::UserRole).toInt();
    //GlobalVars::adcSampleFrequency = 100 / (cmdParam + 1);

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_FREQUENCY, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configAverageNumber()
{
    //std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputAverageNumber->itemData(inputAverageNumber->currentIndex(), Qt::UserRole).toInt();
    //GlobalVars::signalACCNumber = cmdParam;

    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_AVERAGE_NUMBER, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configSampleChannels()
{
    int cmdParam = inputSampleChannels->itemData(inputSampleChannels->currentIndex(), Qt::UserRole).toInt();
    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_SAMPLE_CHANNELS, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configBiasVoltage()
{
    int cmdParam = inputBiasVoltage->itemData(inputBiasVoltage->currentIndex(), Qt::UserRole).toInt();
    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_ADC_BIAS_VOLTAGE, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configAverageStatus()
{
    int cmdParam = inputAverageStatus->itemData(inputAverageStatus->currentIndex(), Qt::UserRole).toInt();
    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_AVERAGE, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configDifferenceStatus()
{
    int cmdParam = inputDifferenceStatus->itemData(inputDifferenceStatus->currentIndex(), Qt::UserRole).toInt();
    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_DIFFERENCE, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configTriggerSource()
{
    int cmdParam = inputTriggerSource->itemData(inputTriggerSource->currentIndex(), Qt::UserRole).toInt();
    QByteArray configCmd(24, 0x00);
    configCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONFIG_ADC_TRIGGER_SOURCE, cmdParam);
    //qDebug()<<testCmd.toHex();
    emit sendConfigCmd(configCmd);
}

void ConfigTestParametersWidget::configSpectrumLowFrequency()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputSpectrumLowFrequency->itemData(inputSpectrumLowFrequency->currentIndex(), Qt::UserRole).toInt();
    GlobalVars::lowCutFrequency = cmdParam;
}

void ConfigTestParametersWidget::configSpectrumHighFrequency()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputSpectrumHighFrequency->itemData(inputSpectrumHighFrequency->currentIndex(), Qt::UserRole).toInt();
    GlobalVars::highCutFrequency = cmdParam;
}

void ConfigTestParametersWidget::configRMSDataStatus()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    int cmdParam = inputRMSDataStatus->itemData(inputRMSDataStatus->currentIndex(), Qt::UserRole).toInt();
    if(cmdParam == 1)
        GlobalVars::rmsDataTransmitEnable = true;
    else
        GlobalVars::rmsDataTransmitEnable = false;
}
