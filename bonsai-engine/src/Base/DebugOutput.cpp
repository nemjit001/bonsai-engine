#include "DebugOutput.hpp"

#include <cstdarg>
#include <cstdio>

void PrintDebugString(char const* fmt, ...)
{
    va_list args{};
    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);
    std::puts("");
}
