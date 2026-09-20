#include "mainwindow.h"
#include "message.h"
#include "GlobalVars.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("DVS System"));
    resize(1000, 600);

    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 创建图表管理器
    dataGraph = new ChartManager(centralWidget);
    mainLayout->addWidget(dataGraph);
    setCentralWidget(centralWidget);

    creatActions();    
    creatToolBars();

    // 创建对象
    configParamWidget = new ConfigTestParametersWidget;     /*配置参数面板*/
    queryParamWidget = new QueryParametersWidget;           /*查询参数面板*/
    testWidget = new TestWidget;                            /*测试面板*/
    msgBar = statusBar();
    statusLabel = new QLabel(this);
    msgBar->addPermanentWidget(statusLabel);
    mainController = new Controller(this);

    // 连接信号槽
    //connect(mainController, &Controller::specDataReady, dataGraph, &ChartManager::updateSpectrumChart, Qt::QueuedConnection);
    //connect(mainController, &Controller::rmsDataReady, dataGraph, &ChartManager::updateRMSChart, Qt::QueuedConnection);
    //connect(mainController, &Controller::rawDataReady, dataGraph, &ChartManager::updateRawDataChart, Qt::QueuedConnection);
    connect(testWidget, &TestWidget::sendTestCmd, mainController, &Controller::onSendFpgaCmd);
    connect(configParamWidget, &ConfigTestParametersWidget::sendConfigCmd, mainController, &Controller::onSendFpgaCmd);
    connect(queryParamWidget, &QueryParametersWidget::sendQueryCmd, mainController, &Controller::onSendFpgaCmd);

    connect(testWidget, &TestWidget::startReceiveData, mainController, &Controller::startThread);
    connect(testWidget, &TestWidget::stopReceiveData, mainController, &Controller::stopThread);
}

void MainWindow::creatActions()
{
    parameterConfigAction = new QAction(tr("配置"), this);
    parameterConfigAction->setShortcut(tr("Ctrl+P"));
    parameterConfigAction->setStatusTip(tr("配置测试参数"));
    connect(parameterConfigAction, &QAction::triggered, this, &MainWindow::configTestParameters);

    parameterQueryAction = new QAction(tr("查询"), this);
    parameterQueryAction->setShortcut(tr("Ctrl+P"));
    parameterQueryAction->setStatusTip(tr("查询测试参数"));
    connect(parameterQueryAction, &QAction::triggered, this, &MainWindow::queryTestParameters);

    testAction = new QAction(tr("测试"), this);
    testAction->setShortcut(tr("Ctrl+T"));
    testAction->setStatusTip(tr("测试"));
    connect(testAction, &QAction::triggered, this, &MainWindow::test);

    exitAction = new QAction(tr("退出"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("退出程序"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::creatMenus()
{
    configMenu = menuBar()->addMenu(tr("配置"));
    configMenu->addAction(parameterConfigAction);
    queryMenu = menuBar()->addMenu(tr("查询"));
    queryMenu->addAction(parameterQueryAction);
    testMenu = menuBar()->addMenu(tr("测试"));
    testMenu->addAction(testAction);
}

void MainWindow::creatToolBars()
{
    configTool = addToolBar("Config");
    configTool->addAction(parameterConfigAction);
    configTool->setAllowedAreas(Qt::TopToolBarArea);
    configTool->setMovable(false);
    queryTool = addToolBar("Query");
    queryTool->addAction(parameterQueryAction);
    queryTool->setAllowedAreas(Qt::TopToolBarArea);
    queryTool->setMovable(false);
    testTool = addToolBar("Test");
    testTool->addAction(testAction);
    testTool->setAllowedAreas(Qt::TopToolBarArea);
    testTool->setMovable(false);
}

void MainWindow::creatChart()
{
    dataGraph = new ChartManager();
}

void MainWindow::configTestParameters()
{
    configParamWidget->show();
}

void MainWindow::queryTestParameters()
{
    queryParamWidget->show();
}

void MainWindow::test()
{
    testWidget->show();
}

void MainWindow::close()
{}

MainWindow::~MainWindow()
{
    if (mainController)
    {
        delete mainController;
        mainController = nullptr;
    }

    disconnect();
}
