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
#	define STDCALL  __stdcall
#	define VARARGS  __cdecl
#	ifdef FORCEINLINE
#		undef FORCEINLINE
#		define FORCEINLINE  __forceinline
#	endif

#	define FORCEINLINE  __forceinline
#	define LK_ITOA(c, buf, size, p)  _itoa_s(c, buf, size, p)
#endif

/** Linux Platform */
#if defined(LK_PLATFORM_LINUX)
	#define STDCALL  __stdcall
	#define VARARGS  __cdecl
	#define FORCEINLINE __forceinline
	#define LK_ITOA(c, buf, size, p)  void() // FIXME
#endif

/* Mark a function 'Not implemented'. */
#define LK_MARK_FUNC_NOT_IMPLEMENTED(...) \
	LK_CORE_ASSERT(false, "[ Function not implemented ]\n{}" __VA_OPT__("\nDev note: {}"), LK_FUNCSIG __VA_OPT__(, __VA_ARGS__))

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
			static const bool bClassRegistered = (LClass::Register<Class>(#Class), true); \
			return LClass::Get(LType<Class>::ID()); \
		} \
		virtual const LClass* GetClass() const override \
		{ \
			return Class::StaticClass(); \
		} \
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


