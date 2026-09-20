#ifndef QUERYPARAMETERSWIDGET_H
#define QUERYPARAMETERSWIDGET_H

#include <QWidget>
#include <QPushButton>

class QueryParametersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QueryParametersWidget(QWidget *parent = nullptr);

private:
    QPushButton *querySamplePointsButton;
    QPushButton *querySampleDelayPointsButton;
    QPushButton *queryPulseFrequencyButton;
    QPushButton *queryPulseWidthButton;
    QPushButton *querySampleFrequencyButton;
    QPushButton *queryAverageNumberButton;
    QPushButton *queryBiasVoltageButton;
    QPushButton *querySampleChannelsButton;
    QPushButton *queryAverageStatusButton;
    QPushButton *queryDifferenceStatusButton;
    QPushButton *queryTriggerSourceButton;

signals:
    void sendQueryCmd(QByteArray cmd);

protected slots:
    void querySamplePoints();
    void querySampleDelayPoints();
    void queryPulseFrequency();
    void queryPulseWidth();
    void querySampleFrequency();
    void queryAverageNumber();
    void queryBiasVoltage();
    void querySampleChannels();
    void queryAverageStatus();
    void queryDifferenceStatus();
    void queryTriggerSource();
};

#endif // QUERYPARAMETERSWIDGET_H
