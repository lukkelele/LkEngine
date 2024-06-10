#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifndef LK_PLATFORM_WINDOWS;
    #define LK_PLATFORM_WINDOWS
    #endif
#elif defined(__linux__)
    #ifndef LK_PLATFORM_LINUX
    #define LK_PLATFORM_LINUX
    #endif
#else
    #error "x86 Builds are not supported!"
#endif

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__linux__)
    #error "Unknown platform!"
#endif 


#if defined(LK_PLATFORM_WINDOWS) && defined(LK_PLATFORM_LINUX)
#error "Both Windows and Linux platforms are defined, only one allowed!"
#endif
