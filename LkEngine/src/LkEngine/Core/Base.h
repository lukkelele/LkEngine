#pragma once

#include "LkEngine/Core/PlatformDetection.h"

#include <cassert>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cstddef>

#include "LkEngine/Core/UUID.h"

namespace LkEngine {

	#define LK_ASSERT(_COND)                 assert(_COND)
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
	using RendererID = uint32_t;

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

	/* Pair Creation */
	template<typename T, typename P>
	using pair = std::pair<T, P>;
	template<typename T, typename P, typename ... ARGS>
	constexpr pair<T, P> new_pair(ARGS&& ... args)
	{
		return std::make_pair<T, P>(std::forward<ARGS>(args)...);
	}
	

#ifdef LK_RENDERER_API_OPENGL
	// OpenGL call macro to use for invoking opengl functions, provides error handling
	#define GL_CALL(_FUNC) OpenGL_ClearError(); _FUNC; LK_ASSERT(OpenGL_LogCall(#_FUNC, __FILE__, __LINE__))
    // OpenGL Error handling functions, defined in OpenGLContext.cpp
    void OpenGL_ClearError();
    bool OpenGL_LogCall(const char* function, const char* file, int line);
	const char* OpenGL_GetVersion();

    constexpr int OpenGL_Major_Version = 4;
    constexpr int OpenGL_Minor_Version = 5;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";
    constexpr const char* OpenGL_GLSL = OpenGL_GLSL_33;

	constexpr const char* LK_SHADER_VERSION = OpenGL_GLSL;

#elif defined(LK_RENDERER_API_VULKAN)
	constexpr const char* LK_SHADER_VERSION = "";
#endif

}


#ifdef RENDERER_API_VULKAN
#include "Vulkan/vulkan.h"

static std::string Vulkan_GetErrorString(VkResult errorCode)
{
	switch (errorCode)
	{
#define STR(r) case VK_ ##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
		STR(ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

#define VK_EVALUATE(_FUNC) 
// #define VK_EVALUATE(_FUNC) \
// {
// 	VkResult result = (_FUNC);
//     if (result != VK_SUCCESS)
//     {
// 		std::cout << "VkResult == \"" + Vulkan_GetErrorString(result) + "\"";
// 		assert(result == VK_SUCCESS);
//     }
// }
#else
#define VK_EVALUATE(_FUNC) 
#endif