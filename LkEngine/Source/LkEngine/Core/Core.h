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


	template<typename T>
	FORCEINLINE T BitMask(const uint32_t Count);

	template<>
	FORCEINLINE uint64_t BitMask<uint64_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 64);
		return (uint64_t(Count < 64) << Count) - 1;
	}

	template<>
	FORCEINLINE uint32_t BitMask<uint32_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 32);
		return uint32_t(uint64_t(1) << Count) - 1;
	}

	template<>
	FORCEINLINE uint16_t BitMask<uint16_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 16);
		return uint16_t((uint32_t(1) << Count) - 1);
	}

	template<>
	FORCEINLINE uint8_t BitMask<uint8_t>(const uint32_t Count)
	{
		LK_CORE_ASSERT(Count <= 8);
		return uint8_t((uint32_t(1) << Count) - 1);
	}

}

