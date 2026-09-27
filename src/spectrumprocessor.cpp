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
//#include <fftw3.h>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include "message.h"
#include <QtConcurrent>




//给到内核计算
static const char* kSpectrumPostProcessSource = R"CLC(
inline float calc_power(__global const float2* row,uint bin, uint bins,float nWindow){
    
    float2 z = row[bin];

    float power = z.x * z.x + z.y * z.y;
    power = power / nWindow;

    if(bin != 0 && bin != bins -1){
        power *= 2.0f; 
    }

    return power;
}

__kernel void spectrum_postprocess(__global const float2* fft,
                                   __global short* finalOut,
                                   __global float* peakOut,
                                   __global float* noiseOut,
                                   __global short* freOut,
                                   const int rows,
                                   const int bins,
                                   const int finalStartBin,
                                   const int finalEndBin,
                                   const int peakStartBin,
                                   const int peakEndBin,
                                   const int noiseGapBins,
                                   const int noiseSideBins,
                                   const float nWindow,
                                   const float deltaFrequency,
                                   __local float* scratch, 
                                   __local int* scratchIndex)
{
    
    const uint pos = get_group_id(0);     
    const uint lid = get_local_id(0);     
    const uint localSize = get_local_size(0); 

    const int active = ((int)pos < rows);

    const int scanStart = (finalStartBin < peakStartBin) ? finalStartBin:peakStartBin;
    const int scanEnd = (finalEndBin < peakEndBin) ? peakEndBin : finalEndBin;
    
    __global const float2* row = fft + ((size_t)pos * (size_t)bins); 
    
    const int loadStart = scanStart - 2;
    const int loadCount = scanEnd - scanStart + 5;

    __local float * localPower = scratch;
    __local float * reduce = scratch + loadCount;

    for(int i = (int)lid ; i < loadCount; i += (int)localSize){
        const int bin = loadStart + i;
        float power = 0.0f;

        if(active && bin >= 0 && bin < bins){
            power = calc_power(row,bin,bins,nWindow);
        }
        
        localPower[i] = power;
    }

    barrier(CLK_LOCAL_MEM_FENCE);




    float localAmpSum = 0.0f;
    float localPeak = -1.0f;
    int localPeakIndex = peakStartBin;

    for(int bin = scanStart + (int)lid; bin <= scanEnd; bin += (int)localSize){
        const int localIndex = bin - loadStart;

        const float smoothPower = ( localPower[localIndex - 2] + 
                                    4.0f * localPower[localIndex - 1] + 
                                    6.0f * localPower[localIndex] + 
                                    4.0f * localPower[localIndex + 1] +
                                    localPower[localIndex + 2]) * 0.0625f;
        
        if(bin >= finalStartBin && bin <= finalEndBin){
            localAmpSum += native_sqrt(smoothPower);
        }

        if(bin >= peakStartBin && bin <= peakEndBin){
            if( smoothPower > localPeak ){
                localPeak = smoothPower;
                localPeakIndex = bin;
            }
        }
    }

    reduce[lid] = localAmpSum;
    barrier(CLK_LOCAL_MEM_FENCE);




    for(uint step = localSize >> 1; step > 0; step >>= 1){
        if(lid < step){ reduce[lid] += reduce[lid + step]; }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(active && lid == 0){
        float scaled = 0.0f;
        if(finalEndBin >= finalStartBin){
            const float finalBinCount = (float)(finalEndBin - finalStartBin + 1);
            const float averageAmplitude  = reduce[0] / finalBinCount;
            const float voltageMv = averageAmplitude * 0.5f;
            const float db = 20.0f * log10(voltageMv + 1.0e-10f);
            scaled = fmax(db, 1.0f) * 100.0f;
            scaled = clamp(scaled,0.0f,32767.0f);
        }
        finalOut[pos] = convert_short_rtz(scaled);
    }

    reduce[lid] = localPeak;
    scratchIndex[lid] = localPeakIndex;

    barrier(CLK_LOCAL_MEM_FENCE);

    for(uint step = localSize >> 1; step > 0; step >>= 1){
        if(lid < step){
            const float otherPeak = reduce[lid + step];
            const int otherIndex = scratchIndex[lid + step];
            const float currentPeak  = reduce[lid];
            const int currentIndex  = scratchIndex[lid];

            if(otherPeak > currentPeak){
                reduce[lid] = otherPeak;
                scratchIndex[lid] = otherIndex;
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(active && lid == 0){
        const float peakPower = reduce[0];
        const int peakIndex = scratchIndex[0];

        peakOut[pos] = peakPower;
        
        const float peakFrequency = (float)peakIndex * deltaFrequency;
        freOut[pos] = convert_short_rtz(clamp(peakFrequency,0.0f,32767.0f));
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    const int peakIndex = scratchIndex[0];

    int leftStart = peakIndex - noiseSideBins;
    int leftEnd =  leftStart + noiseGapBins;
    int rightStart = peakIndex + noiseGapBins;
    int rightEnd = rightStart + noiseGapBins;

    if(leftStart < 0)       leftStart = 0;
    if(leftEnd < 0)         leftEnd = 0;
    if(leftStart > bins)    leftStart = bins;
    if(leftEnd > bins)      leftEnd = bins;
    if(rightStart < 0)      rightStart = 0;
    if(rightEnd < 0)        rightEnd = 0;
    if(rightStart > bins)   rightStart = bins;
    if(rightEnd > bins)     rightEnd = bins;

    float localNoiseSum = 0.0f;
    int localNoiseCount = 0;

    if(active){
        for(int bin = leftStart + (int)lid; bin < leftEnd; bin += (int)localSize){
            localNoiseSum += calc_power(row,bin,bins,nWindow);
            localNoiseCount ++;
        }
        
        for(int bin = rightStart + (int)lid; bin < rightEnd; bin += (int)localSize){
            localNoiseSum += calc_power(row,bin,bins,nWindow);
            localNoiseCount ++;
        }
    }

    reduce[lid] = localNoiseSum;
    scratchIndex[lid] = localNoiseCount;

    barrier(CLK_LOCAL_MEM_FENCE);

    for(uint step = localSize >> 1; step > 0; step >>= 1){
        if(lid < step){
            reduce[lid] += reduce[lid + step];
            scratchIndex[lid] += scratchIndex[lid + step];
        }
        
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(active && lid == 0){
        if(scratchIndex[0] > 0) noiseOut[pos] = reduce[0]/(float)scratchIndex[0];
        else
        {
            noiseOut[pos] = 0.0f;
        }
    }        
}
)CLC";


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

    precomputeTwiddleFactors(GlobalVars::signalSamplePoints);

    //数据个数，最后两个数据最为R2C的额外空间
    sampleCount = (GlobalVars::signalSamplePoints/2 +1) * 2;    
    bufferCount = sampleCount * GlobalVars::lineSamplePoints;
    bufferSize = bufferCount * sizeof(float);
    app = {};
    initialized = false;

    //初始化顺序: OpenCl -> buffer -> VkFFT plan
    CreateOpencl();
    CreateBuffer();
    initializeVkFFT_();

    //初始化需要提交的buffer和参数
    createPostProcessBuffers();
    createPostProcessProgram();

    //初始化噪声数组
    PowerNoiseValue.resize(GlobalVars::lineSamplePoints); 
    std::fill(PowerNoiseValue.begin(),PowerNoiseValue.end(),0.0f);

    //初始化最大峰值数组
    peakPower.resize(GlobalVars::lineSamplePoints);
    std::fill(peakPower.begin(),peakPower.end(),0.0f);

    //峰值频率初始化
    peakFre.resize(GlobalVars::lineSamplePoints);
    std::fill(peakFre.begin(),peakFre.end(),0);

    //初始化转置后的数组
    batchMajor.resize(bufferCount);
    std::fill(batchMajor.begin(),batchMajor.end(),0.0f);
    
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
    successfulAttempts = 0;     
    requirSuccessAttempts = 4;  //满足出现次数

}

SpectrumProcessor::~SpectrumProcessor()
{
    //stopProcessing();
    // 先释放 VkFFT，再释放 OpenCL buffer、队列和 context。
    if (initialized) { deleteVkFFT(&app);}
    if (buffer) {clReleaseMemObject(buffer);}
    if (queue) {clReleaseCommandQueue(queue);}
    if (postKernel) {
        clReleaseKernel(postKernel);
        postKernel = nullptr;
    }

    if (postProgram) {
        clReleaseProgram(postProgram);
        postProgram = nullptr;
    }

    if (finalBuffer) {
        clReleaseMemObject(finalBuffer);
        finalBuffer = nullptr;
    }

    if (peakBuffer) {
        clReleaseMemObject(peakBuffer);
        peakBuffer = nullptr;
    }

    if (noiseBuffer) {
        clReleaseMemObject(noiseBuffer);
        noiseBuffer = nullptr;
    }

    if (context) {clReleaseContext(context);}

    qDebug() << "SpectrumProcessor destroyed";
}

void SpectrumProcessor::checkVkFFT(VkFFTResult code, const char * operation){
    if(code != VKFFT_SUCCESS){
        throw std::runtime_error(std::string(operation) + "failed, VkFFT error = " + std::to_string(static_cast<int>(code)));
    }
}

void SpectrumProcessor::checkCL(cl_int code, const char * operation){
    if(code != CL_SUCCESS){
        throw std::runtime_error(std::string(operation) + "failed, OopenCL error = " + std::to_string(code)); 
    }
}

//创建opencl
void SpectrumProcessor::CreateOpencl(){
    cl_uint platformCount = 0;

    //查询Opencl平台数量
    checkCL(clGetPlatformIDs(0,nullptr,&platformCount),"clGetPlatformIDs(count)");

    if(platformCount == 0){
        throw std::runtime_error("No OpenCL platform found");
    }

    //获取Opencl平台
    std::vector<cl_platform_id> platforms(platformCount);
    checkCL(clGetPlatformIDs(platformCount, platforms.data(), nullptr),"clGetPlatformIDs");

    //选择第一个GPU
    for(cl_platform_id candidatePlatform : platforms){
        cl_uint deviceCount = 0;
        const cl_int result = clGetDeviceIDs(candidatePlatform,CL_DEVICE_TYPE_GPU,0,nullptr,&deviceCount);
        if(result == CL_DEVICE_NOT_FOUND || deviceCount == 0) continue;
        checkCL(result,"clGetDeviceIDs(GPU,count)");

        std::vector<cl_device_id> devices(deviceCount);
        checkCL(clGetDeviceIDs(candidatePlatform,CL_DEVICE_TYPE_GPU,deviceCount,devices.data(),nullptr),"clGetDeviceIDs(GPU)");
        platform = candidatePlatform;
        device = devices.front();
        break;
    }

    //如果没有找到GPU,则选择第一个可用设备
    if(!device){
        for(cl_platform_id candidatePlatform : platforms){
            cl_uint deviceCount = 0;
            const cl_int result = clGetDeviceIDs(candidatePlatform,CL_DEVICE_TYPE_ALL,0,nullptr,&deviceCount);
            if(result == CL_DEVICE_NOT_FOUND || deviceCount == 0) continue;
            checkCL(result,"clGetDeviceIDs(ALL,count)");

            std::vector<cl_device_id> devices(deviceCount);
            checkCL(clGetDeviceIDs(candidatePlatform,CL_DEVICE_TYPE_ALL,deviceCount,devices.data(),nullptr),"clGetDeviceIDs(ALL)");
            platform = candidatePlatform;
            device = devices.front();
            break;
        }
    }

    if(!device) throw std::runtime_error("No OpneCL device found");

    cl_int result = CL_SUCCESS;

    //创建OpenCL context
    context = clCreateContext(nullptr,1,&device,nullptr,nullptr,&result);
    checkCL(result, "clCreateContext");

    //创建命令队列，VKFFT会把kernel加入这个队列
    queue = clCreateCommandQueue(context,device,0,&result);
    checkCL(result, "clCreateCommandQueue");
}

//创建buffer
void SpectrumProcessor::CreateBuffer(){
    cl_int result = CL_SUCCESS;

    //主数据buffer 由应用程序分配和释放,VKFFT不负责创建它
    buffer = clCreateBuffer(context,CL_MEM_READ_WRITE,bufferSize,nullptr,&result);
    checkCL(result, "clCreateBuffer");
}

//初始化VKFFT
void SpectrumProcessor::initializeVkFFT_(){
    //配置一个一维、批量、单精度R2C FFT
    VkFFTConfiguration configuration = {};
    configuration.FFTdim = 1;
    configuration.size[0] = GlobalVars::signalSamplePoints;
    configuration.numberBatches = GlobalVars::lineSamplePoints;
    configuration.performR2C = 1;

    //将OpneCl device 和 context 传给 VKFFT
    configuration.device = &device;
    configuration.context = &context;

    //指定主计算buffer大小
    configuration.buffer = &buffer;
    configuration.bufferSize = &bufferSize;

    //初始化FFT plan, 并生成/编译 FFT kernel
    checkVkFFT(initializeVkFFT(&app,configuration),"initializeVkFFT");
    initialized = true;

}

//创建GPU输出Buffer
void SpectrumProcessor::createPostProcessBuffers(){
    cl_int result = CL_SUCCESS;

    //创建finalBuffer
    const size_t rows = static_cast<size_t>(GlobalVars::lineSamplePoints);
    finalBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rows * sizeof(cl_short), nullptr, &result);
    checkCL(result, "create finalBuffe");
    
    //创建peakBuffer
    peakBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rows * sizeof(cl_float), nullptr, &result);
    checkCL(result, "create peakBuffer");

    //创建noiseBuffer
    noiseBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rows * sizeof(cl_float), nullptr, &result);
    checkCL(result, "create noiseBuffer");

    //创建peakFreBuffer
    peakFreBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rows * sizeof(cl_short), nullptr, &result);
    checkCL(result,"create peakFreBuffer");
}

//编译和创建kernel
void SpectrumProcessor::createPostProcessProgram(){
    cl_int result = CL_SUCCESS;

    const size_t sourceLength = strlen(kSpectrumPostProcessSource);

    //创建源程序
    postProgram = clCreateProgramWithSource(context, 1, &kSpectrumPostProcessSource, &sourceLength, &result);
    checkCL(result,"clCreateProgramWithSource");

    //编译程序
    result = clBuildProgram(postProgram, 1, &device, nullptr, nullptr, nullptr);

    if(result != CL_SUCCESS){
        size_t logSize = 0;

        //获取程序信息
        clGetProgramBuildInfo(postProgram,device,CL_PROGRAM_BUILD_LOG,0,nullptr,&logSize);

        std::string buildLog;
        if(logSize > 0){
            buildLog.resize(logSize);
            clGetProgramBuildInfo(postProgram,device,CL_PROGRAM_BUILD_LOG,logSize,&buildLog[0],nullptr);
        }

        //异常
        throw std::runtime_error("OpenCL postprocess build failed:" + buildLog);

    }

    postKernel = clCreateKernel(postProgram,"spectrum_postprocess",&result);
    checkCL(result, "clCreateKernel");
}

//添加GPU后处理函数
void SpectrumProcessor::runGpuPostProcess(int rows){
    const int bins =  GlobalVars::signalSamplePoints / 2 + 1; //频点数
    const int maxBin = bins - 2;//最大频率下标
    const int finalStartBin = std::max(0,GlobalVars::minFrequencyPoint);    //低频限制
    const int finalEndBin = std::min(GlobalVars::maxFrequencyPoint, maxBin);//高频限制
    const int peakStartBin = std::max(0,static_cast<int>(GlobalVars::findFreStart/GlobalVars::deltaFrequency));   //频率起始
    const int peakEndBin = std::min(maxBin, static_cast<int>(GlobalVars::findFreEnd / GlobalVars::deltaFrequency));//频率结束
    const int sidelobeAndnoiseGapBins = 4; //旁瓣蛋单边范围
    const int noiseSideBins = sidelobeAndnoiseGapBins * 2;//噪声边缘
  
    
    const cl_int rowsArg = rows;
    const cl_int binsArg = bins;
    const cl_int finalStartArg = finalStartBin;
    const cl_int finalEndArg = finalEndBin;
    const cl_int peakStartArg = peakStartBin;
    const cl_int peakEndArg = peakEndBin;
    const cl_int sidelobeAndnoiseGapArg = sidelobeAndnoiseGapBins;
    const cl_int noiseSideArg = noiseSideBins;
    const cl_float nWindowArg = NWindow;
    const cl_float deltaFrequencyArg = GlobalVars::deltaFrequency;

    //传参
    cl_uint arg = 0;
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_mem),&buffer),"set fft buffer");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_mem),&finalBuffer),"set final buffer");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_mem),&peakBuffer),"set peak buffer");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_mem),&noiseBuffer),"set noise buffer");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_mem),&peakFreBuffer),"set peakFre buffer");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&rowsArg),"set rows");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&binsArg),"set bins");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&finalStartArg),"set final start");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&finalEndArg),"set final end");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&peakStartArg),"set peak start");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&peakEndArg),"set peak end");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&sidelobeAndnoiseGapArg),"set noise gap");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_int),&noiseSideArg),"set noise side");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_float),&nWindowArg),"set nWindow");
    checkCL(clSetKernelArg(postKernel,arg++,sizeof(cl_float),&deltaFrequencyArg),"set deltaFrequency");
    
    const size_t localSize = 256;
    const int scanStart = std::min(finalStartBin, peakStartBin);
    const int scanEnd = std::max(finalEndBin, peakEndBin);
    const size_t loadCount =static_cast<size_t>(scanEnd - scanStart + 5);
    const size_t localFloatCount = loadCount + localSize;

    checkCL(clSetKernelArg(postKernel,arg++,localFloatCount * sizeof(cl_float),nullptr),"set local float memory");
    checkCL(clSetKernelArg(postKernel,arg++,localSize * sizeof(cl_int),nullptr),"set local index memory");

    const size_t globalSize = static_cast<size_t>(rows) * localSize;
    checkCL(clEnqueueNDRangeKernel(queue,postKernel,1,nullptr,&globalSize,&localSize,0,nullptr,nullptr),"enqueue spectrum postprocess");
    
    //读数据
    checkCL(clEnqueueReadBuffer(queue,finalBuffer,CL_FALSE,0,static_cast<size_t>(rows) * sizeof(cl_short),finalSpectrum.data(),0,nullptr,nullptr),"read final spectrum");
    checkCL(clEnqueueReadBuffer(queue,peakBuffer,CL_FALSE,0,static_cast<size_t>(rows) * sizeof(cl_float),peakPower.data(),0,nullptr,nullptr),"read peak power");
    checkCL(clEnqueueReadBuffer(queue,noiseBuffer,CL_FALSE,0,static_cast<size_t>(rows) * sizeof(cl_float),PowerNoiseValue.data(),0,nullptr,nullptr),"read noise power");
    checkCL(clEnqueueReadBuffer(queue,peakFreBuffer,CL_FALSE,0,static_cast<size_t>(rows) * sizeof(cl_short), peakFre.data(), 0, nullptr,nullptr),"read peak frequence");
    checkCL(clFinish(queue),"finish spectrum postprocess");
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

    //使用多线程处理：加窗、去直流、转置
    QThreadPool::globalInstance()->setMaxThreadCount(4);
    float * batchPtr = batchMajor.data();
    const float * windowPtr = hannWindow.constData();

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
                //dst[j] = (dst[j] - mean) * windowPtr[j];
                dst[j] = dst[j] - mean;
                rmsSumRaw[i] = dst[j] * dst[j];
                dst[j] =  dst[j] * windowPtr[j];
            }
        }
    );

    //emit rmsRawDataReady(rmsSumRaw);
    
    
    
    
    /*
    //使用openMP多线程
    //#pragma omp parallel for num_threads(4) schedule(static)
    for(int i = 0; i < rows; i++){
        float * dst = batchMajor.data() + static_cast<size_t>(i) * sampleCount;
        int64_t sum = 0;
        for(int j = 0; j < cols; j++){
            const int16_t value = srcRows[j][i];
            dst[j] = static_cast<float>(value);
            sum += value;
        }

        const float mean = static_cast<float>(sum) / cols;
        for(int j = 0; j < cols; j++){
            dst[j] = (dst[j] - mean) * hannWindow[j];
        }
    }*/
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

    //初始化噪声数组
    if(SpectrumProcessor::PowerNoiseValue.empty())
    {
        SpectrumProcessor::PowerNoiseValue.resize(rows);
        std::fill(SpectrumProcessor::PowerNoiseValue.begin(), SpectrumProcessor::PowerNoiseValue.end(), 0);
    }
    
    //数据预处理：去直流、加窗、rms预处理、转置
    preProcessRawData(bufferPtr,rows,cols);
    
    //将全部输入数据上传到OpenCL device buffer
    checkCL(clEnqueueWriteBuffer(queue,buffer,CL_FALSE,0,bufferSize, batchMajor.data(),0,nullptr,nullptr),"clEnqueueWriteBuffer");

    VkFFTLaunchParams launchParams = {};
    // OpenCL 后端通过 commandQueue 指定 FFT 所使用的命令队列
    launchParams.commandQueue = &queue;

    //-1表示正向FFT
    checkVkFFT(VkFFTAppend(&app, -1 , &launchParams),"VkFFTAppend(batch R2C)");

    //读回全部频谱数据 
    checkCL(clEnqueueReadBuffer(queue,buffer,CL_FALSE,0,bufferSize,spectrum.data(),0,nullptr,nullptr),"clEnqueueReadBuffer");

    //等待全部FFT kernel 执行完成
    checkCL(clFinish(queue),"clFinish after batch FFT");

    //使用CPU计算功率及最大值
    //calculatePower();
    
    //使用GPU计算
    runGpuPostProcess(rows);

    m_shared -> state[bufferIndex] = bufferState::Free;//buffer空闲

    /*
    // 使用快速转换 - 优化：展开循环（保留）
    int16_t* finalPtr = finalSpectrum.data();
    
    
    int position = 0;
    uint32_t invaildCount = 0;
    for ( ; position + 3 < rows; position += 4) {
        // 展开4次循环，提高缓存利用率
        float value0 = finalPtr[position] * 100.0f;
        float value1 = finalPtr[position+1] * 100.0f;
        float value2 = finalPtr[position+2] * 100.0f;
        float value3 = finalPtr[position+3] * 100.0f;

        finalPtr[position] = static_cast<int16_t>(value0 < 0 ? 0 : (value0 > 32767 ? 32767 : value0));
        finalPtr[position+1] = static_cast<int16_t>(value1 < 0 ? 0 : (value1 > 32767 ? 32767 : value1));
        finalPtr[position+2] = static_cast<int16_t>(value2 < 0 ? 0 : (value2 > 32767 ? 32767 : value2));
        finalPtr[position+3] = static_cast<int16_t>(value3 < 0 ? 0 : (value3 > 32767 ? 32767 : value3));

    }

    // 处理剩余的部分
    for (; position < rows; position++) {
        float value = finalPtr[position] * 100.0f;
        finalPtr[position] = static_cast<int16_t>(value < 0 ? 0 : (value > 32767 ? 32767 : value));
    }*/

    //两次发送一次平均幅值
    if(m_shared -> sendFinalSpectrumState[0] == bufferState::Free && sendFren == 2){
        m_shared -> sendFinalSpectrumState[0] = bufferState::Processing;
        emit specDataProcessed(finalSpectrum);
        sendFren = 1;
    }
    else{
        sendFren++;
    }
    
    //定位
    PositioningVibration();

    qDebug() << "SpectrumProcessor: processed" << rows << "points in"
    << timer.elapsed() << "ms with" ;

    emit processorBusy(false);
}

//计算功率(测试)
void SpectrumProcessor::calculatePower(void){
    std::vector<std::vector<float>> power(GlobalVars::lineSamplePoints,std::vector<float>(GlobalVars::signalSamplePoints/2 + 1,0.0f)); //功率
    std::int32_t isNyquist = sampleCount -2;
    
    //两个线程同时计算
    #pragma omp parallel for num_threads(2) schedule(static)
    for(int64_t position = 0; position < GlobalVars::lineSamplePoints; position++){
        int64_t positionIndex = position * sampleCount;
        for(int16_t j = 0,freq = 0; j < sampleCount; j+=2,freq++){
            uint32_t freIndex = positionIndex + j;
            
            power[position][freq] = spectrum[freIndex] * spectrum[freIndex] + spectrum[freIndex + 1] * spectrum[freIndex + 1];//计算功率
            power[position][freq] =  power[position][freq] / NWindow;//归一化

            if( j != 0 && j != isNyquist){
                power[position][freq] =  power[position][freq] * 4; //单边普补偿
            }
        }
         peakPower[position] = calculateBandPeakPower(power[position],position); //找最大峰值
         finalSpectrum[position] = calculatefinalSpectrum(power[position]);     
    }

}

// CPU计算发送频谱(有频率截止测试)
int16_t SpectrumProcessor::calculatefinalSpectrum(const std::vector<float>& curPositionPower)
{
    const int realSpectrumSize = static_cast<int>(curPositionPower.size());
    const int complexMaxIndex = realSpectrumSize - 2;  // N/2 - 1

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

    // 使用合理参考（如1mV）
    int16_t powerDb = static_cast<int16_t>(20.0f * std::log10(voltageMv / 1.0f + 1e-10f));

    return std::max(powerDb,  static_cast<int16_t>(0));
}

/*CPU计算峰值(测试)*/
/*过滤掉左右的旁瓣，取左右数据作为噪声*/
float SpectrumProcessor::calculateBandPeakPower(const std::vector<float>& curPositionPower, uint64_t position) {

    const int realSpectrumSize = static_cast<int>(curPositionPower.size());
    const int freqMaxIndex = realSpectrumSize - 2;   // N/2 - 1

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
    const int16_t min_snr_db = 20; //20
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
         if( peakFre[maxFreqIndex] == 96 || peakFre[maxFreqIndex] == 104){
            recognitionFre = 100;
         }
         else if( peakFre[maxFreqIndex] == 208 ||  peakFre[maxFreqIndex] == 192)
         {
            recognitionFre = 200;
         }
         else if( peakFre[maxFreqIndex] == 408 || peakFre[maxFreqIndex] == 382 )
         {
            recognitionFre = 400;
         }
         else if( peakFre[maxFreqIndex] == 608 || peakFre[maxFreqIndex] == 582 )
         {
            recognitionFre = 600;
         }
         else if( peakFre[maxFreqIndex] == 808 || peakFre[maxFreqIndex] == 782 )
         {
             recognitionFre = 800;
         }
         else if( peakFre[maxFreqIndex] >= 900 ){
            recognitionFre = 1000;
         }
         else
         {
            recognitionFre = peakFre[maxFreqIndex];
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

