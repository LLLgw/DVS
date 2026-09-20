#include "systemgpiocontroller.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

SystemGPIOController::SystemGPIOController(QObject *parent)
    : QObject{parent}
    , m_initialized(false)
    , m_yellowLedState(false)
    , m_greenLedState(false)
    , m_dvsPowerState(false)
    , m_fourGPowerState(false)
    , m_systemStatus("Not initialized")
    , m_yellowBlinkMode(BLINK_OFF)
    , m_greenBlinkMode(BLINK_OFF)
{
    // 设置定时器
    m_yellowBlinkTimer.setSingleShot(false);
    m_greenBlinkTimer.setSingleShot(false);

    // 连接定时器信号到槽
    connect(&m_yellowBlinkTimer, &QTimer::timeout, this, &SystemGPIOController::onYellowBlinkTimeout);
    connect(&m_greenBlinkTimer, &QTimer::timeout, this, &SystemGPIOController::onGreenBlinkTimeout);

    qDebug() << "SystemGPIOController created";
}

SystemGPIOController::~SystemGPIOController()
{
    if (m_initialized) {
        qDebug() << "SystemGPIOController destructor - cleaning up...";

        // 停止所有定时器
        m_yellowBlinkTimer.stop();
        m_greenBlinkTimer.stop();

        // 关闭所有GPIO输出
        setGPIOValue(YELLOW_LED, false);
        setGPIOValue(GREEN_LED, false);
        setGPIOValue(DVS_POWER_CTRL, true);
        setGPIOValue(FOURG_POWER_CTRL, false);

        // 注意：这里不取消导出GPIO，因为有些系统在应用退出时会自动清理
        // 如果需要在退出时取消导出，可以添加unexport逻辑
    }
}

bool SystemGPIOController::initialize()
{
    QMutexLocker locker(&m_mutex);

    if (m_initialized) {
        qWarning() << "GPIO controller already initialized";
        return true;
    }

    qInfo() << "=== Initializing System GPIO Controller ===";

    // 更新系统状态
    m_systemStatus = "Initializing";

    // 1. 初始化黄色LED
    qInfo() << "Initializing YELLOW_LED (GPIO" << YELLOW_LED << ")...";
    if (!setupSingleGPIO(YELLOW_LED, true, false)) {
        qCritical() << "Failed to initialize YELLOW_LED";
        m_systemStatus = "Initialize failed - YELLOW_LED";

        return false;
    }
    m_yellowLedState = false;

    // 2. 初始化绿色LED
    qInfo() << "Initializing GREEN_LED (GPIO" << GREEN_LED << ")...";
    if (!setupSingleGPIO(GREEN_LED, true, false)) {
        qCritical() << "Failed to initialize GREEN_LED";
        unexportGPIO(YELLOW_LED);
        m_systemStatus = "Initialize failed - GREEN_LED";

        return false;
    }
    m_greenLedState = false;

    // 3. 初始化DVS电源控制,关闭DVS电源,高电平关闭，低电平打开
    qInfo() << "Initializing DVS_POWER_CTRL (GPIO" << DVS_POWER_CTRL << ")...";
    if (!setupSingleGPIO(DVS_POWER_CTRL, true, true)) {
        qCritical() << "Failed to initialize DVS_POWER_CTRL";
        unexportGPIO(YELLOW_LED);
        unexportGPIO(GREEN_LED);
        m_systemStatus = "Initialize failed - DVS_POWER";

        return false;
    }
    m_dvsPowerState = false;

    // 4. 初始化4G电源控制,打开4G电源,高电平关闭，低电平打开
    qInfo() << "Initializing 4G_POWER_CTRL (GPIO" << FOURG_POWER_CTRL << ")...";
    if (!setupSingleGPIO(FOURG_POWER_CTRL, true, false)) {
        qCritical() << "Failed to initialize 4G_POWER_CTRL";
        unexportGPIO(YELLOW_LED);
        unexportGPIO(GREEN_LED);
        unexportGPIO(DVS_POWER_CTRL);
        m_systemStatus = "Initialize failed - 4G_POWER";

        return false;
    }

    m_fourGPowerState = true;

    m_initialized = true;
    m_systemStatus = "Initialize success";

    qInfo() << "=== System GPIO Controller Initialized Successfully ===";
    qInfo() << "- YELLOW_LED: GPIO" << YELLOW_LED;
    qInfo() << "- GREEN_LED: GPIO" << GREEN_LED;
    qInfo() << "- DVS_POWER_CTRL: GPIO" << DVS_POWER_CTRL;
    qInfo() << "- 4G_POWER_CTRL: GPIO" << FOURG_POWER_CTRL;

    return true;
}

bool SystemGPIOController::setupSingleGPIO(int gpioNumber, bool isOutput, bool defaultValue)
{
    // 导出GPIO
    if (!exportGPIO(gpioNumber)) {
        qWarning() << "Failed to export GPIO" << gpioNumber;
        return false;
    }

    // 设置方向
    QString direction = isOutput ? "out" : "in";
    if (!setGPIODirection(gpioNumber, isOutput)) {
        qWarning() << "Failed to set direction for GPIO" << gpioNumber;
        unexportGPIO(gpioNumber);
        return false;
    }

    // 如果是输出，设置默认值
    if (isOutput) {
        if (!setGPIOValue(gpioNumber, defaultValue)) {
            qWarning() << "Failed to set default value for GPIO" << gpioNumber;
            unexportGPIO(gpioNumber);
            return false;
        }
    }

    qDebug() << "GPIO" << gpioNumber << "initialized as" << direction
             << "with default value" << defaultValue;

    return true;
}

bool SystemGPIOController::exportGPIO(int gpioNumber)
{
    QString exportPath = "/sys/class/gpio/export";

    // 先检查是否已导出
    QString gpioDir = QString("/sys/class/gpio/gpio%1").arg(gpioNumber);
    if (QDir(gpioDir).exists()) {
        qDebug() << "GPIO" << gpioNumber << "already exported";
        return true;
    }

    // 导出GPIO
    QFile exportFile(exportPath);
    if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open export file:" << exportFile.errorString();
        return false;
    }

    QTextStream stream(&exportFile);
    stream << gpioNumber;
    exportFile.close();

    // 等待系统创建文件（重要！）
    int retryCount = 0;
    while (retryCount < 10 && !QDir(gpioDir).exists()) {
        QThread::msleep(50);
        retryCount++;
    }

    if (!QDir(gpioDir).exists()) {
        qWarning() << "GPIO directory not created after export:" << gpioDir;
        return false;
    }

    qDebug() << "Exported GPIO" << gpioNumber;
    return true;
}

bool SystemGPIOController::unexportGPIO(int gpioNumber)
{
    QString unexportPath = "/sys/class/gpio/unexport";

    QFile unexportFile(unexportPath);
    if (!unexportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open unexport file:" << unexportFile.errorString();
        return false;
    }

    QTextStream stream(&unexportFile);
    stream << gpioNumber;
    unexportFile.close();

    qDebug() << "Unexported GPIO" << gpioNumber;
    return true;
}

bool SystemGPIOController::setGPIODirection(int gpioNumber, bool isOutput)
{
    QString directionPath = QString("/sys/class/gpio/gpio%1/direction").arg(gpioNumber);
    QString direction = isOutput ? "out" : "in";

    QFile directionFile(directionPath);
    if (!directionFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open direction file:" << directionFile.errorString();
        return false;
    }

    QTextStream stream(&directionFile);
    stream << direction;
    directionFile.close();

    qDebug() << "Set GPIO" << gpioNumber << "direction to" << direction;
    return true;
}

bool SystemGPIOController::setGPIOValue(int gpioNumber, bool value)
{
    QString valuePath = QString("/sys/class/gpio/gpio%1/value").arg(gpioNumber);

    // 检查文件是否存在
    QFile file(valuePath);
    if (!file.exists()) {
        qWarning() << "GPIO value file not exists:" << valuePath;
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open value file:" << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    stream << (value ? "1" : "0");
    file.close();

    return true;
}

bool SystemGPIOController::getGPIOValue(int gpioNumber)
{
    QString valuePath = QString("/sys/class/gpio/gpio%1/value").arg(gpioNumber);

    QFile file(valuePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open value file for reading:" << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    QString value = stream.readAll().trimmed();
    file.close();

    return value == "1";
}

// ==================== LED 控制方法 ====================

bool SystemGPIOController::setYellowLed(bool on)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    // 停止闪烁定时器
    if (m_yellowBlinkTimer.isActive()) {
        m_yellowBlinkTimer.stop();
        m_yellowBlinkMode = BLINK_OFF;
        qDebug() << "Stopped yellow LED blinking";
    }

    // 设置LED状态
    if (setGPIOValue(YELLOW_LED, on)) {
        bool oldState = m_yellowLedState;
        m_yellowLedState = on;

        if (oldState != on) {
            qInfo() << "Yellow LED:" << (on ? "ON" : "OFF");
        }

        return true;
    }

    QString errorMsg = QString("Failed to set yellow LED to %1").arg(on ? "ON" : "OFF");
    qWarning() << errorMsg;

    return false;
}

bool SystemGPIOController::setGreenLed(bool on)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    // 停止闪烁定时器
    if (m_greenBlinkTimer.isActive()) {
        m_greenBlinkTimer.stop();
        m_greenBlinkMode = BLINK_OFF;
        qDebug() << "Stopped green LED blinking";
    }

    // 设置LED状态
    if (setGPIOValue(GREEN_LED, on)) {
        bool oldState = m_greenLedState;
        m_greenLedState = on;

        if (oldState != on) {
            qInfo() << "Green LED:" << (on ? "ON" : "OFF");
        }

        return true;
    }

    QString errorMsg = QString("Failed to set green LED to %1").arg(on ? "ON" : "OFF");
    qWarning() << errorMsg;

    return false;
}

void SystemGPIOController::toggleYellowLed()
{
    bool newState = !m_yellowLedState;
    setYellowLed(newState);
}

void SystemGPIOController::toggleGreenLed()
{
    bool newState = !m_greenLedState;
    setGreenLed(newState);
}

bool SystemGPIOController::setYellowLedBlink(BlinkMode mode)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    m_yellowBlinkMode = mode;
    m_yellowBlinkTimer.stop();

    switch (mode) {
    case BLINK_OFF:
        // 直接关闭LED
        return setYellowLed(false);

    case BLINK_SLOW:
        // 慢闪：1Hz (500ms on, 500ms off)
        m_yellowBlinkTimer.setInterval(500);
        m_yellowBlinkTimer.start();
        qDebug() << "Yellow LED blinking SLOW (1Hz)";
        break;

    case BLINK_FAST:
        // 快闪：4Hz (125ms on, 125ms off)
        m_yellowBlinkTimer.setInterval(125);
        m_yellowBlinkTimer.start();
        qDebug() << "Yellow LED blinking FAST (4Hz)";
        break;

    case BLINK_DOUBLE:
        // 双闪：短-短-长模式
        m_yellowBlinkTimer.setInterval(200);
        m_yellowBlinkTimer.start();
        qDebug() << "Yellow LED blinking DOUBLE";
        break;

    case BLINK_BREATH:
        // 呼吸效果（简化版，使用PWM模拟）
        // 注意：sysfs GPIO不支持真正的PWM，这里使用快速闪烁模拟
        m_yellowBlinkTimer.setInterval(100);
        m_yellowBlinkTimer.start();
        qDebug() << "Yellow LED breathing effect";
        break;

    default:
        qWarning() << "Unknown blink mode for yellow LED:" << mode;
        return false;
    }

    return true;
}

bool SystemGPIOController::setGreenLedBlink(BlinkMode mode)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    m_greenBlinkMode = mode;
    m_greenBlinkTimer.stop();

    switch (mode) {
    case BLINK_OFF:
        return setGreenLed(false);

    case BLINK_SLOW:
        m_greenBlinkTimer.setInterval(500);
        m_greenBlinkTimer.start();
        qDebug() << "Green LED blinking SLOW (1Hz)";
        break;

    case BLINK_FAST:
        m_greenBlinkTimer.setInterval(125);
        m_greenBlinkTimer.start();
        qDebug() << "Green LED blinking FAST (4Hz)";
        break;

    case BLINK_DOUBLE:
        m_greenBlinkTimer.setInterval(200);
        m_greenBlinkTimer.start();
        qDebug() << "Green LED blinking DOUBLE";
        break;

    case BLINK_BREATH:
        m_greenBlinkTimer.setInterval(100);
        m_greenBlinkTimer.start();
        qDebug() << "Green LED breathing effect";
        break;

    default:
        qWarning() << "Unknown blink mode for green LED:" << mode;
        return false;
    }

    return true;
}

void SystemGPIOController::onYellowBlinkTimeout()
{
    static int doubleBlinkPhase = 0;

    switch (m_yellowBlinkMode) {
    case BLINK_SLOW:
    case BLINK_FAST:
        // 简单闪烁：反转当前状态
        {
            bool currentValue = getGPIOValue(YELLOW_LED);
            setGPIOValue(YELLOW_LED, !currentValue);
        }
        break;

    case BLINK_DOUBLE:
        // 双闪模式：亮-灭-亮-长灭
        switch (doubleBlinkPhase) {
        case 0:
            setGPIOValue(YELLOW_LED, true);  // 第一次亮
            doubleBlinkPhase++;
            break;
        case 1:
            setGPIOValue(YELLOW_LED, false); // 第一次灭
            doubleBlinkPhase++;
            break;
        case 2:
            setGPIOValue(YELLOW_LED, true);  // 第二次亮
            doubleBlinkPhase++;
            break;
        case 3:
            setGPIOValue(YELLOW_LED, false); // 长灭
            doubleBlinkPhase = 0;
            break;
        }
        break;

    case BLINK_BREATH:
        // 呼吸效果：使用快速闪烁模拟
        {
            static bool breathState = false;
            breathState = !breathState;
            setGPIOValue(YELLOW_LED, breathState);
        }
        break;

    default:
        // 不应该执行到这里
        m_yellowBlinkTimer.stop();
        break;
    }
}

void SystemGPIOController::onGreenBlinkTimeout()
{
    static int doubleBlinkPhase = 0;

    switch (m_greenBlinkMode) {
    case BLINK_SLOW:
    case BLINK_FAST:
    {
        bool currentValue = getGPIOValue(GREEN_LED);
        setGPIOValue(GREEN_LED, !currentValue);
    }
    break;

    case BLINK_DOUBLE:
        switch (doubleBlinkPhase) {
        case 0:
            setGPIOValue(GREEN_LED, true);
            doubleBlinkPhase++;
            break;
        case 1:
            setGPIOValue(GREEN_LED, false);
            doubleBlinkPhase++;
            break;
        case 2:
            setGPIOValue(GREEN_LED, true);
            doubleBlinkPhase++;
            break;
        case 3:
            setGPIOValue(GREEN_LED, false);
            doubleBlinkPhase = 0;
            break;
        }
        break;

    case BLINK_BREATH:
    {
        static bool breathState = false;
        breathState = !breathState;
        setGPIOValue(GREEN_LED, breathState);
    }
    break;

    default:
        m_greenBlinkTimer.stop();
        break;
    }
}

// ==================== 电源控制方法 ====================

bool SystemGPIOController::setDvsPower(bool on)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    // DVS电源控制逻辑
    if (on) {
        qInfo() << "Powering ON DVS...";
        setGPIOValue(DVS_POWER_CTRL, false);
    } else {
        qInfo() << "Powering OFF DVS...";
        setGPIOValue(DVS_POWER_CTRL, true);
    }

    return true;
}

bool SystemGPIOController::setFourGPower(bool on)
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        qWarning() << "GPIO controller not initialized";
        return false;
    }

    // 4G模块电源控制（通常需要特定时序）
    if (on) {
        qInfo() << "Powering ON 4G module...";
        setGPIOValue(FOURG_POWER_CTRL, false);
        // 典型的4G模块上电序列：
        // 1. 拉高PWRKEY至少1秒
        // 2. 释放
        // 3. 等待模块启动
    } else {
        qInfo() << "Powering OFF 4G module...";
        setGPIOValue(FOURG_POWER_CTRL, true);
    }

    return true;
}

// ==================== 系统控制序列 ====================

void SystemGPIOController::systemStartupSequence()
{
    if (!m_initialized) {
        qWarning() << "Cannot perform startup sequence - GPIO not initialized";
        return;
    }

    qInfo() << "=== Starting System Startup Sequence ===";
    m_systemStatus = "系统启动中...";
    emit systemStatusUpdated(m_systemStatus);

    // 1. 黄色LED呼吸效果表示正在启动
    setYellowLedBlink(BLINK_BREATH);

    // 2. 开启DVS电源
    QThread::msleep(500);
    setDvsPower(true);

    // 3. 开启4G电源
    QThread::msleep(1000);
    setFourGPower(true);

    // 4. 等待系统稳定
    QThread::msleep(2000);

    // 5. 启动完成指示
    setYellowLed(true);           // 黄色LED常亮
    setGreenLedBlink(BLINK_SLOW); // 绿色LED慢闪

    m_systemStatus = "系统已就绪";
    qInfo() << "=== System Startup Sequence Completed ===";
    emit systemStatusUpdated(m_systemStatus);
}

void SystemGPIOController::systemShutdownSequence()
{
    if (!m_initialized) {
        qWarning() << "Cannot perform shutdown sequence - GPIO not initialized";
        return;
    }

    qInfo() << "=== Starting System Shutdown Sequence ===";
    m_systemStatus = "系统关闭中...";
    emit systemStatusUpdated(m_systemStatus);

    // 1. 绿色LED双闪表示正在关闭
    setGreenLedBlink(BLINK_DOUBLE);

    // 2. 关闭4G电源
    QThread::msleep(1000);
    setFourGPower(false);

    // 3. 关闭DVS电源
    QThread::msleep(1000);
    setDvsPower(false);

    // 4. 关闭所有LED
    QThread::msleep(500);
    setYellowLed(false);
    setGreenLed(false);

    m_systemStatus = "系统已关闭";
    qInfo() << "=== System Shutdown Sequence Completed ===";
    emit systemStatusUpdated(m_systemStatus);
}

void SystemGPIOController::emergencyStop()
{
    qCritical() << "!!! EMERGENCY STOP TRIGGERED !!!";

    m_systemStatus = "紧急停止！";
    emit systemStatusUpdated(m_systemStatus);

    // 立即停止所有定时器
    m_yellowBlinkTimer.stop();
    m_greenBlinkTimer.stop();

    // 立即关闭所有电源
    setGPIOValue(DVS_POWER_CTRL, false);
    setGPIOValue(FOURG_POWER_CTRL, false);

    // LED快速交替闪烁报警
    QTimer *emergencyTimer = new QTimer(this);
    bool emergencyState = false;

    // 使用lambda表达式连接
    connect(emergencyTimer, &QTimer::timeout, this, [this, &emergencyState]() {
        emergencyState = !emergencyState;
        setGPIOValue(YELLOW_LED, emergencyState);
        setGPIOValue(GREEN_LED, !emergencyState);
    });

    emergencyTimer->start(100); // 10Hz交替闪烁

    // 10秒后停止报警
    QTimer::singleShot(10000, this, [this, emergencyTimer]() {
        emergencyTimer->stop();
        emergencyTimer->deleteLater();
        setYellowLed(false);
        setGreenLed(false);
        m_systemStatus = "紧急停止完成";
        emit systemStatusUpdated(m_systemStatus);
    });

    emit systemError("紧急停止已触发");
}

// ==================== 状态查询方法 ====================

QString SystemGPIOController::getGPIOStatus() const
{
    QString status;

    status += QString("=== 系统GPIO状态 ===\n");
    status += QString("系统状态: %1\n").arg(m_systemStatus);
    status += QString("初始化状态: %1\n").arg(m_initialized ? "已初始化" : "未初始化");
    status += QString("\n");

    status += QString("黄色LED (GPIO%1): %2\n")
                  .arg(YELLOW_LED)
                  .arg(m_yellowLedState ? "开" : "关");

    status += QString("绿色LED (GPIO%1): %2\n")
                  .arg(GREEN_LED)
                  .arg(m_greenLedState ? "开" : "关");

    status += QString("DVS电源 (GPIO%1): %2\n")
                  .arg(DVS_POWER_CTRL)
                  .arg(m_dvsPowerState ? "开" : "关");

    status += QString("4G电源 (GPIO%1): %2\n")
                  .arg(FOURG_POWER_CTRL)
                  .arg(m_fourGPowerState ? "开" : "关");

    // 添加闪烁模式信息
    if (m_yellowBlinkTimer.isActive()) {
        QString modeStr;
        switch (m_yellowBlinkMode) {
        case BLINK_SLOW: modeStr = "慢闪"; break;
        case BLINK_FAST: modeStr = "快闪"; break;
        case BLINK_DOUBLE: modeStr = "双闪"; break;
        case BLINK_BREATH: modeStr = "呼吸"; break;
        default: modeStr = "未知";
        }
        status += QString("黄色LED闪烁模式: %1\n").arg(modeStr);
    }

    if (m_greenBlinkTimer.isActive()) {
        QString modeStr;
        switch (m_greenBlinkMode) {
        case BLINK_SLOW: modeStr = "慢闪"; break;
        case BLINK_FAST: modeStr = "快闪"; break;
        case BLINK_DOUBLE: modeStr = "双闪"; break;
        case BLINK_BREATH: modeStr = "呼吸"; break;
        default: modeStr = "未知";
        }
        status += QString("绿色LED闪烁模式: %1\n").arg(modeStr);
    }

    return status;
}

