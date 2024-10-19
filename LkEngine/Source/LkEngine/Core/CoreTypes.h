/******************************************************************
 * CoreTypes
 * 
 *******************************************************************/
#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>


namespace LkEngine 
{
	using Type_Nullptr = std::nullptr_t;

	using byte = uint8_t;
	using llong = long long;
	using RendererID = uint32_t;

	/** EShapeType */
	enum class EShapeType
	{
		Null = 0,
		Triangle,
		Quad,
		Hexagon
	};

}
