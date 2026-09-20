#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QWidget>
#include <QtCharts>

class ChartManager : public QWidget
{
    Q_OBJECT
public:
    explicit ChartManager(QWidget *parent = nullptr);
    ~ChartManager();

public slots:
    // 更新图表数据
    void updateSpectrumChart(const QVector<int16_t>& spectrum);
    void updateRMSChart(const QVector<int16_t>& rmsData);
    void updateRawDataChart(const QVector<int16_t>& rawData);

signals:

private:
    // 初始化函数
    void initUI();
    void createSpectrumChart();
    void createRMSChart();
    void createRawDataChart();

    // 图表相关
    QVBoxLayout* m_mainLayout;

    // 频谱图表
    QChart* m_spectrumChart;
    QChartView* m_spectrumChartView;
    QLineSeries* m_spectrumSeries;
    QValueAxis* m_spectrumXAxis;
    QValueAxis* m_spectrumYAxis;

    // RMS图表
    QChart* m_rmsChart;
    QChartView* m_rmsChartView;
    QLineSeries* m_rmsSeries;
    QValueAxis* m_rmsXAxis;
    QValueAxis* m_rmsYAxis;

    // 原始数据图表
    QChart* m_rawChart;
    QChartView* m_rawChartView;
    QLineSeries* m_rawSeries;
    QValueAxis* m_rawXAxis;
    QValueAxis* m_rawYAxis;

    // 颜色定义
    QColor m_spectrumColor;
    QColor m_rmsColor;
    QColor m_rawColor;
};

#endif // CHARTMANAGER_H
