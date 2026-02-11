#include "Logger.h"

#include <csignal>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace Utils::Logging {

constexpr spdlog::level::level_enum logLevelToSpdlogImpl(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return spdlog::level::debug;
        case LogLevel::INFO: return spdlog::level::info;
        case LogLevel::WARNING: return spdlog::level::warn;
        case LogLevel::ERROR: return spdlog::level::err;
        case LogLevel::CRITICAL: return spdlog::level::critical;
        case LogLevel::OFF: return spdlog::level::off;
        default: return spdlog::level::info;
    }
}

consteval spdlog::level::level_enum logLevelToSpdlog(LogLevel level) {
    return logLevelToSpdlogImpl(level);
}

Logger::Logger(std::string name, std::shared_ptr<LoggerConfig> config) 
    : m_name(std::move(name)), 
      m_config(config ? config : std::make_shared<LoggerConfig>()),
      m_logger(buildLogger(m_name, m_config)) {
    
    updateLoggerLevel();
}

Logger& Logger::getInstance() {
    static Logger instance("Root");
    return instance;
}

const std::string& Logger::getName() const {
    return m_name;
}

void Logger::onUpdate(const std::shared_ptr<LoggerConfig>& newConfig) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = newConfig;
    updateLoggerLevel();
}

template <LogLevel Level>
void Logger::log(std::string_view message) {
    // Compile-time conversion of Level to spdlog level
    m_logger->log(logLevelToSpdlog(Level), message);
}

void Logger::flush() {
    m_logger->flush();
}

void Logger::addSink(std::shared_ptr<spdlog::sinks::sink> sink) {
    if (!sink) return;
    m_logger->sinks().push_back(sink);
    // Ensure the new sink captures everything allowed by the logger
    sink->set_level(spdlog::level::trace); 
}

void Logger::clearSinks() {
    m_logger->sinks().clear();
}

void Logger::updateLoggerLevel() {
    LogLevel threshold = m_config->globalLogLevel;
    
    if (const auto it = m_config->loggersLogLevels.find(m_name); it != m_config->loggersLogLevels.end()) {
        threshold = it->second;
    }

    m_logger->set_level(logLevelToSpdlogImpl(threshold));
}

std::shared_ptr<spdlog::logger> Logger::buildLogger(const std::string& name, const std::shared_ptr<LoggerConfig>& config) {
    // Lifecycle Manager ensures spdlog::shutdown is called on exit or signal
    struct LifecycleManager {
        LifecycleManager() {
            // Register cleanup on normal exit
            std::atexit([]() { spdlog::shutdown(); });
            
            // Register signal handlers
            auto handler = [](int sig) {
                spdlog::shutdown();
                // Restore default handler and raise signal again to terminate
                std::signal(sig, SIG_DFL);
                std::raise(sig);
            };
            
            std::signal(SIGINT, handler);
            std::signal(SIGTERM, handler);
        }
    };
    
    static LifecycleManager s_lifecycleManager;

    // We set sinks to TRACE so they accept everything; m_logger filters based on its level
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(config->filename, true);
    fileSink->set_level(spdlog::level::trace);

    auto logger = std::make_shared<spdlog::logger>(name, spdlog::sinks_init_list{consoleSink, fileSink});
    auto pattern = "[%Y-%m-%d %H:%M:%S.%e] [P%P:T%t] [%^%l%$] [%s:%#] [%n:%!] %v";
    logger->set_pattern(pattern);
    
    return logger;
}

// Explicit Instantiations
template void Logger::log<LogLevel::DEBUG>(std::string_view);
template void Logger::log<LogLevel::INFO>(std::string_view);
template void Logger::log<LogLevel::WARNING>(std::string_view);
template void Logger::log<LogLevel::ERROR>(std::string_view);
template void Logger::log<LogLevel::CRITICAL>(std::string_view);
template void Logger::log<LogLevel::OFF>(std::string_view);

}  // namespace Utils::Logging