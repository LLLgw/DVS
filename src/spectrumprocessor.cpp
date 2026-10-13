#include "spectrumprocessor.h"
#include "GlobalVars.h"
#include "servercommunicator.h"
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QFile>
#include <QDebug>
#include <QVector>
#include <QLineSeries>
#include <QThreadPool>
#include <QThread>
#include <QThreadStorage>
#include <QThreadStorageData>
#include <complex>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include "message.h"
#include <QtConcurrent>


SpectrumProcessor::SpectrumProcessor(std::shared_ptr<SharedBuffer> shared,QObject *parent)
    : QObject{parent},m_shared(std::move(shared))
{
    /*hanming窗*/
    hammingWindow.resize(GlobalVars::signalSamplePoints);
    for (int i = 0; i < GlobalVars::signalSamplePoints; ++i)
    {
        // 使用 std::cos 配合 float 参数
        hammingWindow[i] = 0.54f - 0.46f * std::cos(2.0f * (float)M_PI * i / (GlobalVars::signalSamplePoints - 1));
    }
    /*hann窗*/
    hannWindow.resize(GlobalVars::signalSamplePoints);
    for (int i = 0; i < GlobalVars::signalSamplePoints; ++i)
    {
        hannWindow[i] = 0.5f - 0.5f * std::cos(2.0f * (float)M_PI * i / (GlobalVars::signalSamplePoints - 1));
    }

    //归一化准备
    hannwindowavg = std::accumulate(hannWindow.begin(), hannWindow.end(), 0.0f) / GlobalVars::signalSamplePoints;
    NWindow = (GlobalVars::signalSamplePoints * hannwindowavg) * (GlobalVars::signalSamplePoints * hannwindowavg);

    //precomputeTwiddleFactors(GlobalVars::signalSamplePoints);

    //数据个数，最后两个数据最为R2C的额外空间
    //sampleCount = (GlobalVars::signalSamplePoints/2 +1) * 2;
    sampleCount =  GlobalVars::signalSamplePoints;
    bufferCount = sampleCount * GlobalVars::lineSamplePoints;
    bufferSize = bufferCount * sizeof(float);
  

    //初始化噪声数组
    PowerNoiseValue.resize(GlobalVars::lineSamplePoints); 
    std::fill(PowerNoiseValue.begin(),PowerNoiseValue.end(),0.0f);

    //初始化最大峰值数组
    peakPower.resize(GlobalVars::lineSamplePoints);
    std::fill(peakPower.begin(),peakPower.end(),0.0f);

    //峰值频率初始化
    peakFre.resize(GlobalVars::lineSamplePoints);
    std::fill(peakFre.begin(),peakFre.end(),0);


    //频谱数据初始化
    spectrum.resize(bufferCount);
    std::fill(spectrum.begin(),spectrum.end(),0.0f);

    //平均幅度初始化
    finalSpectrum.resize(GlobalVars::lineSamplePoints);
    std::fill(finalSpectrum.begin(),finalSpectrum.end(),0);

    //原始数据总和
    rmsSumRaw.resize(GlobalVars::lineSamplePoints);
    std::fill(rmsSumRaw.begin(),rmsSumRaw.end(),0);

    
    sendFren = 2;               //控制发送频率
    recoFrequency = -1;         //识别频率
    Positioning = -1;           //识别位置
    successfulAttempts = 0;     //成功识别次数
    requirSuccessAttempts = 4;  //满足识别次数


    //复数输出
    const std::size_t ouputCount = (sampleCount / 2 + 1) * GlobalVars::lineSamplePoints;
    fftOutput = static_cast<fftwf_complex*>(fftwf_malloc( ouputCount * sizeof(fftwf_complex)));
    inputScratch = static_cast<float*>(fftwf_malloc( bufferCount * sizeof(float)));

    const int * inembed = nullptr;
    const int size = sampleCount;
    const int istride = 1;
    const int ostride = 1;
    const int idist = sampleCount;
    const int odist = sampleCount / 2 + 1;
    const int* onembed = nullptr;


    plan = fftwf_plan_many_dft_r2c(
        1,
        &size,
        GlobalVars::lineSamplePoints,
        inputScratch,
        inembed,
        istride,
        idist,
        fftOutput,
        onembed,
        ostride,
        odist,
        FFTW_ESTIMATE
    );

    if(plan == nullptr){
        throw std::runtime_error("fftwf_plan_many_dft_r2c failed");
    }

}

SpectrumProcessor::~SpectrumProcessor()
{
    qDebug() << "SpectrumProcessor destroyed";
}

/*数据预处理：去直流、加窗、rms预处理、转置*/
void SpectrumProcessor::preProcessRawData(QVector<QVector<int16_t>>* bufferPtr,const int rows, const int cols)
{

    std::vector<const int16_t*> srcRows(cols);      //每行的首地址
    for(int k = 0; k < cols; k++){
        srcRows[k] = (*bufferPtr)[k].constData();
    }

    QVector<int> rowIndices(rows);

    for(int i = 0; i < rows; i++){
        rowIndices[i] = i;
    }

    float * batchPtr = inputScratch;
    const float * windowPtr = hannWindow.constData();

    //使用多线程处理：加窗、去直流、转置
    QThreadPool::globalInstance()->setMaxThreadCount(4);
    
    QtConcurrent::blockingMap(rowIndices,[&](int& i){
            float * dst = batchPtr + static_cast<size_t>(i) * static_cast<size_t>(sampleCount);
            int64_t sum = 0;

            for(int j = 0; j < cols; j++){
                const int16_t value = srcRows[j][i];
                dst[j] = static_cast<float>(value);
                sum += value;
            }

            const float mean = static_cast<float>(sum) /static_cast<float>(cols);
            for(int j = 0; j < cols; j++){
                dst[j] = dst[j] - mean;
                //rmsSumRaw[i] = dst[j] * dst[j]; 这里会导致处理速度变慢
                dst[j] =  dst[j] * windowPtr[j];
            }
        }
    );

    //emit rmsRawDataReady(rmsSumRaw); // 发送至rms处理
}

/*处理频谱数据(测试)*/
void SpectrumProcessor::processSpectrumData(QVector<QVector<int16_t>>* bufferPtr,int bufferIndex)
{
    
    // 1. 安全检查：如果指针为空则直接返回
    if (!bufferPtr || bufferPtr->isEmpty()) return;

    // 2. 发射信号，表示处理开始
    emit processorBusy(true);
    QElapsedTimer timer;
    timer.start();

    // 3. 修改访问方式：使用 (*bufferPtr)[0] 和 bufferPtr->size()
    const int rows = std::min<int>((*bufferPtr)[0].size(), GlobalVars::lineSamplePoints);
    const int cols = std::min<int>(bufferPtr->size(), GlobalVars::signalSamplePoints);

    // 边界检查
    if (rows <= 0 || cols <= 0) {
        emit processorBusy(false);
        return;
    }

    //数据预处理：去直流、加窗、rms预处理、转置
    preProcessRawData(bufferPtr,rows,cols);

    {
        std::lock_guard<std::mutex> lock(m_shared->mutex);
        m_shared -> state[bufferIndex] = bufferState::Free;//buffer空闲
    }

    //计算FFT
    fftwf_execute(plan);

    //使用CPU计算功率及最大值
    calculatePower();

    //两次发送一次平均幅值
    if(m_shared -> sendFinalSpectrumState[0] == bufferState::Free ){
        m_shared -> sendFinalSpectrumState[0] = bufferState::Processing;
        emit specDataProcessed(finalSpectrum);
    }

    //定位
    PositioningVibration();

    qDebug() << "SpectrumProcessor: processed" << rows << "points in"
    << timer.elapsed() << "ms with" ;

    emit processorBusy(false);
}

//CPU计算功率(测试)
void SpectrumProcessor::calculatePower(void){

    uint16_t bins = GlobalVars::signalSamplePoints/2 + 1;
    uint16_t isNyquist =  bins - 1;      //奈奎斯特
    std::vector<std::vector<float>> power(GlobalVars::lineSamplePoints,std::vector<float>(bins,0.0f));

    //开启多线程
    QVector<int> rowIndices( GlobalVars::lineSamplePoints);
    for(int i = 0; i <  GlobalVars::lineSamplePoints; i++){
        rowIndices[i] = i;
    }
    QThreadPool::globalInstance()->setMaxThreadCount(4);

    QtConcurrent::blockingMap(rowIndices,[&](int& position){
            size_t index = static_cast<size_t>(position) * static_cast<size_t>(bins);
            for(uint bin = 0 ; bin < bins; bin ++){
    
                uint64_t freIndex = index + static_cast<size_t>(bin); 
                const float real = fftOutput[freIndex][0];
                const float imag = fftOutput[freIndex][1];
                float value = (real * real + imag * imag) / NWindow ;   //计算功率、归一化
                
                if(bin != 0 && bin != isNyquist) value = value * 2.0f;  //单边普补偿
                power[position][bin] = value;
            }
            peakPower[position] = calculateBandPeakPower(power[position], position); //寻峰、计算噪声底
            finalSpectrum[position] =  calculatefinalSpectrum(power[position]);      //计算振幅
    });

}

// CPU计算发送频谱(有频率截止测试)
int16_t SpectrumProcessor::calculatefinalSpectrum(const std::vector<float>& curPositionPower)
{
    const int realSpectrumSize = static_cast<int>(curPositionPower.size());
    const int complexMaxIndex = realSpectrumSize - 2;  // 不算奈奎斯特

    // GlobalVars的值已经是基于复数FFT的0到N/2-1范围
    const int start = std::max(0, GlobalVars::minFrequencyPoint);
    const int end = std::min(GlobalVars::maxFrequencyPoint, complexMaxIndex);

    if (end <= start) return 0;

    float energy = 0.0f;
    const float * freqPtr = curPositionPower.data();

    for (int k = start; k <= end; k++) {
        energy += std::sqrt(freqPtr[k]);
    }

    // 方法1：除以N得到真实幅度
    float avgAmplitude = energy / (end - start + 1);

    // 转换为电压（1LSB = 0.5mV）
    const float LSB_TO_MV = 0.5f;
    float voltageMv = avgAmplitude * LSB_TO_MV;

    float powerDbFloat = 20.0f * std::log10(voltageMv / 1.0f + 1e-10f);
    if(powerDbFloat < 5.0f) powerDbFloat = 1.0f; // 最小值限制

    int16_t powerDb = static_cast<int16_t>(std::round(powerDbFloat * 100.0f));
    //int16_t powerDb = static_cast<int16_t>(std::max(5.0f , 20.0f * std::log10(voltageMv / 1.0f + 1e-10f)) * 100);
    //powerDb = powerDb < 0.0f ? 0.0f : powerDb > 32767 ? 32767 : powerDb; 
    
    return powerDb;
}

/*CPU计算峰值(测试)*/
/*过滤掉左右的旁瓣，取左右数据作为噪声*/
float SpectrumProcessor::calculateBandPeakPower(const std::vector<float>& curPositionPower, uint64_t position) {

    //const int arvgeFrequency = 200;
    const int realSpectrumSize = static_cast<int>(curPositionPower.size());
    const int freqMaxIndex = realSpectrumSize - 2;   // 不算奈奎斯特

    // 计算频率对应的索引范围，确保不越界
    const int start =  std::max(0,static_cast<int>(GlobalVars::findFreStart/GlobalVars::deltaFrequency)); 
    const int end = std::min(freqMaxIndex, static_cast<int>(GlobalVars::findFreEnd / GlobalVars::deltaFrequency));

    if (end < start) { return 0.0f; }

    int peakPowerIndex = 0;             //峰值对应的频率下标    
    float peakPower = 0.0f, power = 0.0f;
    int N = (realSpectrumSize - 1) * 2; //真实长度

    //找最高峰值
    for(int k = start; k <= end; k++){
        if(curPositionPower[k] > peakPower){
            peakPowerIndex = k;
            peakPower = curPositionPower[k];
        }
    }

    //峰值频率
    peakFre[position]= peakPowerIndex * GlobalVars::deltaFrequency;

    //单边旁瓣与噪声的大小
    uint16_t freGap = 3;
    const uint16_t nosieFresize = freGap *2;
    std::vector<float> powerValue(nosieFresize);  //噪声幅度

    const int16_t startNoiseLeft = std::max(0,peakPowerIndex - freGap*2); //左侧噪声起始，过滤掉旁瓣
    const int16_t endNoiseLeft = startNoiseLeft + freGap;                 //左侧噪声结束
    const int16_t startNoiseRight = std::min(freqMaxIndex,peakPowerIndex + freGap);//右侧噪声起始，过滤掉旁瓣
    const int16_t endNoiseRight = std::min(freqMaxIndex,startNoiseRight + freGap);   //右侧噪声结束
    int16_t NosiePowerIndex = 0; //噪声功率值下标
    
    //左侧
    for(int k = startNoiseLeft; k < endNoiseLeft && NosiePowerIndex < nosieFresize; k++,NosiePowerIndex++){
        powerValue[NosiePowerIndex] = curPositionPower[k];
    }

    //右侧
    for(int k = startNoiseRight; k < endNoiseRight && NosiePowerIndex < nosieFresize; k++,NosiePowerIndex++){
        powerValue[NosiePowerIndex] = curPositionPower[k];
    }
    //噪音
    PowerNoiseValue[position] = std::accumulate(powerValue.begin(),powerValue.end(),0.0) / nosieFresize;

    return peakPower; 
}

/*定位位置(测试)*/
void SpectrumProcessor::PositioningVibration(void)
{
    const int16_t min_peak_db = 30; //30
    const int16_t min_snr_db = 20;  //20
    int16_t maxFreqIndex = 0;
    uint16_t recognitionFre = 0;
    std::vector<float> snr(SpectrumProcessor::PowerNoiseValue.size());//信噪比

    //找最大点
    for(int i = 0; i < SpectrumProcessor::peakPower.size(); i++) {
         snr[i] = 10 * std::log10(SpectrumProcessor::peakPower[i] / (SpectrumProcessor::PowerNoiseValue[i] + 1e-10f));//信噪比
         SpectrumProcessor::peakPower[i] = 10 * std::log10(SpectrumProcessor::peakPower[i] + 1e-10f); //计算db
        if (SpectrumProcessor::peakPower[i] > SpectrumProcessor::peakPower[maxFreqIndex]) {
            maxFreqIndex = i;
        }
    }

    // 输出符合的频率
    if ( min_peak_db < SpectrumProcessor::peakPower[maxFreqIndex] && snr[maxFreqIndex] >  min_snr_db) {

         //因为频率分辨率为8hz，不能精确到100hz，需要校准
         if( peakFre[maxFreqIndex] >= 96 && peakFre[maxFreqIndex] <= 104){
            recognitionFre = 100;
         }
         else if( peakFre[maxFreqIndex] >= 192 && peakFre[maxFreqIndex] <= 208)
         {
            recognitionFre = 200;
         }
         else if( peakFre[maxFreqIndex] >= 392 && peakFre[maxFreqIndex] <= 408 )
         {
            recognitionFre = 400;
         }
         else if( peakFre[maxFreqIndex] >= 582 && peakFre[maxFreqIndex] <= 608 )
         {
            recognitionFre = 600;
         }
         else if( peakFre[maxFreqIndex] >= 782 && peakFre[maxFreqIndex] <= 808 )
         {
             recognitionFre = 800;
         }
         else if( peakFre[maxFreqIndex] >= 900 ){
            recognitionFre = 1000;
         }
         else
         {
            //recognitionFre = peakFre[maxFreqIndex];
            return; //不符合频率要求，跳过
         }

         //调试  <<peakFre[maxFreqIndex]
         qDebug() << "############################### " << recognitionFre << " hz local : " <<  maxFreqIndex 
         << "m   Spectrum: "  << SpectrumProcessor::peakPower[maxFreqIndex]
         << "snr:  "<<snr[maxFreqIndex];

        //连续两次峰值在相邻位置，认为是同一个峰值
        if (Positioning == -1) { Positioning = maxFreqIndex;} 
        else {

            if ( (maxFreqIndex <= Positioning + 1 && maxFreqIndex >= Positioning - 1) && ( recoFrequency - 8 <= recognitionFre &&  recognitionFre <= recoFrequency + 8)) {
                successfulAttempts ++;

                if(successfulAttempts >= requirSuccessAttempts){
                    emit Fkp_local(recognitionFre,maxFreqIndex);
                    qDebug() << "**********************************************************************************" << recognitionFre <<" hz local : " << maxFreqIndex << "m   Spectrum: " << SpectrumProcessor::peakPower[maxFreqIndex];
                    successfulAttempts = 1;
                }
            }
            else
            {
                successfulAttempts = 1;
            }

             Positioning = maxFreqIndex;
             recoFrequency = recognitionFre;

        }
    }
}

// 预计算旋转因子（用于复数FFT部分）
void SpectrumProcessor::precomputeTwiddleFactors(int n) {
    if (twiddleFactors.size() >= static_cast<size_t>(n/2)) {
        return; // 已经预计算过足够大的表
    }

    twiddleFactors.clear();
    const float theta = -2.0f * (float)M_PI / n;
    for (int i = 0; i < n/2; ++i) {
        twiddleFactors.push_back(std::complex<float>(
            std::cos(i * theta),
            std::sin(i * theta)
            ));
    }
}

