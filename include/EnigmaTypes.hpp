/**
 * @file EnigmaTypes.hpp
 * @brief Common types and interfaces for the EnigmaCore library.
 */

#pragma once

#include <string_view>

/**
 * @brief Semantic type for alphabet indexes [0-25].
 */
using AlphabetIndex = int;

/**
 * @brief Severity levels for logging.
 */
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

/**
 * @brief Interface for platform-specific logging.
 * Decouples the core engine from std::cout/std::cerr.
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    /**
     * @brief Logs a message with a specific severity level.
     * @param level The severity level.
     * @param message The message to log.
     */
    virtual void log(LogLevel level, std::string_view message) = 0;
};
