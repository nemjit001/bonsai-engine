#include "Assert.hpp"

#include "DebugOutput.hpp"

void AssertHandler(char const* expr, char const* file, int line)
{
    PrintDebugString("Assertion failed: %s @ %s:%d", expr, file, line);
}
