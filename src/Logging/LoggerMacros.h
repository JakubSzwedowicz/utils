#pragma once

#include <spdlog/fmt/fmt.h>

#include "Logger.h"

namespace {
using _Logger = Utils::Logging::Logger;
}

#define LOG(LogLevelValue, ...) m_logger.log<Utils::Logging::LogLevel::LogLevelValue>(fmt::format(__VA_ARGS__))

#define LOG_D(...) LOG(DEBUG, __VA_ARGS__)
#define LOG_I(...) LOG(INFO, __VA_ARGS__)
#define LOG_W(...) LOG(WARNING, __VA_ARGS__)
#define LOG_E(...) LOG(ERROR, __VA_ARGS__)
#define LOG_C(...) LOG(CRITICAL, __VA_ARGS__)
