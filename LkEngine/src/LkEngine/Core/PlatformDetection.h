#pragma once

#ifdef _WIN32
#ifdef _WIN64
    #ifndef LK_PLATFORM_WINDOWS;
    #define LK_PLATFORM_WINDOWS
    #endif
#else
    #error "x86 Builds are not supported!"
#endif

#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
    #error "Android is not supported"

#elif defined(__linux__)
    #ifndef LK_PLATFORM_LINUX
    #define LK_PLATFORM_LINUX
    #endif
#else
    #error "Unknown platform!"
#endif 

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
