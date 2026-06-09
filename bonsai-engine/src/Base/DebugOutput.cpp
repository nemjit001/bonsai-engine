#include "DebugOutput.hpp"

#include <cstdarg>
#include <cstdio>
#include "PlatformDefines.hpp"
#include "PlatformTypes.hpp"

#if     BONSAI_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif  //BONSAI_PLATFORM_WINDOWS

/// @brief Maximum length in bytes for debug string output.
static constexpr usize BONSAI_MAX_DEBUG_STRING_LENGTH = 512u;

void PrintDebugString(char const* fmt, ...)
{
    va_list args{};
    va_start(args, fmt);

    char buffer[BONSAI_MAX_DEBUG_STRING_LENGTH]{};
    std::vsnprintf(buffer, BONSAI_MAX_DEBUG_STRING_LENGTH - 1, fmt, args);
    va_end(args);

#if     BONSAI_PLATFORM_WINDOWS
    OutputDebugString(buffer);
    OutputDebugString("\r\n");
#else
    std::fputs(buffer, stderr);
    std::fputs("\n", stderr);
#endif
}
