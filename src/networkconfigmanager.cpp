#include "networkconfigmanager.h"

#include <QCoreApplication>
#include <QAbstractSocket>
#include <QDir>
#include <QFileInfo>
#include <QHostAddress>
#include <QSettings>
#include <QStandardPaths>

namespace {

bool isValidIpv4Addr(const QString &ip)
{
    QHostAddress addr;
    if (!addr.setAddress(ip) || addr.protocol() != QAbstractSocket::IPv4Protocol)
        return false;

    const quint32 ipValue = addr.toIPv4Address();
    if (ipValue == 0 || ipValue == 0xFFFFFFFFu)
        return false;

    return true;
}

QString configFilePath()
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (dirPath.isEmpty())
        dirPath = QCoreApplication::applicationDirPath();

    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(".");

    return dir.filePath("network.ini");
}

} // namespace

NetworkConfig NetworkConfigManager::defaultConfig()
{
    NetworkConfig config;
    config.udpReportTarget.ip = QStringLiteral("8.138.101.239");
    config.udpReportTarget.port = 41235;   // 41235改

    config.tcpUpstream.configured.ip = QStringLiteral("8.138.101.239");
    config.tcpUpstream.configured.port = 49877; // 49877改
    config.tcpUpstream.lastKnownGood.ip = QStringLiteral("8.138.101.239");
    config.tcpUpstream.lastKnownGood.port = 49877;  // 49877改
    config.tcpUpstream.state = committedState();

    return config;
}

NetworkConfig NetworkConfigManager::load()
{
    NetworkConfig config = defaultConfig();

    QSettings settings(configFilePath(), QSettings::IniFormat);
    const QString ip = settings.value(QStringLiteral("udpReportTarget/ip"), config.udpReportTarget.ip).toString().trimmed();
    const quint16 port = static_cast<quint16>(settings.value(QStringLiteral("udpReportTarget/port"), config.udpReportTarget.port).toUInt());

    if (isValidIpv4Endpoint(ip, port))
    {
        config.udpReportTarget.ip = ip;
        config.udpReportTarget.port = port;
    }

    const QString configuredIp = settings.value(QStringLiteral("tcpUpstream/configured/ip"),
                                                config.tcpUpstream.configured.ip).toString().trimmed();
    const quint16 configuredPort = static_cast<quint16>(settings.value(QStringLiteral("tcpUpstream/configured/port"),
                                                                       config.tcpUpstream.configured.port).toUInt());
    if (isValidIpv4Endpoint(configuredIp, configuredPort))
    {
        config.tcpUpstream.configured.ip = configuredIp;
        config.tcpUpstream.configured.port = configuredPort;
    }

    const QString lastGoodIp = settings.value(QStringLiteral("tcpUpstream/lastKnownGood/ip"),
                                              config.tcpUpstream.lastKnownGood.ip).toString().trimmed();
    const quint16 lastGoodPort = static_cast<quint16>(settings.value(QStringLiteral("tcpUpstream/lastKnownGood/port"),
                                                                     config.tcpUpstream.lastKnownGood.port).toUInt());
    if (isValidIpv4Endpoint(lastGoodIp, lastGoodPort))
    {
        config.tcpUpstream.lastKnownGood.ip = lastGoodIp;
        config.tcpUpstream.lastKnownGood.port = lastGoodPort;
    }

    QString state = settings.value(QStringLiteral("tcpUpstream/state"), committedState()).toString().trimmed().toLower();
    if (state != pendingState())
        state = committedState();
    config.tcpUpstream.state = state;

    if (!isValidIpv4Endpoint(config.tcpUpstream.lastKnownGood.ip, config.tcpUpstream.lastKnownGood.port))
    {
        config.tcpUpstream.lastKnownGood = config.tcpUpstream.configured;
    }

    return config;
}

bool NetworkConfigManager::save(const NetworkConfig &config)
{
    if (!isValidIpv4Endpoint(config.udpReportTarget.ip, config.udpReportTarget.port))
        return false;

    if (!isValidIpv4Endpoint(config.tcpUpstream.configured.ip, config.tcpUpstream.configured.port))
        return false;

    if (!isValidIpv4Endpoint(config.tcpUpstream.lastKnownGood.ip, config.tcpUpstream.lastKnownGood.port))
        return false;

    const QString filePath = configFilePath();
    QFileInfo fileInfo(filePath);
    QDir dir(fileInfo.absolutePath());
    if (!dir.exists() && !dir.mkpath("."))
        return false;

    QSettings settings(filePath, QSettings::IniFormat);
    settings.setValue(QStringLiteral("udpReportTarget/ip"), config.udpReportTarget.ip);
    settings.setValue(QStringLiteral("udpReportTarget/port"), config.udpReportTarget.port);

    settings.setValue(QStringLiteral("tcpUpstream/configured/ip"), config.tcpUpstream.configured.ip);
    settings.setValue(QStringLiteral("tcpUpstream/configured/port"), config.tcpUpstream.configured.port);
    settings.setValue(QStringLiteral("tcpUpstream/lastKnownGood/ip"), config.tcpUpstream.lastKnownGood.ip);
    settings.setValue(QStringLiteral("tcpUpstream/lastKnownGood/port"), config.tcpUpstream.lastKnownGood.port);

    QString state = config.tcpUpstream.state.trimmed().toLower();
    if (state != pendingState())
        state = committedState();
    settings.setValue(QStringLiteral("tcpUpstream/state"), state);

    settings.sync();

    return settings.status() == QSettings::NoError;
}

bool NetworkConfigManager::isValidIpv4Endpoint(const QString &ip, quint16 port)
{
    if (port == 0)
        return false;

    return isValidIpv4Addr(ip);
}

QString NetworkConfigManager::committedState()
{
    return QStringLiteral("committed");
}

QString NetworkConfigManager::pendingState()
{
    return QStringLiteral("pending");
}
