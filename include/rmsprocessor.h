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
    void processRMSData(const QVector<float> samples);

signals:
    void rmsDataProcessed(const QVector<int16_t>& rmsData);

private:
    void checkForRefresh();
    QVector<int16_t> convertToInt16();
    void updateRMS(const QVector<float>& newSamples);
    QVector<double> applySavitzkyGolay(const QVector<double>& data, int window, int order) const;

    QVector<float> m_currentRMS;          // 当前RMS值（20000个点）

    int m_windowSize;           // 窗口大小
    int m_windowIndex;          // 当前窗口位置
    int m_refreshInterval;      // 刷新间隔（采集次数）
};

#endif // RMSPROCESSOR_H
