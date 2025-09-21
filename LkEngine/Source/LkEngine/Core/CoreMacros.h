/**
 * @file
 * @brief Core macros used by the entire engine.
 */
#pragma once

#include <cassert>
#include <typeinfo>
#include <regex>

/* Needs to be fixed. */
#if defined(LK_ENGINE_STATIC_LIB)
#	define CORE_API
#else
#	if defined(LK_ENGINE_CORE)
#		define CORE_API __declspec(dllexport)
#	else
#		define CORE_API __declspec(dllimport)
#	endif
#endif

#define LK_UNUSED(Arg)       ((void)(Arg))
#define LK_ARRAYSIZE(Array)  (static_cast<int>((sizeof(Array) / sizeof(*(Array))))) 
#define LK_BIT(x)            (1 << x)
#define LK_STRINGIFY(x)      #x
#define LK_TEXT(Text)        L##Text /* Wide string. */

#if defined(LK_ENGINE_MSVC)
#	define LK_FUNCSIG __FUNCSIG__
#   define LK_MALLOC  _malloca
#elif defined(LK_ENGINE_GCC) || defined(LK_ENGINE_CLANG)
#	define LK_FUNCSIG __PRETTY_FUNCTION__
#   define LK_MALLOC  alloca
#else
#	error "Unsupported compiler"
#endif

#if defined(STDCALL)
#	undef STDCALL
#endif

#if defined(VARARGS)
#	undef VARARGS
#endif

#if defined(FORCEINLINE)
#	undef FORCEINLINE
#endif

/**
 * TODO: Platform specific implementations in separate headers
 *       rather than checking the platform here.
 */

/** 
 * @brief Platform: Windows
 */
#if defined(LK_PLATFORM_WINDOWS)
#	define FORCEINLINE __forceinline
#	define VARARGS     __cdecl
#	define STDCALL     __stdcall
/** 
 * @brief Compiler: GCC
 */
#elif defined(LK_ENGINE_GCC)
#	define VARARGS		__attribute__((cdecl))
#	define FORCEINLINE	inline __attribute__((always_inline))
#	define STDCALL		__attribute__((stdcall))
/** 
 * @brief Compiler: Clang
 */
#elif defined(LK_ENGINE_CLANG)
#	define FORCEINLINE  __forceinline
#	define VARARGS      __cdecl
#	define STDCALL      __stdcall
#endif

#if defined(SPDLOG_USE_STD_FORMAT)
#	define LK_FMT_LIB std
#else
#	define LK_FMT_LIB fmt
#endif

#define LK_FMT LK_FMT_LIB::format

#define LK_MARK_FUNC_NOT_IMPLEMENTED(...) \
	LK_CORE_ASSERT(false, "[ FUNCTION NOT IMPLEMENTED ]\n{}" __VA_OPT__("\n{}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))

#define LK_MARK_FUNC_FOR_REMOVAL(...) \
	LK_CORE_ASSERT(false, "[ FUNCTION IS DEPRECATED AND IS TO BE REMOVED ]\n{}" __VA_OPT__("\n\t{}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))

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

#define LK_GET_FIRST_ARG(Arg, ...) Arg

#include "LkEngine/Core/LObject/Enum.h"

namespace LkEngine 
{
	/**
	 * @enum EClassFlag
	 */
	enum class EClassFlag : uint32_t
	{
		None       = LK_BIT(0),
		Abstract   = LK_BIT(1),
	};
	LK_ENUM_CLASS(EClassFlag);

	/**
	 * @enum EClassType
	 * Bitflags for different object types.
	 * 
	 * @todo: Should be renamed to something else that doesn't contain the word 'Class'.
	 */
	enum class EClassType : uint64_t
	{
		LField    = LK_BIT(0),
		LEnum     = LK_BIT(1),
		LStruct   = LK_BIT(2),
		LClass    = LK_BIT(3),
		LObject   = LK_BIT(4),
	};
	LK_ENUM_CLASS(EClassType);

	/**
	 * @enum ELogFormat
	 * Format used on a log message.
	 */
	enum class ELogFormat : uint8_t
	{
		Compact = 0,
		Verbose
	};

	namespace Enum 
	{
		static constexpr const char* ToString(const EClassType ClassType)
		{
			switch (ClassType)
			{
				case EClassType::LField:  return "LField";
				case EClassType::LEnum:   return "LEnum";
				case EClassType::LStruct: return "LStruct";
				case EClassType::LClass:  return "LClass";
				case EClassType::LObject: return "LObject";
			}
			return nullptr;
		}

		static constexpr const char* ToString(const ELogFormat Format)
		{
			switch (Format)
			{
				case ELogFormat::Compact: return "Compact";
				case ELogFormat::Verbose: return "Verbose";
			}
			return nullptr;
		}
	}

	namespace Core::Internal	
	{
		/**
		 * @brief Remove a single character from the start of a string.
		 * @returns The passed string without the prefix if the contained the prefix, else return the same string.
		 */
		FORCEINLINE constexpr const char* RemovePrefix(const char* Str, const char Prefix = 'L')
		{
			return (Str[0] == Prefix) ? Str + 1 : Str;
		}
	}

}

/**
 * @todo: Should do a required/static_assert on the LClass registration to make sure 
 *        a class that inherits LObject in fact do implement the LCLASS macro.
 *        Otherwise there is risk of undefined behaviour because the LClass registration
 *        won't take place.
 */


/**
 * @brief Core Macros
 * @defgroup CoreMacros
 * @ingroup Core
 * @{
 */

/**
 * @def LCLASS
 *
 * Base classes that inherit from LObject are required to be declared as an LCLASS.
 * Adds the static class type to the metadata registry and implements
 * abstract functions and other base functionality from LObject.
 * 
 * @ref LObject
 */
#define LCLASS(Class) \
	public: \
		using BaseClass = LObject; \
		using ThisClass = Class; \
		virtual std::string ClassName() const override { return #Class; } \
		static std::string_view StaticClassName() { return #Class; } \
		static const ::LkEngine::LClass* StaticClass() \
		{ \
			return ::LkEngine::LClass::Get(::LkEngine::LType<Class>::ID()); \
		} \
		virtual const ::LkEngine::LClass* ObjectRegistration() override \
		{ \
			if (!LObject_bRegistered) \
			{ \
				::LkEngine::LClass* ObjectClass = ::LkEngine::LClass::Register<Class>(#Class); \
				if (ObjectClass) \
				{ \
					LObject_bRegistered = true; \
				} \
				return ObjectClass; \
			} \
			return ::LkEngine::LClass::Get(::LkEngine::LType<Class>::ID()); \
		} \
		inline static bool LObject_bRegistered = false; \
		inline static ::LkEngine::EClassType LObject_Type = ::LkEngine::EClassType::LClass;


/**
 * @def LSTRUCT
 *
 * @todo: 
 * - SFINAE on LCLASS/LSTRUCT implementations.
 * - Change 'Class' to 'Struct' in functions.
 * - Struct implementation similar to LClass.
 */
#define LSTRUCT(Struct) \
	public: \
		using BaseClass = LObject; \
		using ThisStruct = Struct; \
		virtual std::string ClassName() const override { return #Struct; } \
		static std::string_view StaticClassName() { return #Struct; } \
		static const ::LkEngine::LClass* StaticClass() \
		{ \
			return ::LkEngine::LClass::Get(::LkEngine::LType<Struct>::ID()); \
		} \
		virtual const ::LkEngine::LClass* ObjectRegistration() override \
		{ \
			if (!LObject_bRegistered) \
			{ \
				::LkEngine::LClass* ObjectClass = ::LkEngine::LClass::Register<Struct>(#Struct); \
				if (ObjectClass) \
				{ \
					LObject_bRegistered = true; \
				} \
				return ObjectClass; \
			} \
			return ::LkEngine::LClass::Get(::LkEngine::LType<Struct>::ID()); \
		} \
		inline static bool LObject_bRegistered = false; \
		inline static ::LkEngine::EClassType LObject_Type = ::LkEngine::EClassType::LStruct;


/** 
 * @def LOBJECT_REGISTER
 * 
 * @todo: 
 * - Determine if to register class or struct (based on LCLASS or LSTRUCT).
 * - The class registration should be re-evaluated, as the call to ObjectRegistration should not be needed.
 *   The initial call to LClass::Get should be enough.
 * @note: Not done.
 */
#define LOBJECT_REGISTER(...) \
		::LkEngine::LClass* ClassObject = const_cast<::LkEngine::LClass*>(::LkEngine::LClass::Get(typeid(this))); \
		if (!ClassObject) \
		{ \
			ClassObject = const_cast<::LkEngine::LClass*>(ObjectRegistration()); \
			if (!IsClassValid()) \
			{ \
				::LkEngine::LObjectBase::SetClass(const_cast<::LkEngine::LClass*>(ClassObject)); \
			} \
		} \


/**
 * @def LASSET
 *
 * LAsset derives from LObject.
 *
 * @note: Not done.
 */
#define LASSET(Class) \
	public: \
		LCLASS(Class); \
		FORCEINLINE virtual bool IsAsset() const override { return true; } \
		template<typename AssetType = Class> \
		std::enable_if<!std::is_same_v<AssetType, LAsset>, EAssetType> \
		FORCEINLINE GetAssetType() const { return GetStaticType(); } \

/** 
 * @def LASSET_REGISTER
 *
 * @note: Not done.
 */
#define LASSET_REGISTER(...) \
		LClass* ClassObject = const_cast<LClass*>(LClass::Get(typeid(this))); \
		if (!ClassObject) \
		{ \
			ClassObject = const_cast<LClass*>(ObjectRegistration()); \
			LObjectBase::SetClass(const_cast<LClass*>(ClassObject)); \
		} \


/** 
 * @def LPANEL
 * 
 * Declaration for UI panels that derive from LPanel.
 * 
 * @note: Mostly a placeholder for now.
 */
#define LPANEL(...) \
			LCLASS(__VA_ARGS__);

/** 
 * @def LPANEL_REGISTER
 * 
 * Registration for UI panels that derive from LPanel.
 * 
 * @note: Mostly a placeholder for now.
 */
#define LPANEL_REGISTER(...) \
			LOBJECT_REGISTER(__VA_ARGS__);

/** @} */

