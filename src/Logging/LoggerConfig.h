//
// Created by Jakub Szwedowicz on 10/10/25.
//

#pragma once

#include <string>
#include <unordered_map>

#include "LogLevel.h"

namespace Utils::Logging {

struct LoggerConfig {
    std::string filename = "mainLog.txt";
    LogLevel globalLogLevel = LogLevel::INFO;
    std::unordered_map<std::string, LogLevel> loggersLogLevels;
};

}  // namespace Utils::Logger
