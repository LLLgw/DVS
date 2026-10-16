#include "rmsprocessor.h"
#include "GlobalVars.h"
#include <QDebug>
#include <numeric>
#include <execution>
#include <QtConcurrentRun>
#include <QtConcurrentMap>

RMSProcessor::RMSProcessor(QObject *parent)
    : QObject{parent}
    , m_windowSize(512)
    , m_refreshInterval(512)
    , m_collectionCount(0)
{
    m_currentRMS = QVector<double>(32000, 0.0);
    m_squaredSum = QVector<double>(32000, 0.0);

    //qDebug() << "RMSProcessor created (no thread management)";
}

RMSProcessor::~RMSProcessor()
{
    //stopProcessing();
    qDebug() << "RMSProcessor destroyed";
}

void RMSProcessor::processRMSData(const QVector<int16_t>& samples)
{
    updateRMS(samples);

    m_collectionCount++;

    checkForRefresh();

    //qDebug() << "RMSProcessor: Processed collection" << m_collectionCount;
}

void RMSProcessor::updateRMS(const QVector<int16_t>& newSamples)
{
    int points = newSamples.size();
    if (points > GlobalVars::lineSamplePoints)
        points = GlobalVars::lineSamplePoints;

    const double alpha = 1.0 / m_windowSize;  // 指数加权系数
// 并行更新每个采样点的RMS
/*
    QtConcurrent::blockingMap(0, points, [&](int i) {
        double newValue = static_cast<double>(newSamples[i]);

        if (m_collectionCount < m_windowSize) {
            m_squaredSum[i] += newValue * newValue;
            m_currentRMS[i] = std::sqrt(m_squaredSum[i] / (m_collectionCount + 1));
        } else {
            m_squaredSum[i] = (1.0 - alpha) * m_squaredSum[i] + newValue * newValue;
            m_currentRMS[i] = std::sqrt(m_squaredSum[i] / m_windowSize);
        }
    });
*/

    for (int i = 0; i < points; ++i) {
        double newValue = static_cast<double>(newSamples[i]) + 2000.0;
        //double newSquare = newValue * newValue;
        //double newSquare = newValue;

        // 递推更新平方和
        if(m_collectionCount < m_windowSize)
        {
            m_squaredSum[i] = m_squaredSum[i] + newValue;
            m_currentRMS[i] = m_squaredSum[i] / (m_collectionCount + 1);
        }
        else
        {
            //m_squaredSum[i] = (1.0 - alpha) * m_squaredSum[i] + alpha * newSquare;
            m_squaredSum[i] = (1.0 - alpha) * m_squaredSum[i] + newValue;
            m_currentRMS[i] = m_squaredSum[i] / m_windowSize;
        }
        //m_currentRMS[i] = 20.0 * log10(m_currentRMS[i]);
        //m_currentRMS[i] = newValue;
        // 计算RMS
        //if (m_squaredSum[i] > 0) {
        //    m_currentRMS[i] = std::sqrt(m_squaredSum[i]);
        //} else {
        //    m_currentRMS[i] = 0.0;
        //}
    }

    // 应用平滑滤波减少噪声
    //if (points > 10) {
    //    QVector<double> smoothed = applySavitzkyGolay(m_currentRMS, 7, 2);
    //    m_currentRMS = smoothed;
    //}
}

void RMSProcessor::checkForRefresh()
{
    if (m_collectionCount % m_refreshInterval == 0) {
        qDebug() << "RMSProcessor: Refreshing graph after"
                 << m_collectionCount << "collections";
        if(m_collectionCount >= 2 * m_windowSize)
            m_collectionCount = m_windowSize;

        QVector<int16_t> rmsData = convertToInt16(m_currentRMS);

        if(GlobalVars::rmsDataTransmitEnable)
            emit rmsDataProcessed(rmsData);
        //qDebug() << "RMS Data update";
    }
}

QVector<int16_t> RMSProcessor::convertToInt16(const QVector<double>& rmsValues)
{
    QVector<int16_t> result(GlobalVars::lineSamplePoints);

    if (rmsValues.isEmpty()) {
        result.fill(0);
        return result;
    }

    const double* src = rmsValues.constData();
    int16_t* dst = result.data();

    const int convertSize = qMin(rmsValues.size(), GlobalVars::lineSamplePoints);

    // 转换有效数据部分
    for (int i = 0; i < convertSize; ++i) {
        dst[i] = static_cast<int16_t>(src[i]);
    }

    // 如果输入数据不足，填充0
    if (convertSize < GlobalVars::lineSamplePoints) {
        std::fill(dst + convertSize, dst + GlobalVars::lineSamplePoints, 0);
    }

    return result;
}

QVector<double> RMSProcessor::applySavitzkyGolay(const QVector<double>& data, int window, int order) const
{
    // 简化的Savitzky-Golay滤波实现
    if (data.size() < window || window <= order) {
        return data;
    }

    QVector<double> result(data.size(), 0.0);
    int halfWindow = window / 2;

    // 预先计算权重系数（简化版本）
    QVector<double> weights(window, 1.0 / window);

    for (int i = 0; i < data.size(); ++i) {
        int start = std::max(0, i - halfWindow);
        int end = std::min<int>(data.size() - 1, i + halfWindow);

        // 调整窗口大小以适应边界
        int actualWindow = end - start + 1;
        if (actualWindow < 3) {
            result[i] = data[i];
            continue;
        }

        // 多项式拟合（简化：使用加权平均）
        double weightedSum = 0.0;
        double weightSum = 0.0;

        for (int j = 0; j < actualWindow; ++j) {
            int dataIdx = start + j;
            double weight = weights[j];
            weightedSum += data[dataIdx] * weight;
            weightSum += weight;
        }

        result[i] = weightedSum / weightSum;
    }

    return result;
}
