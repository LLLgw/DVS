#include "GlobalVars.h"

std::mutex GlobalVars::g_mutex;
int GlobalVars::signalSampleFrequency = 5120;
//int GlobalVars::signalSampleFrequency = 5000;
int GlobalVars::signalSamplePoints = 128;
//int GlobalVars::signalSamplePoints = 64;
// 调试平均策略只改这两项：硬件平均 * 软件平均 = 总平均
int GlobalVars::signalAverageNumber = 8;
int GlobalVars::signalSoftwareAverageNumber = 1;
int GlobalVars::lineSamplePoints = 20000;
//int GlobalVars::lineSamplePoints = 20000;
int GlobalVars::lineSampleDelayPoints = 0;
int GlobalVars::adcSampleFrequency = 100;                       //MHz
//int GlobalVars::adcSampleFrequency = 50;                       //MHz
int GlobalVars::kMaxSamplesPerFrame = 712;
int GlobalVars::kFullFrames = GlobalVars::lineSamplePoints / 712;                      //每线测试完整帧数
int GlobalVars::kRemainingSamples = GlobalVars::lineSamplePoints % 712;                //剩余采样点
int GlobalVars::kTotalFramesPerCycle = GlobalVars::kRemainingSamples ? (GlobalVars::kFullFrames + 1) : GlobalVars::kFullFrames;         //每线测试总帧数
int GlobalVars::lowCutFrequency = 0;                           //低频截止频率
int GlobalVars::highCutFrequency = 320;                         //高频截止频率
double GlobalVars::deltaFrequency = static_cast<double>(GlobalVars::signalSampleFrequency) /
                                   GlobalVars::signalSamplePoints /
                                   (GlobalVars::signalAverageNumber * GlobalVars::signalSoftwareAverageNumber); //频点间隔频率
//double GlobalVars::deltaFrequency = 4096.0 / 8.0 /64.0;        //频点间隔频率
//double GlobalVars::deltaFrequency = 5000.0 / 8.0 /128.0;        //频点间隔频率
int GlobalVars::minFrequencyPoint = static_cast<int>(GlobalVars::lowCutFrequency / GlobalVars::deltaFrequency); //低频截止频率对应频点
//int GlobalVars::minFrequencyPoint = 30 / 8.0;                   //低频截止频率对应频点
//int GlobalVars::minFrequencyPoint = 30 / 4.8828;                   //低频截止频率对应频点
int GlobalVars::maxFrequencyPoint = static_cast<int>(GlobalVars::highCutFrequency / GlobalVars::deltaFrequency); //高频截止频率对应频点
//int GlobalVars::maxFrequencyPoint = 150 / 8.0;                  //高频截止频率对应频点
//int GlobalVars::maxFrequencyPoint = 150 / 4.8828;                  //高频截止频率对应频点
uint8_t GlobalVars::systemState = SYSTEM_IDLE;
bool GlobalVars::averageEnable = false;
bool GlobalVars::differenceEnable = false;
bool GlobalVars::rmsDataTransmitEnable = false;
bool GlobalVars::rawDataDisplayEnable = false;
bool GlobalVars::fpgaConnected = false;
bool GlobalVars::serverConnected = false;
bool GlobalVars::appLogIn = false;
bool GlobalVars::fpgaInited = false;
int GlobalVars::uploadDataByteLength = 2;
int GlobalVars::spatialResolution = 1;
// 【新增】默认截断距离为满量程 20000
int GlobalVars::truncateDistance = 20000;
