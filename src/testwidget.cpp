#include "testwidget.h"
#include "message.h"
#include "GlobalVars.h"
#include <QVBoxLayout>
#include <QDebug>

TestWidget::TestWidget(QWidget *parent)
    : QWidget{parent}
{
    enableTestButton = new QPushButton(this);
    enableTestButton->setText(tr("启动DVS测试"));
    enableTestButton->setEnabled(true);
    disableTestButton = new QPushButton(this);
    disableTestButton->setText(tr("停止DVS测试"));
    disableTestButton->setEnabled(true);
    rawDataButton = new QPushButton(this);
    rawDataButton->setText(tr("显示原始数据"));
    rawDataButton->setEnabled(true);

    QVBoxLayout *configLayout = new QVBoxLayout(this);
    configLayout->addWidget(enableTestButton);
    configLayout->addWidget(disableTestButton);
    configLayout->addWidget(rawDataButton);

    connect(enableTestButton, SIGNAL(clicked(bool)), this, SLOT(startTest()));
    connect(disableTestButton, SIGNAL(clicked(bool)), this, SLOT(stopTest()));
    connect(rawDataButton, SIGNAL(clicked(bool)), this, SLOT(displayRawData()));
}

void TestWidget::startTest()
{

    //GlobalVars::kFullFrames = GlobalVars::lineSamplePoints / GlobalVars::kMaxSamplesPerFrame;  // 完整帧数
    //GlobalVars::kRemainingSamples = GlobalVars::lineSamplePoints % GlobalVars::kMaxSamplesPerFrame; // 剩余采样点
    //GlobalVars::kTotalFramesPerCycle = GlobalVars::kRemainingSamples ? (GlobalVars::kFullFrames + 1) : GlobalVars::kFullFrames;
    // 计算频点频率
    //GlobalVars::deltaFrequency = GlobalVars::signalSampleFrequency / GlobalVars::signalSamplePoints / GlobalVars::signalACCNumber;
    //if (GlobalVars::deltaFrequency < 0.01)
    //    GlobalVars::deltaFrequency = 0.01;
    //GlobalVars::minFrequencyPoint = GlobalVars::lowCutFrequency / GlobalVars::deltaFrequency;
    //GlobalVars::maxFrequencyPoint = GlobalVars::highCutFrequency / GlobalVars::deltaFrequency;
    //int maxFreq = GlobalVars::signalSamplePoints / 2;
    //if (GlobalVars::maxFrequencyPoint > maxFreq)
    //    GlobalVars::maxFrequencyPoint = maxFreq;
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    if(GlobalVars::systemState == SYSTEM_IDLE)
    {
        GlobalVars::systemState = SYSTEM_TEST;
        quint16 cmdParam = 1;

        QByteArray testCmd(24, 0x00);
        testCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_TEST, cmdParam);
        //qDebug()<<testCmd.toHex();
        emit sendTestCmd(testCmd);
        emit startReceiveData();
        enableTestButton->setEnabled(false);
        disableTestButton->setEnabled(true);
    }
}

void TestWidget::stopTest()
{
    std::lock_guard<std::mutex> lock(GlobalVars::g_mutex);
    if(GlobalVars::systemState == SYSTEM_TEST)
    {
        GlobalVars::systemState = SYSTEM_IDLE;
        quint16 cmdParam = 0;

        QByteArray testCmd(24, 0x00);
        testCmd = buildFpgaCommand(FUNC_CONTROL, CMD_CONTROL_TEST, cmdParam);
        //qDebug()<<testCmd.toHex();
        emit sendTestCmd(testCmd);
        emit stopReceiveData();
        disableTestButton->setEnabled(false);
        enableTestButton->setEnabled(true);
    }
}

void TestWidget::displayRawData()
{
    if (GlobalVars::rawDataDisplayEnable == true)
    {
        GlobalVars::rawDataDisplayEnable = false;
        rawDataButton->setText(tr("显示原始数据"));
    }
    else
    {
        GlobalVars::rawDataDisplayEnable = true;
        rawDataButton->setText(tr("屏蔽原始数据"));
    }
}
