#include "GlobalConfig.h"
#include <QGuiApplication>
#include <QScreen>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>
// 实现单例获取方法
sysconfig::GlobalConfig& sysconfig::GlobalConfig::get() {
    static GlobalConfig instance;
    return instance;
}

QString sysconfig::GlobalConfig::getLocalIpAddress()
{
    return m_localIpAddress;
}

QSize sysconfig::GlobalConfig::getAdjustedSize()
{
    // 获取主屏幕分辨率
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return QSize(800, 600); // 默认尺寸
    }

    QSize resolution = screen->size();
    int width = resolution.width();
    int height = resolution.height();

    // 根据分辨率映射到目标尺寸
    if (width >= 3840 && height >= 2160) { // 4K
        return QSize(2560, 1440);
    }
    else if (width >= 2560 && height >= 1440) { // 2K
        return QSize(1600, 900);
    }
    else if (width >= 1920 && height >= 1080) { // 1080p
        return QSize(1600, 900);
    }
    else if (width >= 1600 && height >= 900) { // 1080p
        return QSize(1280, 720);
    }
    else {
        return QSize(800, 600); // 默认尺寸
    }
}

void sysconfig::GlobalConfig::initLocalIpAddress()
{
    const QList<QHostAddress>& addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress& addr : addresses) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()) {
            m_localIpAddress = addr.toString();
            break;
        }
    }
    if (m_localIpAddress.isEmpty()) {
        m_localIpAddress = "127.0.0.1";
    }
    qDebug() << "本机IP地址:" << m_localIpAddress;
}

// 构造函数初始化默认值
sysconfig::GlobalConfig::GlobalConfig() {
    renderQuality = 100;
    useHardwareAcceleration = true;
    appDataPath = "default/path";  // 可在此初始化其他默认值
	initLocalIpAddress();// 初始化本机IP地址
}