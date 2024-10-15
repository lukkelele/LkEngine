#pragma once

#if defined(_WIN32) || defined(_WIN64)
#	ifndef LK_PLATFORM_WINDOWS
#	define LK_PLATFORM_WINDOWS
#	endif
#elif defined(__linux__)
#   ifndef LK_PLATFORM_LINUX
#   define LK_PLATFORM_LINUX
#   endif
#else
#   error "Platform not supported"
#endif

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__linux__)
#   error "Unknown platform!"
#endif 

#if defined(LK_PLATFORM_WINDOWS) && defined(LK_PLATFORM_LINUX)
#   error "Both Windows and Linux platforms are defined, only one allowed!"
#endif

#if defined(_MSC_VER)
#   ifndef LK_COMPILER_MSC
#	define LK_COMPILER_MSC
#	endif
#	define LK_DEBUG_BREAK __debugbreak()
#elif defined(__clang__)
#   ifndef LK_COMPILER_CLANG
#	define LK_COMPILER_CLANG
#	endif
#	define LK_DEBUG_BREAK __builtin_debugtrap()
#elif defined(__GNUC__)
#   ifndef LK_COMPILER_GNU
#	define LK_COMPILER_GNU
#	endif
#	define LK_DEBUG_BREAK
#endif


#ifdef LK_DEBUG
#	define LK_ENABLE_ASSERTS 1
#else
#	define LK_ENABLE_ASSERTS 0
#endif
