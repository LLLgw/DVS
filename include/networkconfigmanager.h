#ifndef NETWORKCONFIGMANAGER_H
#define NETWORKCONFIGMANAGER_H

#include <QString>
#include <QtGlobal>

struct UdpReportTargetConfig
{
    QString ip;
    quint16 port;
};

struct TcpEndpointConfig
{
    QString ip;
    quint16 port;
};

struct TcpUpstreamConfig
{
    TcpEndpointConfig configured;
    TcpEndpointConfig lastKnownGood;
    QString state; // committed / pending
};

struct NetworkConfig
{
    UdpReportTargetConfig udpReportTarget;
    TcpUpstreamConfig tcpUpstream;
};

class NetworkConfigManager
{
public:
    static NetworkConfig load();
    static bool save(const NetworkConfig &config);
    static NetworkConfig defaultConfig();

    static bool isValidIpv4Endpoint(const QString &ip, quint16 port);
    static QString committedState();
    static QString pendingState();
};

#endif // NETWORKCONFIGMANAGER_H
