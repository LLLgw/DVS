#include "queryparameterswidget.h"
#include "message.h"
#include <QVBoxLayout>
#include <QDebug>

QueryParametersWidget::QueryParametersWidget(QWidget *parent)
    : QWidget{parent}
{
    querySamplePointsButton = new QPushButton(this);
    querySamplePointsButton->setText(tr("查询采样点数"));
    querySamplePointsButton->setEnabled(true);
    querySampleDelayPointsButton = new QPushButton(this);
    querySampleDelayPointsButton->setText(tr("查询采样延时点数"));
    querySampleDelayPointsButton->setEnabled(true);
    queryPulseFrequencyButton = new QPushButton(this);
    queryPulseFrequencyButton->setText(tr("查询脉冲频率"));
    queryPulseFrequencyButton->setEnabled(true);
    queryPulseWidthButton = new QPushButton(this);
    queryPulseWidthButton->setText(tr("查询脉冲宽度"));
    queryPulseWidthButton->setEnabled(true);
    querySampleFrequencyButton = new QPushButton(this);
    querySampleFrequencyButton->setText(tr("查询采样频率"));
    querySampleFrequencyButton->setEnabled(true);
    queryAverageNumberButton = new QPushButton(this);
    queryAverageNumberButton->setText(tr("查询平均次数"));
    queryAverageNumberButton->setEnabled(true);
    queryBiasVoltageButton = new QPushButton(this);
    queryBiasVoltageButton->setText(tr("偏置电压"));
    queryBiasVoltageButton->setEnabled(true);
    querySampleChannelsButton = new QPushButton(this);
    querySampleChannelsButton->setText(tr("查询采样通道数"));
    querySampleChannelsButton->setEnabled(true);
    queryAverageStatusButton = new QPushButton(this);
    queryAverageStatusButton->setText(tr("查询平均开启状态"));
    queryAverageStatusButton->setEnabled(true);
    queryDifferenceStatusButton = new QPushButton(this);
    queryDifferenceStatusButton->setText(tr("查询差分开启状态"));
    queryDifferenceStatusButton->setEnabled(true);
    queryTriggerSourceButton = new QPushButton(this);
    queryTriggerSourceButton->setText(tr("查询ADC触发源"));
    queryTriggerSourceButton->setEnabled(true);
    QVBoxLayout *configLayout = new QVBoxLayout(this);
    configLayout->addWidget(querySamplePointsButton);
    configLayout->addWidget(querySampleDelayPointsButton);
    configLayout->addWidget(queryPulseFrequencyButton);
    configLayout->addWidget(queryPulseWidthButton);
    configLayout->addWidget(querySampleFrequencyButton);
    configLayout->addWidget(queryAverageNumberButton);
    configLayout->addWidget(queryBiasVoltageButton);
    configLayout->addWidget(querySampleChannelsButton);
    configLayout->addWidget(queryAverageStatusButton);
    configLayout->addWidget(queryDifferenceStatusButton);
    configLayout->addWidget(queryTriggerSourceButton);
    connect(querySamplePointsButton, SIGNAL(clicked(bool)), this, SLOT(querySamplePoints()));
    connect(querySampleDelayPointsButton, SIGNAL(clicked(bool)), this, SLOT(querySampleDelayPoints()));
    connect(queryPulseFrequencyButton, SIGNAL(clicked(bool)), this, SLOT(queryPulseFrequency()));
    connect(queryPulseWidthButton, SIGNAL(clicked(bool)), this, SLOT(queryPulseWidth()));
    connect(querySampleFrequencyButton, SIGNAL(clicked(bool)), this, SLOT(querySampleFrequency()));
    connect(queryAverageNumberButton, SIGNAL(clicked(bool)), this, SLOT(queryAverageNumber()));
    connect(queryBiasVoltageButton, SIGNAL(clicked(bool)), this, SLOT(queryBiasVoltage()));
    connect(querySampleChannelsButton, SIGNAL(clicked(bool)), this, SLOT(querySampleChannels()));
    connect(queryAverageStatusButton, SIGNAL(clicked(bool)), this, SLOT(queryAverageStatus()));
    connect(queryDifferenceStatusButton, SIGNAL(clicked(bool)), this, SLOT(queryDifferenceStatus()));
    connect(queryTriggerSourceButton, SIGNAL(clicked(bool)), this, SLOT(queryTriggerSource()));
}

void QueryParametersWidget::querySamplePoints()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_SAMPLEPOINTS, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::querySampleDelayPoints()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_SAMPLE_DELAYPOINTS, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryPulseFrequency()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_PULSE_FREQUENCY, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryPulseWidth()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_PULSE_WIDTH, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::querySampleFrequency()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_SAMPLE_FREQUENCY, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryAverageNumber()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_AVERAGE_NUMBER, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryBiasVoltage()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_ADC_BIAS_VOLTAGE, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::querySampleChannels()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_SAMPLE_CHANNELS, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryAverageStatus()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONTROL_AVERAGE, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryDifferenceStatus()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONTROL_DIFFERENCE, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}

void QueryParametersWidget::queryTriggerSource()
{
    quint16 param = 0;

    QByteArray queryCmd(24, 0x00);
    queryCmd = buildFpgaCommand(FUNC_QUERY, CMD_CONFIG_ADC_TRIGGER_SOURCE, param);
    qDebug()<<queryCmd.toHex();
    emit sendQueryCmd(queryCmd);
}
