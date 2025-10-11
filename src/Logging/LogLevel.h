//
// Created by Jakub Szwedowicz on 10/10/25.
//

#pragma once
#include <type_traits>

namespace Utils::Logging {
enum class LogLevel : uint8_t { DEBUG, INFO, WARNING, ERROR, CRITICAL, OFF };

inline bool operator<(const LogLevel& left, const LogLevel& right) {
    return static_cast<std::underlying_type_t<LogLevel>>(left) < static_cast<std::underlying_type_t<LogLevel>>(right);
}

inline bool operator==(const LogLevel& left, const LogLevel& right) {
    return static_cast<std::underlying_type_t<LogLevel>>(left) == static_cast<std::underlying_type_t<LogLevel>>(right);
}

}  // namespace Utils::Logger
