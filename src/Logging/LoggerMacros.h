#pragma once

#include <spdlog/spdlog.h>

#include "Logger.h"

namespace {
using _Logger = Utils::Logging::Logger;
}

#define SHOULD_LOG(LogLevelValue)                                                    \
    _Logger::getInstance().shouldLog(_Logger::transformFilePathToFilename(__FILE__), \
                                     Utils::Logging::LogLevel::LogLevelValue)

#define LOG(LogLevelValue, ...)                                                                      \
    if (SHOULD_LOG(LogLevelValue)) {                                                                 \
        SPDLOG_LOGGER_CALL(_Logger::getInstance().getSpdlogger(),                                    \
                           _Logger::logLevelToSPDLevelEnum(Utils::Logging::LogLevel::LogLevelValue), \
                           fmt::format(__VA_ARGS__));                                                \
    }

#define LOG_D(...) LOG(DEBUG, __VA_ARGS__)
#define LOG_I(...) LOG(INFO, __VA_ARGS__)
#define LOG_W(...) LOG(WARNING, __VA_ARGS__)
#define LOG_E(...) LOG(ERROR, __VA_ARGS__)
#define LOG_C(...) LOG(CRITICAL, __VA_ARGS__)
