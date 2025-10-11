#include "Logger.h"

namespace Utils::Logging {
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::onUpdate(const std::shared_ptr<LoggerConfig>& newConfig) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = newConfig;
}

bool Logger::shouldLog(const std::string& filename, LogLevel level) const {
    std::shared_ptr<LoggerConfig> config;
    {
        std::lock_guard lock(m_mutex);
        config = m_config;
    }

    if (const auto it = config->filesLogLevels.find(filename); it != config->filesLogLevels.end()) {
        return level >= it->second;
    }

    return level >= config->globalLogLevel;
}

std::shared_ptr<spdlog::logger> Logger::getSpdlogger() { return m_logger; }

Logger::Logger() {
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(logLevelToSpdlogLevel(m_config->globalLogLevel));

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_config->filename, true);
    fileSink->set_level(logLevelToSpdlogLevel(m_config->globalLogLevel));

    m_logger = std::make_shared<spdlog::logger>("Logger", spdlog::sinks_init_list{consoleSink, fileSink});
    auto pattern = "[%Y-%m-%d %H:%M:%S.%e] [P%P:T%t] [%^%l%$] [%s:%#] [%n:%!] %v";
    m_logger->set_pattern(pattern);
}

Logger::~Logger() { spdlog::shutdown(); }

spdlog::level::level_enum Logger::logLevelToSpdlogLevel(LogLevel level) {
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

}  // namespace Utils::Logging