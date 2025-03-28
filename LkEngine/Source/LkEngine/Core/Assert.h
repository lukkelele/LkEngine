/******************************************************************
 * Assert
 *
 *******************************************************************/
#pragma once

#include "PlatformDetection.h"

/**
 * Variadic argument support.
 */
#if defined(LK_ENGINE_MSVC) || defined(LK_ENGINE_GCC) || defined(LK_ENGINE_CLANG)
#	define LK_VARIADIC_ARGS  0
#	define LK_ENABLE_ASSERTS 1
#	define LK_ENABLE_VERIFY  1
#endif

/**
 * Asserts.
 */
#if LK_ENABLE_ASSERTS
#	if LK_VARIADIC_ARGS
#		define LK_CORE_ASSERT_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Core,   "Assertion Failed", ##__VA_ARGS__)
#		define LK_ASSERT_MESSAGE_INTERNAL(...)	    ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Client, "Assertion Failed", ##__VA_ARGS__)
#	else
#		define LK_CORE_ASSERT_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Core,   "Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#		define LK_ASSERT_MESSAGE_INTERNAL(...)		::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Client, "Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#	endif
#	define LK_CORE_ASSERT(Condition, ...)                         \
		{												          \
			if (!(Condition))                                     \
			{                                                     \
				if constexpr (sizeof(#__VA_ARGS__) > 1)           \
				{                                                 \
					LK_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
				}                                                 \
				else                                              \
				{                                                 \
					LK_CORE_ASSERT_MESSAGE_INTERNAL("{}", LK_FUNCSIG); \
				}                                                 \
				LK_DEBUG_BREAK;                                   \
			}                                                     \
		}
#	define LK_ASSERT(Condition, ...)                         \
		{                                                    \
			if (!(Condition))                                \
			{                                                \
				if constexpr (sizeof(#__VA_ARGS__) > 1)      \
				{                                            \
					LK_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
				}                                            \
				else                                         \
				{                                            \
					LK_ASSERT_MESSAGE_INTERNAL("{}", LK_FUNCSIG); \
				}                                            \
				LK_DEBUG_BREAK;                              \
			}                                                \
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
#		define LK_CORE_VERIFY_MESSAGE_INTERNAL(...) ::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Core,   "Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#		define LK_VERIFY_MESSAGE_INTERNAL(...)		::LkEngine::LLog::PrintAssertMessage(::LkEngine::ELoggerType::Client, "Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#	endif
#	define LK_CORE_VERIFY(Condition, ...)                              \
		{                                                              \
			if (!(Condition))                                          \
			{                                                          \
				if constexpr (sizeof(#__VA_ARGS__) > 1)                \
				{                                                      \
					LK_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__);      \
				}                                                      \
				else                                                   \
				{                                                      \
					LK_CORE_VERIFY_MESSAGE_INTERNAL("{}", LK_FUNCSIG); \
				}                                                      \
				LK_DEBUG_BREAK;                                        \
			}                                                          \
		}
#	define LK_VERIFY(Condition, ...)                              \
		{                                                         \
			if (!(Condition))                                     \
			{                                                     \
				if constexpr (sizeof(#__VA_ARGS__) > 1)           \
				{                                                 \
					LK_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__);      \
				}                                                 \
				else                                              \
				{                                                 \
					LK_VERIFY_MESSAGE_INTERNAL("{}", LK_FUNCSIG); \
				}                                                 \
				LK_DEBUG_BREAK;                                   \
			}                                                     \
		}
#else
#	define LK_CORE_VERIFY(Condition, ...)
#	define LK_VERIFY(Condition, ...)
#endif
