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
#include <CL/cl.h>
#include "vkFFT.h"

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

public slots:
    void processSpectrumData(QVector<QVector<int16_t>>* bufferPtr,int bufferIndex);//改

private:
    QVector<float> hammingWindow; // 修改为 float
    QVector<float> hannWindow;    // 修改为 float
    //QVector<std::complex<float>> twiddleFactors;
    std::vector<std::complex<float>> twiddleFactors; // 修改为 float
    //int cachedFFTSize = 0;

    

    //void complexFFT(QVector<std::complex<float>>& data);
    void complexFFT(std::vector<std::complex<float>>& data); // 修改为 float
    //void realFFT(const QVector<float>& realInput, QVector<std::complex<float>>& complexOutput);
    void realFFT(const std::vector<float>& realInput,
                 std::vector<std::complex<float>>& complexOutput,
                 std::vector<std::complex<float>>& packed); // 修改为 float

    void precomputeTwiddleFactors(int size);
    void fft(std::vector<std::complex<float>>& x); // 修改为 float
    //std::vector<std::complex<float>> fftIterative(const std::vector<std::complex<float>>& x);
    //float calculateSpectrumPower(const QVector<std::complex<float>>& freqData);
    float calculateSpectrumPower(const std::vector<std::complex<float>>& freqData); // 修改为 float
    float calculateSpectrum(const std::vector<std::complex<float>>& freqData);     // 修改为 float
    float calculateSNR(const std::vector<std::complex<float>>& freqData);          // 修改为 float
    QVector<int16_t> convertSpectrumToInt16(const QVector<float>& spectrum);       // 修改为 float

    //改
    cl_platform_id platform = nullptr;        //平台
    cl_device_id device = nullptr;            //设备
    cl_context context = nullptr;             //设备上下文
    cl_command_queue queue = nullptr;         //命令队列
    cl_mem buffer = nullptr;                  //主数据buffer

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

    //改
    cl_program postProgram = nullptr;
    cl_kernel postKernel = nullptr;
    cl_mem finalBuffer = nullptr;
    cl_mem peakBuffer = nullptr;
    cl_mem noiseBuffer = nullptr;

    void createPostProcessProgram();
    void createPostProcessBuffers();
    void runGpuPostProcess(int rows, QVector<int16_t>& finalSpectrum);


    QVector<float> peakPower;                   //功率
    QVector<float> PowerNoiseValue;             //噪声功率
    float hannwindowavg;                        // 窗均值
    int16_t Positioning = -1;                   //位置
    uint16_t Same_positsion_count = 0;          //位置计数
    std::shared_ptr<SharedBuffer> m_shared;     //buffer状态指针
    float NWindow;                              //归一化分母
    std::vector<float> batchMajor;              //转置后
    std::vector<float> spectrum;                //频谱

    void PositioningVibration(void);
    float calculateBandPeakPower(const std::vector<float>& curPositionPower, uint64_t position);
    void calculatePower(const std::vector<float>& spectrum, QVector<int16_t>& finalSpectrum);
    int16_t calculatefinalSpectrum(const std::vector<float>& curPositionPower);
};

#endif // SPECTRUMPROCESSOR_H
