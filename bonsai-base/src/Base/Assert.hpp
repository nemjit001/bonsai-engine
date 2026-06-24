#pragma once
#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "PlatformDefines.hpp"

void AssertHandler(char const* expr, char const* file, int line);

#if     BONSAI_COMPILER_MSVC
    #define BONSAI_DEBUG_BREAK()    __debugbreak()
#elif   BONSAI_COMPILER_GCC && BONSAI_COMPILER_CLANG && __has_builtin(__builtin_debugtrap)
    #define BONSAI_DEBUG_BREAK()    __builtin_debugtrap()
#elif   BONSAI_COMPILER_GCC
    #define BONSAI_DEBUG_BREAK()    __builtin_trap()
#else
    #define BONSAI_DEBUG_BREAK()
#endif

#define BONSAI_ASSERT(expr) (!!(expr) || (AssertHandler(#expr, __FILE__, __LINE__), BONSAI_DEBUG_BREAK(), 0))

#endif //ASSERT_HPP
