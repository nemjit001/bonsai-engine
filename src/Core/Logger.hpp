#pragma once
#ifndef BONSAI_LOGGER_HPP
#define BONSAI_LOGGER_HPP

// Determine compile time log level based on build type
#include "Core/PlatformDefines.hpp"
#if     BONSAI_BUILD_TYPE_DEBUG
    #define SPDLOG_ACTIVE_LEVEL TRACE
#elif   BONSAI_BUILD_TYPE_RELEASE
    #define SPDLOG_ACTIVE_LEVEL INFO
#endif

#include <spdlog/spdlog.h>

namespace Bonsai
{
    /// @brief The Logger class contains logging helpers.
    class Logger
    {
    public:
        /// @brief Default log level for Bonsai.
        static constexpr spdlog::level::level_enum DEFAULT_LOG_LEVEL = spdlog::level::trace;
    };
} // namespace Bonsai


#endif //BONSAI_LOGGER_HPP
