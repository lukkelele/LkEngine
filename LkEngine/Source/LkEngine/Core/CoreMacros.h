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
	LK_CORE_ASSERT(false, "[ Function not implemented ]\n{}" __VA_OPT__("\nDev note: {}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))

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
		static std::string_view StaticClass() { return #Class; } \
		virtual std::string_view ClassName() const override { return StaticClass(); } \
	private: \
		void LK_META_CLASS_REGISTER_FUNC() \
		{ \
			if constexpr (!std::is_same_v<::LkEngine::LObject, ::LkEngine::Class>) \
			{ \
				LMetadataRegistry::Get().Register(#Class, this); \
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
		LCLASS(ClassName); \
		virtual bool IsAsset() const override { return true; } \
		template<typename AssetType = ClassName> \
		std::enable_if<!std::is_same_v<AssetType, LAsset>, EAssetType> \
		FORCEINLINE GetAssetType() const { return GetStaticType(); } \

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
			LMetadataRegistry::Get().Register((*this).StaticClass(), this); \
			return true; \
		}(); \
		assert(LK_ObjectRegistration && "LCLASS_REGISTER failed"); \
	}
