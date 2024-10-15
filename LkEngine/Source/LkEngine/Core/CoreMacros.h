/******************************************************************
 * CoreMacros
 * 
 *
 *******************************************************************/
#pragma once

#include <typeinfo>
#include <regex>

#include "LkEngine/Core/Memory/MemoryPtr.h"


#define LK_UNUSED(_VAR)         ((void)(_VAR))
#define LK_ARRAYSIZE(_ARR)      (static_cast<int>((sizeof(_ARR) / sizeof(*(_ARR))))) 
#define BIT_FIELD(x)            (1 << x)
#define STRINGIFY(x)            #x

/* Disable warnings, push and pop. */
#define LK_DISABLE_WARN_BEGIN     _Pragma("warning(push, 0)")
#define LK_DISABLE_WARN_END       _Pragma("warning(pop)")

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
/** Windows Platform */
#if defined(LK_PLATFORM_WINDOWS)
	#define STDCALL __stdcall
	#define VARARGS __cdecl

	/* Force Inline */
	#ifdef FORCEINLINE
	#undef FORCEINLINE
	#define FORCEINLINE __forceinline
	#endif
	#define FORCEINLINE __forceinline

	#define LK_ITOA(c, buf, size, p)  _itoa_s(c, buf, size, p)
#endif

/** Linux Platform */
#if defined(LK_PLATFORM_LINUX)
	#define STDCALL __stdcall
	#define VARARGS __cdecl
	#define FORCEINLINE __forceinline
	#define LK_ITOA(c, buf, size, p)  void() // FIXME
#endif

/* Mark a function 'Not implemented'. */
#define LK_MARK_FUNC_NOT_IMPLEMENTED(...) \
	LK_CORE_ASSERT(false, "Not implemented - {}" __VA_OPT__("\nNote: {}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))
	//LK_CORE_ASSERT(false, "Not implemented - {}\nNote: {}", LK_FUNCSIG, __VA_ARGS__)

namespace LkEngine 
{
	template <typename T>
	FORCEINLINE T ImplicitConv(std::type_identity_t<T> Object)
	{
		return Object;
	}
}

#if 0
inline static std::string CleanClassName(const char* InClassName)
{
	std::string CleanName = InClassName;

	static const std::string Keyword_Class   = "class ";
	static const std::string Keyword_Struct  = "struct ";
	static const std::string Suffix_Pointer  = "__ptr";
	static const std::string Suffix_PointerSymbol = "*";

	/* Remove keywords like class and struct. */
	if (CleanName.find(Keyword_Class) == 0)
	{
		CleanName = CleanName.substr(Keyword_Class.length());
	}
	else if (CleanName.find(Keyword_Struct) == 0)
	{
		CleanName = CleanName.substr(Keyword_Struct.length());
	}

	/* Find and remove namespaces(keep only the class name). */
	const size_t Position = CleanName.find_last_of("::");
	if (Position != std::string::npos)
	{
		CleanName = CleanName.substr(Position + 1);  // Extract class name after the last "::"
	}

	/* Remove any "__ptrXX" entries(e.g., __ptr64, __ptr32). */
    std::regex Regex_Pointer("__ptr[0-9]+");
    CleanName = std::regex_replace(CleanName, Regex_Pointer, "");

	auto RemoveSuffixPredicate = [](const char InCharacter)
	{
		return InCharacter == '*' 
			|| InCharacter == ' ' 
			|| InCharacter == '_';
	};

	/* Remove pointer suffixes. */
    CleanName.erase(std::remove_if(CleanName.begin(), CleanName.end(), RemoveSuffixPredicate), CleanName.end());

    return CleanName;
}
#endif

/**
 * LCLASS
 * 
 *  Base classes that inherit from LObject are required to be declared as an LCLASS.
 *
 *  Adds the static class type to the metadata registry and implements 
 *  abstract functions and other base functionality from LObject.
 */
#define LCLASS(ClassName) \
	public: \
		using BaseClass = LObject; \
		virtual std::string GetStaticClass() const override { return #ClassName; } \
		static std::string StaticClass() { return #ClassName; } \
	private: \
		void LK_META_CLASS_REGISTER_FUNC() \
		{ \
			if constexpr (!std::is_same_v<LkEngine::LObject, LkEngine::##ClassName>) \
			{ \
				LMetadataRegistry::Get().Register(#ClassName, this); \
			} \
		}

/// FIXME: LASSET macro is not done/implemented yet.
/**
 * LASSET
 *
 *  FIXME: In progress
 *
 *  LAsset derives from LObject.
 */
#define LASSET(ClassName) \
	public: \
		template<typename TAsset = ClassName> \
		std::enable_if<!std::is_same_v<TAsset, LAsset>, EAssetType> \
		FORCEINLINE GetAssetType() const { return GetStaticType(); } \
		\
		LCLASS(ClassName); \

/*
/// TO ADD
    template <typename T>
    static void EnsureBaseInheritance() 
    {
        static_assert(std::is_base_of<Base, T>::value, "T must inherit from Base");
    }
*/

/**
 * Register class.
 *
 * TODO: This function should always get called for every LCLASS occurence.
 *       Implement some way to force this behaviour.
 */
#define LCLASS_REGISTER() \
	{ \
		const char* ClassName = typeid(this).name(); \
		static bool LK_ObjectRegistration = [&]() \
		{ \
			LMetadataRegistry::Get().Register((*this).GetStaticClass(), this); \
			return true; \
		}(); \
		assert(LK_ObjectRegistration && "LCLASS_REGISTER failed"); \
	}
