/******************************************************************
 * CoreTypes
 * 
 *******************************************************************/
#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "PlatformDetection.h"


namespace LkEngine 
{
	using Type_Nullptr = std::nullptr_t;

	using byte = uint8_t;
	using llong = long long;

	using RendererID = uint32_t;

	/* Path separator. */
	template<typename TChar>
	constexpr TChar TPathSeparator = '/';

#if defined(LK_PLATFORM_WINDOWS)
	template<> constexpr char TPathSeparator<char> = '\\';
	template<> constexpr wchar_t TPathSeparator<wchar_t> = L'\\';
#elif defined(LK_PLATFORM_LINUX)
	template<> constexpr char TPathSeparator<char> = '/';
	template<> constexpr wchar_t TPathSeparator<wchar_t> = L'/';
#endif

	constexpr LK_TCHAR PathSeparator = TPathSeparator<LK_TCHAR>;
#if 0
#if (LK_CHAR_ENCODING == LK_CHAR_UTF8)
	constexpr char PathSeparator = TPathSeparator<char>;
#elif (LK_CHAR_ENCODING == LK_CHAR_UNICODE)
	constexpr wchar_t PathSeparator = TPathSeparator<wchar_t>;
#else
#	error "No character encoding format is specified"
#endif
#endif

	/** EShapeType */
	enum class EShapeType
	{
		Null = 0,
		Triangle,
		Quad,
		Hexagon
	};


	template <typename To, typename From>
	To* Cast(From* Source)
	{
		static_assert((sizeof(From) > 0) && (sizeof(To) > 0), "Incomplete types, cannot proceed with cast");
		if (Source)
		{
			return (static_cast<To*>(Source));
		}

		return nullptr;
	}

}
