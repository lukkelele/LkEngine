#pragma once

//#include "Base.h"
//#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Log.h"


#ifdef LK_PLATFORM_WINDOWS
	#define LK_DEBUG_BREAK __debugbreak()
#elif defined(LK_COMPILER_CLANG)
	#define LK_DEBUG_BREAK __builtin_debugtrap()
#else
	#define LK_DEBUG_BREAK
#endif

#ifdef LK_DEBUG
	#define LK_ENABLE_ASSERTS
#endif

#define LK_ENABLE_VERIFY
#define LK_VARIADIC_OPT_DISABLED

#ifdef LK_ENABLE_ASSERTS
	// Internal helpers, do not use directly
	#if defined(LK_COMPILER_CLANG) || defined(LK_VARIADIC_OPT_DISABLED)
		#define LK_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::EType::Core, "Assertion Failed", ##__VA_ARGS__)
		#define LK_ASSERT_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::EType::Client, "Assertion Failed", ##__VA_ARGS__)
	#else
		#define LK_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::Type::Core, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
		#define LK_ASSERT_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::Type::Client, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
	#endif

	#define LK_CORE_ASSERT(condition, ...) { if(!(condition)) { LK_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); LK_DEBUG_BREAK; } }
	#define LK_ASSERT(condition, ...) { if(!(condition)) { LK_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); LK_DEBUG_BREAK; } }
#else
	#define LK_CORE_ASSERT(condition, ...)
	#define LK_ASSERT(condition, ...)
#endif

#define LK_CORE_THROW_ERROR(...) LK_CORE_ASSERT(false, __VA_ARGS__)
#define LK_THROW_ERROR(...)      LK_ASSERT(false, __VA_ARGS__)


#ifdef LK_ENABLE_VERIFY
	// Internal helpers, do not use directly
	#if defined(LK_COMPILER_CLANG) || defined(LK_VARIADIC_OPT_DISABLED)
		#define LK_CORE_VERIFY_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::EType::Core, "Verify Failed", ##__VA_ARGS__)
		#define LK_VERIFY_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::EType::Client, "Verify Failed", ##__VA_ARGS__)
	#else
		#define LK_CORE_VERIFY_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::Type::Core, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
		#define LK_VERIFY_MESSAGE_INTERNAL(...)  ::LkEngine::Log::PrintAssertMessage(::LkEngine::Log::Type::Client, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
	#endif

	#define LK_CORE_VERIFY(condition, ...) { if (!(condition)) { LK_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); LK_DEBUG_BREAK; } }
	#define LK_VERIFY(condition, ...) { if (!(condition)) { LK_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); LK_DEBUG_BREAK; } }

#else
	#define LK_CORE_VERIFY(condition, ...)
	#define LK_VERIFY(condition, ...)
#endif
