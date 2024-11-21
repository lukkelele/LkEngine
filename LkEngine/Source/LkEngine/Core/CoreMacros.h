/******************************************************************
 * CoreMacros
 * 
 *
 *******************************************************************/
#pragma once

#include <typeinfo>
#include <regex>


#define LK_UNUSED(_VAR)         ((void)(_VAR))
#define LK_ARRAYSIZE(_ARR)      (static_cast<int>((sizeof(_ARR) / sizeof(*(_ARR))))) 
#define LK_BIT_FIELD(x)         (1 << x)
#define LK_STRINGIFY(x)         #x
#define LK_TEXT(_TEXT)			L##_TEXT /* Wide string literal. */

/** Function Signature. */
#ifdef __clang__
#	define LK_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#	define LK_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#	define LK_FUNCSIG __FUNCSIG__
#else
#	error "Unsupported compiler"
#endif

/// @TODO: Do Platform implementations in separate headers for Windows/Linux
#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

/** 
 * Platform: Windows
 */
#if defined(LK_PLATFORM_WINDOWS)
#	define STDCALL					__stdcall
#	define VARARGS					__cdecl
#	define FORCEINLINE				__forceinline
#	define WARNINGS_DISABLE()		__pragma(warning(push, 0))
#	define WARNINGS_ENABLE()		__pragma(warning(pop))
#	define LK_ITOA(c, buf, size, p)	_itoa_s(c, buf, size, p)
/** 
 * Platform: Linux
 */
#elif defined(LK_PLATFORM_LINUX)
#	define STDCALL		__stdcall
#	define VARARGS		__cdecl
#	define FORCEINLINE	__forceinline
#	define LK_ITOA(c, buf, size, p)  void() // FIXME
#	define WARNINGS_DISABLE() \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"") \
        _Pragma("GCC diagnostic ignored \"-Wextra\"") \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#	define WARNINGS_ENABLE() _Pragma("GCC diagnostic pop")
#endif

/* Log formatter. */
#ifdef SPDLOG_USE_STD_FORMAT
#	define LK_FMT_LIB  std
#else
#	define LK_FMT_LIB  spdlog::fmt_lib
#endif
#define LK_FORMAT_STRING(...)  LK_FMT_LIB::format(__VA_ARGS__)

/* Mark a function 'Not implemented'. */
#define LK_MARK_FUNC_NOT_IMPLEMENTED(...) \
	LK_CORE_ASSERT(false, "[ Function not implemented ]\n{}" __VA_OPT__("\nDev note: {}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))

#if defined(_MSC_VER)
#	define LK_ASSUME(x)  __assume(x)
#	define LK_STRUCT_OFFSET(InStruct, InMember)  offsetof(InStruct, InMember)
#elif defined(__clang__)
#	define LK_ASSUME(x)  __builtin_assume(x)
#	define LK_STRUCT_OFFSET(InStruct, InMember)  __builtin_offsetof(InStruct, InMember)
#else
#	define LK_ASSUME(x)
#	define LK_STRUCT_OFFSET(InStruct, InMember)
#endif

/* Branch prediction hints. */
#ifndef LK_LIKELY
#	if (defined(__clang__) || defined(__GNUC__)) && LK_PLATFORM_LINUX
#		define LK_LIKELY(x)	 __builtin_expect(!!(x), 1)
#	else
#		define LK_LIKELY(x)  (!!(x))
#	endif
#endif

#ifndef LK_UNLIKELY
#	if (defined(__clang__) || defined(__GNUC__)) && LK_PLATFORM_LINUX
#		define LK_UNLIKELY(x)  __builtin_expect(!!(x), 0)
#	else
#		define LK_UNLIKELY(x)  (!!(x))
#	endif
#endif

namespace LkEngine {

	enum class EClassFlag : uint32_t
	{
		None       = LK_BIT_FIELD(0),
		Abstract   = LK_BIT_FIELD(1),
	};

	enum class EClassCastFlag : uint64_t
	{
		LField    = LK_BIT_FIELD(0),
		LEnum     = LK_BIT_FIELD(1),
		LStruct   = LK_BIT_FIELD(2),
		LClass    = LK_BIT_FIELD(3),
		LObject   = LK_BIT_FIELD(4),
	};

}

/* TODO: Should do a required/static_assert on the LClass registration to make sure 
 *       a class that inherits LObject in fact do implement the LCLASS macro.
 *       Otherwise there is risk of undefined behaviour because the LClass registration
 *       won't take place.
 */
/**
 * LCLASS
 *
 *  Base classes that inherit from LObject are required to be declared as an LCLASS.
 *  Adds the static class type to the metadata registry and implements
 *  abstract functions and other base functionality from LObject.
 */
#define LCLASS(Class) \
	public: \
		using BaseClass = LObject; \
		using ThisClass = Class; \
		virtual std::string ClassName() const override { return #Class; } \
		static std::string_view StaticClassName() { return #Class; } \
		static const LClass* StaticClass() \
		{ \
			return LClass::Get(LType<Class>::ID()); \
		} \
		virtual const LClass* ClassRegistration() override \
		{ \
			LClass* ObjectClass = LClass::Register<Class>(#Class); \
			return ObjectClass; \
		} \


/** 
 * LCLASS_REGISTER
 */
#define LCLASS_REGISTER(...) \
		LClass* ClassObject = const_cast<LClass*>(LClass::Get(typeid(this))); \
		if (!ClassObject) \
		{ \
			ClassObject = const_cast<LClass*>(ClassRegistration()); \
			LObjectBase::SetClass(const_cast<LClass*>(ClassObject)); \
		} \


/// FIXME: LASSET macro is not done/implemented yet.
/**
 * LASSET
 *
 *  LAsset derives from LObject.
 *  FIXME: In progress
 */
#define LASSET(Class) \
	public: \
		LCLASS(Class); \
		FORCEINLINE virtual bool IsAsset() const override { return true; } \
		template<typename AssetType = Class> \
		std::enable_if<!std::is_same_v<AssetType, LAsset>, EAssetType> \
		FORCEINLINE GetAssetType() const { return GetStaticType(); } \

/** 
 * LASSET_REGISTER
 */
#define LASSET_REGISTER(...) \
		LClass* ClassObject = const_cast<LClass*>(LClass::Get(typeid(this))); \
		if (!ClassObject) \
		{ \
			ClassObject = const_cast<LClass*>(ClassRegistration()); \
			LObjectBase::SetClass(const_cast<LClass*>(ClassObject)); \
		} \


