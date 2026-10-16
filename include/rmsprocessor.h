#ifndef RMSPROCESSOR_H
#define RMSPROCESSOR_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QThread>
//#include <QWaitCondition>
#include <QDebug>
#include <cmath>
//#include <algorithm>
//#include <functional>

class RMSProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RMSProcessor(QObject *parent = nullptr);
    ~RMSProcessor();

public slots:   
    void processRMSData(const QVector<int16_t>& samples);

signals:
    void rmsDataProcessed(const QVector<int16_t>& rmsData);

private:
    void updateRMS(const QVector<int16_t>& newSamples);
    void checkForRefresh();
    QVector<int16_t> convertToInt16(const QVector<double>& rmsValues);
    QVector<double> applySavitzkyGolay(const QVector<double>& data, int window, int order) const;

    QVector<double> m_currentRMS;          // 当前RMS值（20000个点）
    QVector<double> m_squaredSum;          // 平方和（用于递推计算）

    QVector<QVector<int16_t>> m_slidingWindow;  // 最近的windowSize次采集
    int m_windowSize;                            // 窗口大小
    int m_windowIndex;                           // 当前窗口位置

    int m_refreshInterval;      // 刷新间隔（采集次数）
    int m_collectionCount;      // 采集计数

    mutable QMutex m_dataMutex;
};

#endif // RMSPROCESSOR_H
