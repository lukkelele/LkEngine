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

#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log.h"

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/TypeTrait.h"

#include "LkEngine/Core/Exception.h"
#include "LkEngine/Core/Timer.h"
#include "LkEngine/Core/Memory/MemoryPtr.h"
#include "LkEngine/Core/Memory/Buffer.h"
#include "LkEngine/Core/Memory/Ref.h"
#include "LkEngine/Core/Hash/UUID.h"


namespace LkEngine
{
	/** ERendererAPI */
	enum class ERendererAPI
	{
		None = 0,
		OpenGL,
		Vulkan
	};
}
