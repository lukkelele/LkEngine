/******************************************************************
 * Core
 *
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/PlatformDetection.h"

#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <stdarg.h>
#include <cstdint>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <queue>
#include <ranges>

#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log/Log.h"

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Template/TypeTrait.h"

#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/Memory/Buffer.h"
#include "LkEngine/Core/Hash/UUID.h"


namespace LkEngine {

	enum class ERendererAPI
	{
		None = 0,
		OpenGL,
		Vulkan
	};
	LK_ENUM_CLASS_FLAGS(ERendererAPI);
	LK_ENUM_RANGE_BY_FIRST_AND_LAST(ERendererAPI, ERendererAPI::None, ERendererAPI::Vulkan);

}

