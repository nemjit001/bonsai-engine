#pragma once
#ifndef ASSERT_HPP
#define ASSERT_HPP

void AssertHandler(char const* expr, char const* file, int line);

#define BONSAI_STATIC_ASSERT(expr)  (static_assert(expr))
#define BONSAI_ASSERT(expr)                         \
    if (!(expr)) {                                  \
        AssertHandler(#expr, __FILE__, __LINE__);   \
    }

#endif //ASSERT_HPP
