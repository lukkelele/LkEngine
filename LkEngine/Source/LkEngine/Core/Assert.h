/******************************************************************
 * Assert
 *
 *******************************************************************/
#pragma once

#include "PlatformDetection.h"


/**
 * Variadic argument support.
 */
#if defined(LK_COMPILER_MSC)
#	define LK_VARIADIC_ARGS 1
#	define LK_ENABLE_VERIFY 1
#elif defined(LK_COMPILER_CLANG)
#	define LK_VARIADIC_ARGS 1
#	define LK_ENABLE_VERIFY 1
#else
#	define LK_VARIADIC_ARGS 1
#	define LK_ENABLE_VERIFY 1
#endif

/**
 * Asserts.
 */
#if LK_ENABLE_ASSERTS
#	if LK_VARIADIC_ARGS
#		define LK_CORE_ASSERT_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Core,   "Assertion Failed", ##__VA_ARGS__)
#		define LK_ASSERT_MESSAGE_INTERNAL(...)	    ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Client, "Assertion Failed", ##__VA_ARGS__)
#	else
#		define LK_CORE_ASSERT_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::LLog::Type::Core,   "Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#		define LK_ASSERT_MESSAGE_INTERNAL(...)		::LkEngine::LLog::PrintAssertMessage(::LkEngine::LLog::Type::Client, "Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#	endif
#	define LK_CORE_ASSERT(Condition, ...)                     \
		{                                                     \
			if (!(Condition))                                 \
			{                                                 \
				LK_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
				LK_DEBUG_BREAK;                               \
			}                                                 \
		}
#	define LK_ASSERT(Condition, ...)                     \
		{                                                \
			if (!(Condition))                            \
			{                                            \
				LK_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
				LK_DEBUG_BREAK;                          \
			}                                            \
		}
#else
#	define LK_CORE_ASSERT(Condition, ...)  static_cast<void>(0)
#	define LK_ASSERT(Condition, ...)	   static_cast<void>(0)
#endif

/**
 * Verify.
 */
#if LK_ENABLE_VERIFY
#	if LK_VARIADIC_ARGS
#		define LK_CORE_VERIFY_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Core,   "Verify Failed", ##__VA_ARGS__)
#		define LK_VERIFY_MESSAGE_INTERNAL(...)      ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Client, "Verify Failed", ##__VA_ARGS__)
#	else
#		define LK_CORE_VERIFY_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::LLog::Type::Core,   "Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#		define LK_VERIFY_MESSAGE_INTERNAL(...)		::LkEngine::LLog::PrintAssertMessage(::LkEngine::LLog::Type::Client, "Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#	endif
#	define LK_CORE_VERIFY(Condition, ...)                     \
		{                                                     \
			if (!(Condition))                                 \
			{                                                 \
				LK_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); \
				LK_DEBUG_BREAK;                               \
			}                                                 \
		}
#	define LK_VERIFY(Condition, ...)                     \
		{                                                \
			if (!(Condition))                            \
			{                                            \
				LK_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); \
				LK_DEBUG_BREAK;                          \
			}                                            \
		}
#else
#	define LK_CORE_VERIFY(Condition, ...)
#	define LK_VERIFY(Condition, ...)
#endif
