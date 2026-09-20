#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QComboBox>
#include <QSpinBox>
#include <QToolBar>
#include <QFontComboBox>
#include <QToolButton>
#include <QTextCharFormat>
//#include <QChart>
#include <QtCharts>
//#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <QPointer>
#include "controller.h"
#include "chartmanager.h"
#include "configtestparameterswidget.h"
#include "queryparameterswidget.h"
#include "testwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void creatActions();
    void creatMenus();
    void creatToolBars();
    void creatChart();
    void setupCmdSocket();

private:
    QMenu *configMenu;
    QMenu *queryMenu;
    QMenu *testMenu;
    QAction *parameterConfigAction;
    QAction *parameterQueryAction;
    QAction *testAction;
    QAction *exitAction;
    QToolBar *configTool;
    QToolBar *queryTool;
    QToolBar *testTool;
    QStatusBar *msgBar;
    QLabel *statusLabel;

    Controller *mainController;
    ConfigTestParametersWidget *configParamWidget;
    QueryParametersWidget *queryParamWidget;
    TestWidget *testWidget;
    ChartManager *dataGraph;

private slots:
    void configTestParameters();
    void queryTestParameters();
    void test();
    void close();
};

#endif // MAINWINDOW_H
