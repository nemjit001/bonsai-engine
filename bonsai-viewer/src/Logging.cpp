#include "Logging.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstring>

void LogMessage(LogVerbosity, char const* Fmt, ...)
{
    va_list VarArgs;
    va_start(VarArgs, Fmt);
    vprintf(Fmt, VarArgs);
    va_end(VarArgs);
    puts(""); // Writes newline to stdout
}
