#include "chartmanager.h"
#include "GlobalVars.h"

ChartManager::ChartManager(QWidget *parent)
    : QWidget{parent}
    , m_spectrumColor(QColor(0, 120, 215))
    , m_rmsColor(QColor(220, 80, 60))
    , m_rawColor(QColor(60, 180, 80))
{
    initUI();
}

ChartManager::~ChartManager()
{
}

void ChartManager::initUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);

    // 创建三个图表
    createSpectrumChart();
    createRMSChart();
    createRawDataChart();

    // 设置布局权重，使三个图表均匀分布
    m_mainLayout->setStretch(0, 1);
    m_mainLayout->setStretch(1, 1);
    m_mainLayout->setStretch(2, 1);
}

void ChartManager::createSpectrumChart()
{
    // 创建图表
    m_spectrumChart = new QChart();
    m_spectrumChart->setAnimationOptions(QChart::NoAnimation);
    m_spectrumChart->setTitle("频谱功率曲线");
    m_spectrumChart->setTheme(QChart::ChartThemeLight);
    m_spectrumChart->legend()->setVisible(false);

    // 创建序列
    m_spectrumSeries = new QLineSeries();
    m_spectrumSeries->setName("频谱功率");
    m_spectrumSeries->setColor(m_spectrumColor);
    m_spectrumSeries->setPen(QPen(m_spectrumColor, 1.5));
    m_spectrumChart->addSeries(m_spectrumSeries);

    // 创建坐标轴
    m_spectrumXAxis = new QValueAxis();
    m_spectrumXAxis->setTitleText("距离 (m)");
    m_spectrumXAxis->setRange(0, 20000);
    m_spectrumXAxis->setLabelFormat("%.1f");
    m_spectrumXAxis->setTickCount(11);
    m_spectrumXAxis->setMinorTickCount(4);

    m_spectrumYAxis = new QValueAxis();
    m_spectrumYAxis->setTitleText("功率 (dB)");
    m_spectrumYAxis->setRange(0, 30);
    m_spectrumYAxis->setLabelFormat("%.2f");

    // 附加坐标轴
    m_spectrumChart->addAxis(m_spectrumXAxis, Qt::AlignBottom);
    m_spectrumChart->addAxis(m_spectrumYAxis, Qt::AlignLeft);
    m_spectrumSeries->attachAxis(m_spectrumXAxis);
    m_spectrumSeries->attachAxis(m_spectrumYAxis);

    // 创建图表视图
    m_spectrumChartView = new QChartView(m_spectrumChart);
    m_spectrumChartView->setRenderHint(QPainter::Antialiasing, false);
    m_spectrumChartView->setMinimumHeight(250);
    m_spectrumChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_mainLayout->addWidget(m_spectrumChartView);
}

void ChartManager::createRMSChart()
{
    // 创建图表
    m_rmsChart = new QChart();
    m_rmsChart->setAnimationOptions(QChart::NoAnimation);
    m_rmsChart->setTitle("RMS数据曲线");
    m_rmsChart->setTheme(QChart::ChartThemeLight);
    m_rmsChart->legend()->setVisible(false);

    // 创建序列
    m_rmsSeries = new QLineSeries();
    m_rmsSeries->setName("RMS数据");
    m_rmsSeries->setColor(m_rmsColor);
    m_rmsSeries->setPen(QPen(m_rmsColor, 1.5));
    m_rmsChart->addSeries(m_rmsSeries);

    // 创建坐标轴
    m_rmsXAxis = new QValueAxis();
    m_rmsXAxis->setTitleText("距离 (m)");
    m_rmsXAxis->setRange(0, 20000);
    m_rmsXAxis->setLabelFormat("%.1f");
    m_rmsXAxis->setTickCount(11);
    m_rmsXAxis->setMinorTickCount(4);

    m_rmsYAxis = new QValueAxis();
    m_rmsYAxis->setTitleText("RMS值");
    m_rmsYAxis->setRange(0, 4000);
    m_rmsYAxis->setLabelFormat("%.2f");

    // 附加坐标轴
    m_rmsChart->addAxis(m_rmsXAxis, Qt::AlignBottom);
    m_rmsChart->addAxis(m_rmsYAxis, Qt::AlignLeft);
    m_rmsSeries->attachAxis(m_rmsXAxis);
    m_rmsSeries->attachAxis(m_rmsYAxis);

    // 创建图表视图
    m_rmsChartView = new QChartView(m_rmsChart);
    m_rmsChartView->setRenderHint(QPainter::Antialiasing, false);
    m_rmsChartView->setMinimumHeight(250);
    m_rmsChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_mainLayout->addWidget(m_rmsChartView);
}

void ChartManager::createRawDataChart()
{
    // 创建图表
    m_rawChart = new QChart();
    m_rawChart->setAnimationOptions(QChart::NoAnimation);
    m_rawChart->setTitle("原始数据曲线");
    m_rawChart->setTheme(QChart::ChartThemeLight);
    m_rawChart->legend()->setVisible(false);

    // 创建序列
    m_rawSeries = new QLineSeries();
    m_rawSeries->setName("原始数据");
    m_rawSeries->setColor(m_rawColor);
    m_rawSeries->setPen(QPen(m_rawColor, 1.5));
    m_rawChart->addSeries(m_rawSeries);

    // 创建坐标轴
    m_rawXAxis = new QValueAxis();
    m_rawXAxis->setTitleText("距离 (m)");
    m_rawXAxis->setRange(0, 20000);
    m_rawXAxis->setLabelFormat("%.1f");
    m_rawXAxis->setTickCount(11);
    m_rawXAxis->setMinorTickCount(4);

    m_rawYAxis = new QValueAxis();
    m_rawYAxis->setTitleText("原始值");
    m_rawYAxis->setRange(-2500, 2500);
    m_rawYAxis->setLabelFormat("%.0f");

    // 附加坐标轴
    m_rawChart->addAxis(m_rawXAxis, Qt::AlignBottom);
    m_rawChart->addAxis(m_rawYAxis, Qt::AlignLeft);
    m_rawSeries->attachAxis(m_rawXAxis);
    m_rawSeries->attachAxis(m_rawYAxis);

    // 创建图表视图
    m_rawChartView = new QChartView(m_rawChart);
    m_rawChartView->setRenderHint(QPainter::Antialiasing, false);
    m_rawChartView->setMinimumHeight(250);
    m_rawChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_mainLayout->addWidget(m_rawChartView);
}

void ChartManager::updateSpectrumChart(const QVector<int16_t>& spectrum)
{
    if (spectrum.isEmpty()) return;

    QVector<QPointF> points;
    //points.reserve(GlobalVars::lineSampleDelayPoints + spectrum.size());
    points.reserve(spectrum.size());

    const double xScale = 100.0 / GlobalVars::adcSampleFrequency;
    // 前导零部分（延迟点）
    //for (int i = 0; i < GlobalVars::lineSampleDelayPoints; ++i) {
    //    points.append(QPointF(i * xScale, 0));
    //}

    // 数据部分
    for (int i = 0; i < spectrum.size(); ++i) {
        //double x = (i + GlobalVars::lineSampleDelayPoints) * xScale;
        double x = i * xScale;
        double y = static_cast<double>(spectrum[i]) / 100.0;  // 根据需要调整缩放因子

        points.append(QPointF(x, y));
    }

    // 更新曲线数据
    m_spectrumSeries->replace(points);

    // 更新X轴范围
    //double fiberLength = (GlobalVars::lineSampleDelayPoints + spectrum.size()) * xScale;
    double fiberLength = spectrum.size() * xScale;
    m_spectrumXAxis->setRange(0, fiberLength);
}

void ChartManager::updateRMSChart(const QVector<int16_t>& rmsData)
{
    if (rmsData.isEmpty()) return;

    QVector<QPointF> points;
    //points.reserve(GlobalVars::lineSampleDelayPoints + rmsData.size());
    points.reserve(rmsData.size());

    const double xScale = 100.0 / GlobalVars::adcSampleFrequency;
    // 前导零部分（延迟点）
    //for (int i = 0; i < GlobalVars::lineSampleDelayPoints; ++i) {
    //    points.append(QPointF(i * xScale, 0));
    //}

    // 数据部分
    for (int i = 0; i < rmsData.size(); ++i) {
        //double x = (i + GlobalVars::lineSampleDelayPoints) * xScale;
        double x = i * xScale;
        double y = static_cast<double>(rmsData[i]);  // 根据需要调整缩放因子

        points.append(QPointF(x, y));
    }

    // 更新曲线数据
    m_rmsSeries->replace(points);

    // 更新X轴范围
    //double fiberLength = (GlobalVars::lineSampleDelayPoints + rmsData.size()) * xScale;
    double fiberLength = rmsData.size() * xScale;
    m_rmsXAxis->setRange(0, fiberLength);

}

void ChartManager::updateRawDataChart(const QVector<int16_t>& rawData)
{
    if (rawData.isEmpty()) return;

    QVector<QPointF> points;
    //points.reserve(GlobalVars::lineSampleDelayPoints + rawData.size());
    points.reserve(rawData.size());

    const double xScale = 100.0 / GlobalVars::adcSampleFrequency;
    // 前导零部分（延迟点）
    //for (int i = 0; i < GlobalVars::lineSampleDelayPoints; ++i) {
    //    points.append(QPointF(i * xScale, 0));
    //}

    // 数据部分
    for (int i = 0; i < rawData.size(); ++i) {
        //double x = (i + GlobalVars::lineSampleDelayPoints) * xScale;
        double x = i * xScale;
        double y = static_cast<double>(rawData[i]);  // 根据需要调整缩放因子

        points.append(QPointF(x, y));
    }

    // 更新曲线数据
    m_rawSeries->replace(points);

    // 更新X轴范围
    //double fiberLength = (GlobalVars::lineSampleDelayPoints + rawData.size()) * xScale;
    double fiberLength = rawData.size() * xScale;
    m_rawXAxis->setRange(0, fiberLength);
}
