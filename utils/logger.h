#pragma once

#include <sstream>

namespace pf
{

class Logger
{
public:
    virtual ~Logger() { }

    enum LogTarget {
        eLogNone = 0,
        eLogFile = 1,
        eLogSys = 2,
        eLogMSVC = 4
    };

    enum LogLevel {
        eLogTrace = 0,
        eLogDebug = 1,
        eLogInfo = 2,
        eLogWarn = 3,
        eLogError = 4,
        eLogCritical = 5,
        eLogOff = 6
    };

    static Logger& i();
    
    void init(int target = eLogFile | eLogSys,
              int level = eLogDebug,
              const std::string& path = "",
              const std::string& tagName = "picture_fun");

    void logMessage(int level, const std::string& message);

    void shutdown();

    std::string defaultFolder() const {
        return logFolder_;
    }

    std::string logFilePath() const {
        return logFilePath_;
    }

protected:
    Logger() : inited_(false) {
    }

    bool inited_;
    std::string logFolder_;
    std::string logFilePath_;
};

}


#define LOG_MESSAGE(level, x) \
    do { \
        std::stringstream out; \
        out << x; \
        pf::Logger::i().logMessage(level, out.str()); \
    } while(0); \


#define LOG_INFO(x) LOG_MESSAGE(pf::Logger::eLogInfo, x)
#define LOG_DETAIL(x) LOG_MESSAGE(pf::Logger::eLogDebug, x)
#define LOG_WARN(x) LOG_MESSAGE(pf::Logger::eLogWarn, x)
#define LOG_ERROR(x) LOG_MESSAGE(pf::Logger::eLogError, x)
#define LOG_NOTICE(x) LOG_MESSAGE(pf::Logger::eLogCritical, x)
