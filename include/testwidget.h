#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QUdpSocket>

class TestWidget : public QWidget
//class TestWidget : public QDialog
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);

private:
    QPushButton *enableTestButton;
    QPushButton *disableTestButton;
    QPushButton *rawDataButton;
    //QPushButton *specDataButton;

signals:
    void sendTestCmd(QByteArray cmd);
    //void sendStopTestCmd(QByteArray cmd);
    void startReceiveData();
    void stopReceiveData();

protected slots:
    void startTest();
    void stopTest();
    void displayRawData();
    //void displaySpecData();
};

#endif // TESTWIDGET_H
