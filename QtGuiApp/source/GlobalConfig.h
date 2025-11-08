#pragma once
#include <string>
#include <memory>
#include <QSize>

namespace sysconfig {

	class GlobalConfig : public QObject
    {
		Q_OBJECT
    public:

        GlobalConfig(const GlobalConfig&) = delete;
        GlobalConfig& operator=(const GlobalConfig&) = delete;

       
        static GlobalConfig& get();

		
        std::string appDataPath;
        int renderQuality;
        bool useHardwareAcceleration;
        
        QString getLocalIpAddress();
        // 新增：获取调整后的尺寸
        static QSize getAdjustedSize();
    private:
        void initLocalIpAddress();
        GlobalConfig();
    private:
        // 本机IP地址
        QString m_localIpAddress;
    };
}
