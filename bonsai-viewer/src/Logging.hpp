#pragma once
#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstdint>

/// @brief Available verbosity levels for log messages.
enum class LogVerbosity : uint8_t
{
    Verbose = 0,
    Info    = 1,
    Warning = 2,
    Error   = 3,
};

/// @brief Log a message to the console using printf style formatting.
/// @param Verbosity Log verbosity.
/// @param Fmt Format string.
void LogMessage(LogVerbosity Verbosity, char const* Fmt, ...);

// Log a message using the bonsai logging implementation. Prefer using this over function calls.
#define BONSAI_LOG(Verbosity, ...)  (LogMessage(Verbosity, __VA_ARGS__))

#endif //LOGGING_HPP
