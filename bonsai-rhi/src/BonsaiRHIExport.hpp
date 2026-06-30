#pragma once
#ifndef BONSAI_RHI_EXPORT_HPP
#define BONSAI_RHI_EXPORT_HPP

#include <BonsaiBase.hpp>

#if     BONSAI_PLATFORM_WINDOWS
    #define BONSAI_RHI_APICALL          __stdcall
    #if     BONSAI_RHI_EXPORT_SYMBOLS
        #define BONSAI_RHI_EXPORT       __declspec(dllexport)
        #define BONSAI_RHI_NO_EXPORT
    #else
        #define BONSAI_RHI_EXPORT       __declspec(dllimport)
        #define BONSAI_RHI_NO_EXPORT
    #endif //BONSAI_RHI_EXPORT_SYMBOLS
#elif   BONSAI_PLATFORM_LINUX
    #define BONSAI_RHI_APICALL
    #if     BONSAI_RHI_EXPORT_SYMBOLS
        #define BONSAI_RHI_EXPORT       __attribute((visibility("default")))
        #define BONSAI_RHI_NO_EXPORT    __attribute((visibility("hidden")))
    #else
        #define BONSAI_RHI_EXPORT
        #define BONSAI_RHI_NO_EXPORT
    #endif //BONSAI_RHI_EXPORT_SYMBOLS
#else
    #define BONSAI_RHI_APICALL
    #define BONSAI_RHI_EXPORT
    #define BONSAI_RHI_NO_EXPORT
#endif

#endif //BONSAI_RHI_EXPORT_HPP
