#include "utils/logger.h"
#include "utils/system.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/async.h"

using namespace pf;

#if defined(ANDROID)
#include "spdlog/sinks/android_sink.h"
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/core/utils/filesystem.hpp>

Logger& Logger::i()
{
    static pf::Logger logger;

    return logger;
}

void Logger::shutdown()
{
    inited_ = false;
    spdlog::shutdown();
}

void Logger::init(int target, int level, const std::string& path, const std::string& tagName)
{
    if (inited_)
        return;
    
    spdlog::init_thread_pool(8192, 1);
    std::vector<spdlog::sink_ptr> sinks;
    
    std::string outFolder;
    if (target & eLogFile)
    {
        if (path.find("\\") != std::string::npos || path.find("/") != std::string::npos)
        {
            outFolder = path;
            if (!cv::utils::fs::exists(outFolder))
                cv::utils::fs::createDirectory(outFolder);
        }
        else
        {
#if defined(_WINDOWS) || defined(LINUX)
            std::string defaultFolder = getDefaultFolder() + "/pf";
            if (!cv::utils::fs::exists(defaultFolder))
                cv::utils::fs::createDirectory(defaultFolder);
            defaultFolder = defaultFolder + "/Logs/";
            if (!cv::utils::fs::exists(defaultFolder))
                cv::utils::fs::createDirectory(defaultFolder);
#elif defined(__APPLE__)
            std::string defaultFolder = getDefaultFolder() + "/Logs/";
#else
            std::string defaultFolder = getDefaultFolder() + "/";
#endif
            outFolder = defaultFolder + (path.empty() ? "pf" : path);
            if (!cv::utils::fs::exists(outFolder))
                cv::utils::fs::createDirectory(outFolder);
        }
        logFolder_ = outFolder;
        outFolder += "/" + tagName + ".log";
        logFilePath_ = outFolder;
        
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(outFolder, 1024*1024*8, 8);
        sinks.push_back(rotating_sink);
    }
    
    if (target & eLogSys)
    {
#if defined(_WINDOWS)
        auto sys_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#elif defined(ANDROID)
        auto sys_sink = std::make_shared<spdlog::sinks::android_sink_mt>(tagName);
#else
        auto sys_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
#endif
        sinks.push_back(sys_sink);
    }

    if (target & eLogMSVC) {
#if defined(_WINDOWS)
        auto sys_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        sinks.push_back(sys_sink);
#endif
    }
    
    auto logger = std::make_shared<spdlog::async_logger>("logger",
                                                         sinks.begin(),
                                                         sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    //spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
    spdlog::set_pattern("[%m-%d %H:%M:%S.%e] %^[%l]%$ [tid=%t pid=%P] %v");
    spdlog::flush_on(static_cast<spdlog::level::level_enum>(level));
    
    spdlog::info("initialized logger ...");
    if (target & eLogFile)
        spdlog::info("File logging was enabled, log was saved at: {}", outFolder);
    inited_ = true;
}

void Logger::logMessage(int level, const std::string& message)
{
    spdlog::log(static_cast<spdlog::level::level_enum>(level), message.c_str());
}
