/**
 * @file
 * @brief Core types.
 */
#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "PlatformDetection.h"

namespace LkEngine {

	/**
	 * @defgroup CoreTypes Core Types
	 * Typedefs and other engine type definitions.
	 *
	 * @ingroup Core
	 * @{
	 */

	using byte = uint8_t;
	using llong = long long;

	/**
	 * @typedef LRendererID
	 * Identifier used in render contexts.
	 */
	using LRendererID = uint32_t;

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

	/** 
	 * @enum EShapeType 
	 *
	 * Shape type.
	 */
	enum class EShapeType
	{
		Null = 0,
		Triangle,
		Quad,
		Hexagon
	};

	/**
	 * @brief Static cast.
	 * @tparam To The type to cast to the object to.
	 * @tparam From The type of the source object.
	 * @param Source Object to perform the cast on.
	 * @return A pointer to the casted object.
	 */
	template <typename To, typename From>
	To* Cast(From* Source)
	{
		static_assert((sizeof(From) > 0) && (sizeof(To) > 0), "Invalid type sizes");
		if (Source)
		{
			return (static_cast<To*>(Source));
		}

		return nullptr;
	}

	/** @} */

}
