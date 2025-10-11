#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "LoggerConfig.h"

namespace Utils::Logging {
class Logger {
   public:
    static Logger& getInstance();

    void onUpdate(const std::shared_ptr<LoggerConfig>& newConfig);
    static consteval const char* transformFilePathToFilename(const char* filepath) {
        const char* last = filepath;
        for (const char* p = filepath; *p; ++p) {
            if (*p == '/' || *p == '\\') last = p + 1;
        }
        return last;
    }
    static constexpr spdlog::level::level_enum logLevelToSPDLevelEnum(const LogLevel& level) {
        switch (level) {
            case LogLevel::DEBUG:
                return spdlog::level::debug;
            case LogLevel::INFO:
                return spdlog::level::info;
            case LogLevel::WARNING:
                return spdlog::level::warn;
            case LogLevel::ERROR:
                return spdlog::level::err;
            case LogLevel::CRITICAL:
                return spdlog::level::critical;
            default:
                return spdlog::level::info;
        }
    }
    bool shouldLog(const std::string& filename, LogLevel level) const;
    std::shared_ptr<spdlog::logger> getSpdlogger();

   private:
    Logger();
    ~Logger();

    static spdlog::level::level_enum logLevelToSpdlogLevel(LogLevel level);

   private:
    mutable std::mutex m_mutex;
    std::shared_ptr<LoggerConfig> m_config = std::make_shared<LoggerConfig>();
    std::shared_ptr<spdlog::logger> m_logger;
};

}  // namespace Utils::Logging