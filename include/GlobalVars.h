#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <mutex>
#include <cstdint>

#define SYSTEM_IDLE         0
#define SYSTEM_TEST         1
#define SERVER_START_TEST   2
#define SERVER_STOP_TEST    3

class GlobalVars {    
public:
    static std::mutex g_mutex;
    static int signalSampleFrequency;
    static int signalSamplePoints;
    static int signalAverageNumber;
    static int signalSoftwareAverageNumber;
    static int lineSamplePoints;
    static int lineSampleDelayPoints;
    static int adcSampleFrequency;
    static int kMaxSamplesPerFrame; //每帧最大采样点数
    static int kFullFrames;         //每线测试完整帧数
    static int kRemainingSamples;   //剩余采样点
    static int kTotalFramesPerCycle;//每线测试总帧数
    static int lowCutFrequency;     //低频截止频率
    static int highCutFrequency;    //高频截止频率
    static double deltaFrequency;   //频点间隔频率
    static int minFrequencyPoint;   //低频截止频率对应频点
    static int maxFrequencyPoint;   //高频截止频率对应频点
    static uint8_t systemState;
    static bool averageEnable;
    static bool differenceEnable;
    static bool rmsDataTransmitEnable;
    static bool rawDataDisplayEnable;
    static bool fpgaConnected;
    static bool serverConnected;
    static bool appLogIn;
    static bool fpgaInited;
    // 【新增】保存上报数据长度（1或2），默认2
    static int uploadDataByteLength;
    static int spatialResolution;
    // 【新增】截断距离 (10~20000m)，默认 20000m
    static int truncateDistance;
};

#endif // GLOBALVARS_H
