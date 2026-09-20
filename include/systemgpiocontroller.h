#ifndef SYSTEMGPIOCONTROLLER_H
#define SYSTEMGPIOCONTROLLER_H

#include <QObject>
//#include <QMap>
#include <QTimer>
#include <QMutex>

// GPIO 定义
enum GPIOPin {
    YELLOW_LED = 17,      // GPIO0_C1_d
    GREEN_LED = 23,       // GPIO0_C7_d
    DVS_POWER_CTRL = 140, // GPIO4_B4_d
    FOURG_POWER_CTRL = 141 // GPIO4_B5_d
};

// LED 闪烁模式
enum BlinkMode {
    BLINK_OFF,
    BLINK_SLOW,    // 1Hz
    BLINK_FAST,    // 4Hz
    BLINK_DOUBLE,  // 双闪
    BLINK_BREATH   // 呼吸效果
};

class SystemGPIOController : public QObject
{
    Q_OBJECT

public:
/*
    // GPIO 定义
    enum GPIOPin {
        YELLOW_LED = 17,      // GPIO0_C1_d
        GREEN_LED = 23,       // GPIO0_C7_d
        DVS_POWER_CTRL = 140, // GPIO4_B4_d
        FOURG_POWER_CTRL = 141 // GPIO4_B5_d
    };

    // LED 闪烁模式
    enum BlinkMode {
        BLINK_OFF,
        BLINK_SLOW,    // 1Hz
        BLINK_FAST,    // 4Hz
        BLINK_DOUBLE,  // 双闪
        BLINK_BREATH   // 呼吸效果
    };
*/
    explicit SystemGPIOController(QObject *parent = nullptr);
    ~SystemGPIOController();

    // 初始化
    bool initialize();
    bool isInitialized() const { return m_initialized; }

    // LED控制
    bool setYellowLed(bool on);
    bool setGreenLed(bool on);
    void toggleYellowLed();
    void toggleGreenLed();
    bool setYellowLedBlink(BlinkMode mode);
    bool setGreenLedBlink(BlinkMode mode);

    // 电源控制
    bool setDvsPower(bool on);
    bool setFourGPower(bool on);

    // 系统控制
    void systemStartupSequence();
    void systemShutdownSequence();
    void emergencyStop();

    // 状态查询
    bool yellowLedState() const { return m_yellowLedState; }
    bool greenLedState() const { return m_greenLedState; }
    bool dvsPowerState() const { return m_dvsPowerState; }
    bool fourGPowerState() const { return m_fourGPowerState; }

    QString getGPIOStatus() const;
    QString getSystemStatus() const { return m_systemStatus; }

signals:
    // 状态变化信号
    void yellowLedChanged(bool state);
    void greenLedChanged(bool state);
    void dvsPowerChanged(bool state);
    void fourGPowerChanged(bool state);

    // 系统事件信号
    void systemInitialized(bool success);
    void systemError(const QString &error);
    void systemStatusUpdated(const QString &status);

private:
    // 私有成员变量
    bool m_initialized = false;
    bool m_yellowLedState = false;
    bool m_greenLedState = false;
    bool m_dvsPowerState = false;
    bool m_fourGPowerState = false;

    QMutex m_mutex;
    QTimer m_yellowBlinkTimer;
    QTimer m_greenBlinkTimer;
    QString m_systemStatus;

    BlinkMode m_yellowBlinkMode = BLINK_OFF;
    BlinkMode m_greenBlinkMode = BLINK_OFF;

    // 私有方法
    bool exportGPIO(int gpioNumber);
    bool unexportGPIO(int gpioNumber);
    bool setGPIODirection(int gpioNumber, bool isOutput);
    bool setGPIOValue(int gpioNumber, bool value);
    bool getGPIOValue(int gpioNumber);

    bool setupSingleGPIO(int gpioNumber, bool isOutput, bool defaultValue);

private slots:
    void onYellowBlinkTimeout();
    void onGreenBlinkTimeout();
};

#endif // SYSTEMGPIOCONTROLLER_H
