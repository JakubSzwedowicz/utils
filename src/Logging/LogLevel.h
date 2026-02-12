//
// Created by Jakub Szwedowicz on 10/10/25.
//

#pragma once
#include <cstdint>

namespace Utils::Logging {
enum class LogLevel : uint8_t { DEBUG, INFO, WARNING, ERROR, CRITICAL, OFF, _COUNT };

}  // namespace Utils::Logging
