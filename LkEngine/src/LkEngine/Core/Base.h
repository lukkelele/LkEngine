#pragma once

#include "LkEngine/Core/PlatformDetection.h"

#include <cassert>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <string_view>
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstddef>
#include <array>

#include "Ref.h"
#include "Assert.h"
#include "UUID.h"


namespace LkEngine {

	#define LK_ARRAYSIZE(_ARR)               ((int)(sizeof(_ARR) / sizeof(*(_ARR))))    
	#define LK_UNUSED(_VAR)                  ((void)(_VAR))
	#define BIT_FIELD(x)                     (1 << x)
	#define STRINIGFY(x)                     #x
	
	#ifdef LK_PLATFORM_WINDOWS
		#define LK_ITOA(c, buf, size, p)  _itoa_s(c, buf, size, p)
	#elif defined(LK_PLATFORM_LINUX)
		#define LK_ITOA(c, buf, size, p)  void() // FIXME
	#endif

	using byte = uint8_t;
	using llong = long long;
	using RendererID = uint32_t;
	using Timestep = float;

	enum class ShapeType
	{
		Null = 0,
		Triangle,
		Quad,
		Hexagon
	};

	enum class RendererAPIType
	{
		None,
		OpenGL,
		Vulkan
	};

	// Unique pointer 
	template<typename T>
	using u_ptr = std::unique_ptr<T>;
	template<typename T, typename ... ARGS>
	constexpr u_ptr<T> create_u_ptr(ARGS&& ... args)
	{
		return std::make_unique<T>(std::forward<ARGS>(args)...);
	}

	// Shared pointer
	template<typename T>
	using s_ptr = std::shared_ptr<T>;
	// TODO: Replace create_s_ptr with new_s_ptr
	template<typename T, typename ... ARGS>
	constexpr s_ptr<T> create_s_ptr(ARGS&& ... args)
	{
		return std::make_shared<T>(std::forward<ARGS>(args)...);
	}
	template<typename T, typename ... ARGS>
	constexpr s_ptr<T> new_s_ptr(ARGS&& ... args)
	{
		return std::make_shared<T>(std::forward<ARGS>(args)...);
	}


}
