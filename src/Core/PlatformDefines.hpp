#pragma once
#ifndef BONSAI_PLATFORM_DEFINES_HPP
#define BONSAI_PLATFORM_DEFINES_HPP

#ifndef NDEBUG
    #define BONSAI_BUILD_TYPE_DEBUG     1
    #define BONSAI_BUILD_TYPE_RELEASE   0
#else
    #define BONSAI_BUILD_TYPE_DEBUG     0
    #define BONSAI_BUILD_TYPE_RELEASE   1
#endif //NDEBUG

#if     defined(_WIN32)
    #define BONSAI_PLATFORM_WINDOWS     1
#elif   defined(__unix__)
    #define BONSAI_PLATFORM_LINUX       1
#endif

#endif //BONSAI_PLATFORM_DEFINES_HPP
