#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "LoggerConfig.h"

namespace spdlog {
class logger;
namespace sinks {
class sink;
}
}

namespace Utils::Logging {

class Logger {
   public:
    // Constructors
    explicit Logger(std::string name, std::shared_ptr<LoggerConfig> config = nullptr);
    
    // Methods
    static Logger& getInstance();

    const std::string& getName() const;

    void onUpdate(const std::shared_ptr<LoggerConfig>& newConfig);

    template <LogLevel Level>
    void log(std::string_view message);

    void flush();

    void addSink(std::shared_ptr<spdlog::sinks::sink> sink);

    void clearSinks();



   private:
    // Private Methods
    void updateLoggerLevel();
    
    static std::shared_ptr<spdlog::logger> buildLogger(const std::string& name, const std::shared_ptr<LoggerConfig>& config);

   private:
    // Members
    std::string m_name;
    mutable std::mutex m_mutex;
    std::shared_ptr<LoggerConfig> m_config = std::make_shared<LoggerConfig>();
    
    // Pimpl: Forward declared in namespace. Const as it's always initialized.
    const std::shared_ptr<spdlog::logger> m_logger; 
};

}  // namespace Utils::Logging