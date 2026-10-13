#ifndef SPECTRUMPROCESSOR_H
#define SPECTRUMPROCESSOR_H

#include "buffer_state.h"
#include <QObject>
#include <QUdpSocket>
#include <QDateTime>
#include <complex>
#include <vector>
#include <cstdint>
#include <QVector>
#include <QTcpSocket>
#include "vkFFT.h"
#include <fftw3.h>

class SpectrumProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumProcessor(std::shared_ptr<SharedBuffer> shared,QObject *parent = nullptr);
        ~SpectrumProcessor();

signals:
    void specDataProcessed(const QVector<int16_t>& spectrum);
    void processorBusy(bool busy);
    void Fkp_local(uint32_t dz,uint32_t freqIndex);
    void rmsRawDataReady(QVector<float>);

public slots:
    void processSpectrumData(QVector<QVector<int16_t>>* bufferPtr,int bufferIndex);//改

private:
    QVector<float> hammingWindow; // 修改为 float
    QVector<float> hannWindow;    // 修改为 float
    std::vector<std::complex<float>> twiddleFactors; // 修改为 float

    void precomputeTwiddleFactors(int size);

    uint64_t sampleCount;           //总采样点数
    uint64_t bufferCount;           //批量
    uint64_t bufferSize;            //数据大小
    VkFFTApplication app;
    bool initialized;               //opencl初始化

    void checkCL(cl_int code, const char * operation);
    void CreateOpencl();
    void CreateBuffer();
    void initializeVkFFT_();
    void checkVkFFT(VkFFTResult code, const char * operation);


    QVector<float> peakPower;                   //功率
    QVector<float> PowerNoiseValue;             //噪声功率
    QVector<int16_t> finalSpectrum;             //平均幅度
    QVector<float> rmsSumRaw;                   //每列和

    float hannwindowavg;                        //窗均值
    float NWindow;                              //归一化分母

    int16_t Positioning;                        //位置
    int16_t recoFrequency;                      //识别频率
    int16_t successfulAttempts;                 //满足需求的次数
    int16_t sendFren;                           //防止幅值发送太快
    uint16_t requirSuccessAttempts;             //要求的成功的次数
    std::shared_ptr<SharedBuffer> m_shared;     //buffer状态指针
    
    std::vector<float> spectrum;                //频谱
    std::vector<uint16_t> peakFre;              //每个位置的峰值频率

    void calculatePower(void);
    void PositioningVibration(void);
    void preProcessRawData(QVector<QVector<int16_t>>* bufferPtr,const int rows, const int cols);

    int16_t calculatefinalSpectrum(const std::vector<float>& curPositionPower);
    float calculateBandPeakPower(const std::vector<float>& curPositionPower, uint64_t position);
    
    //使用FFTW
    float * inputScratch;
    fftwf_complex* fftOutput = nullptr;
    fftwf_plan plan = nullptr;
};

#endif // SPECTRUMPROCESSOR_H
