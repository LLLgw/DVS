#ifndef CONFIGTESTPARAMETERSWIDGET_H
#define CONFIGTESTPARAMETERSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

#define DEFAULT_PULSEWIDTH          "100"   //ns
#define DEFAULT_PULSEFREQUENCE      "4096"  //Hz
#define DEFAULT_ACCNUMBER           "8"     //平均次数:1,2,4,8,16,32,64,128
#define DEFAULT_SAMPLEPOINTS        "20000" //点
#define DEFAULT_SAMPLEDELAYPOINTS   "20"    //点
#define DEFAULT_SAMPLEFREQUENCY     "100"   //1:10MHz,2:20MHz,3:40MHz,4:50MHz,5:100MHz
#define DEFAULT_SAMPLECHANNELS      "0"     //0:1通道，1:2通道
#define DEFAULT_BIASVOLTAGE         "-1000" //0,1000mV,-1000mV
#define DEFAULT_AVERAGE_STATUS      "0"     //0:关闭,1:开启
#define DEFAULT_DIFFERENCE_STATUS   "0"     //0:关闭,1:开启
#define DEFAULT_TRIGGER_SOURCE      "0"     //0：内部触发,1:外部触发

class ConfigTestParametersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigTestParametersWidget(QWidget *parent = nullptr);
private:
    QLabel *samplePoints;
    QLabel *sampleDelayPoints;
    QLabel *pulseFrequency;
    QLabel *pulseWidth;
    QLabel *sampleFrequency;
    QLabel *averageNumber;
    QLabel *sampleChannels;
    QLabel *biasVoltage;
    QLabel *averageStatus;
    QLabel *differenceStatus;
    QLabel *triggerSource;
    QLabel *spectrumLowFrequency;
    QLabel *spectrumHighFrequency;
    QLabel *rmsDataStatus;

    QLabel *samplePointsUnit;
    QLabel *sampleDelayPointsUnit;
    QLabel *pulseFrequencyUnit;
    QLabel *pulseWidthUnit;
    QLabel *sampleFrequencyUnit;
    QLabel *averageNumberUnit;
    QLabel *biasVoltageUnit;
    QLabel *spectrumLowFrequencyUnit;
    QLabel *spectrumHighFrequencyUnit;

    QComboBox *inputSamplePoints;
    QComboBox *inputSampleDelayPoints;
    QComboBox *inputPulseFrequency;
    QComboBox *inputPulseWidth;
    QComboBox *inputSampleFrequency;
    QComboBox *inputAverageNumber;
    QComboBox *inputSampleChannels;
    QComboBox *inputBiasVoltage;
    QComboBox *inputAverageStatus;
    QComboBox *inputDifferenceStatus;
    QComboBox *inputTriggerSource;
    QComboBox *inputSpectrumLowFrequency;
    QComboBox *inputSpectrumHighFrequency;
    QComboBox *inputRMSDataStatus;

    QPushButton *configSamplePointsButton;
    QPushButton *configSampleDelayPointsButton;
    QPushButton *configPulseFrequencyButton;
    QPushButton *configPulseWidthButton;
    QPushButton *configSampleFrequencyButton;
    QPushButton *configAverageNumberButton;
    QPushButton *configSampleChannelsButton;
    QPushButton *configBiasVoltageButton;
    QPushButton *configAverageStatusButton;
    QPushButton *configDifferenceStatusButton;
    QPushButton *configTriggerSourceButton;
    QPushButton *configSpectrumLowFrequencyButton;
    QPushButton *configSpectrumHighFrequencyButton;
    QPushButton *configRMSDataStatusButton;

signals:
    void sendConfigCmd(QByteArray);

protected slots:
    //void SaveConfig();
    void configSamplePoints();
    void configSampleDelayPoints();
    void configPulseFrequency();
    void configPulseWidth();
    void configSampleFrequency();
    void configAverageNumber();
    void configSampleChannels();
    void configBiasVoltage();
    void configAverageStatus();
    void configDifferenceStatus();
    void configTriggerSource();
    void configSpectrumLowFrequency();
    void configSpectrumHighFrequency();
    void configRMSDataStatus();
};

#endif // CONFIGTESTPARAMETERSWIDGET_H
