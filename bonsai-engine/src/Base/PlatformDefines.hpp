#ifndef PLATFORM_DEFINES_HPP
#define PLATFORM_DEFINES_HPP

#if     _WIN32
    #define BONSAI_PLATFORM_WINDOWS 1
#elif   __unix__
    #define BONSAI_PLATFORM_LINUX   1
#endif

#if     _MSC_VER
    #define BONSAI_COMPILER_MSVC    1
#elif   __GNUC__
    #define BONSAI_COMPILER_GCC     1
#endif

#if     __clang__
    #define BONSAI_COMPILER_CLANG   1
#endif

#endif //PLATFORM_DEFINES_HPP
