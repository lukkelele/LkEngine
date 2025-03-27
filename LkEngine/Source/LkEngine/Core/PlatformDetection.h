/**
 * @file
 * @brief Platform detection
 */
#pragma once

#if defined(LK_ENGINE_MSVC)
#	define LK_DEBUG_BREAK __debugbreak()
#elif defined(LK_ENGINE_CLANG)
#	define LK_DEBUG_BREAK __builtin_debugtrap()
#elif defined(LK_ENGINE_GCC)
#	define LK_DEBUG_BREAK __builtin_trap()
#endif

/** Character Encoding. */
#if defined(LK_CHAR_UTF8) && defined(LK_CHAR_UNICODE)
#	error "Too many encoding formats defined, only one allowed"
#endif

/* Set the encoding to UTF8 if undefined. */
#if !defined(LK_CHAR_UTF8) && !defined(LK_CHAR_UNICODE)
#	define LK_CHAR_UTF8
#endif

#if defined(LK_CHAR_UTF8) 
#	define LK_TCHAR char
#elif defined(LK_CHAR_UNICODE)
#	define LK_TCHAR wchar_t
#endif
