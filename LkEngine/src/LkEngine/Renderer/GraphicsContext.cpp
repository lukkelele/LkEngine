#include "LKpch.h"
#include "GraphicsContext.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Platform/OpenGL/OpenGLContext.h"
//#include "LkEngine/Platform/Vulkan/VulkanContext.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<GraphicsContext> GraphicsContext::Create(Window* window)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLContext>::Create(window);
			//case RendererAPIType::Vulkan: return Ref<VulkanContext>::Create(window);
			case RendererAPIType::None:   return nullptr;
		}
	}

	void GraphicsContext::SetProfile(const Profile& profile)
	{
		RendererAPIType renderAPI = RendererAPI::Current();

		switch (profile)
		{
			case Profile::Core:
			{
				if (renderAPI == RendererAPIType::Vulkan)
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				}
				else if (renderAPI == RendererAPIType::OpenGL)
				{
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set core profile
				}

				break;
			}
			case Profile::Compability:
			{
				if (renderAPI == RendererAPIType::Vulkan)
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				}
				else if (renderAPI == RendererAPIType::OpenGL)
				{
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // Set core profile
				}
				break;
			}
		}

	}

	void GraphicsContext::SetVersion(int majorVersion, int minorVersion)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return;
			case RendererAPIType::OpenGL:
			{
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
				return;
			}

		}
		LK_CORE_ASSERT(false, "SetVersion(MAJOR, MINOR) failed, neither Vulkan or OpenGL was detected");
	}

	void GraphicsContext::HandleViewportEvents()
	{
		auto& window = Window::Get();
		int viewport_width, viewport_height;
		glfwGetWindowSize(window.GetGlfwWindow(), &viewport_width, &viewport_height);
	}

	std::string GraphicsContext::GetSourceBlendFunctionName(const SourceBlendFunction& srcFunc)
	{
		switch (srcFunc)
		{
			case SourceBlendFunction::Zero:  return "Zero";
			case SourceBlendFunction::One:   return "One";
			case SourceBlendFunction::Alpha: return "Alpha";
			case SourceBlendFunction::Color: return "Color";
			case SourceBlendFunction::One_Minus_DestinationAlpha: return "One_Minus_DestinationAlpha";
			default: throw std::runtime_error("Source blend function name could not be retrieved correctly");
		}
	}

	std::string GraphicsContext::GetDestinationBlendFunctionName(const DestinationBlendFunction& dstFunc)
	{
		switch (dstFunc)
		{
			case DestinationBlendFunction::Zero:  return "Zero";
			case DestinationBlendFunction::One:   return "One";
			case DestinationBlendFunction::Alpha: return "Alpha";
			case DestinationBlendFunction::Color: return "Color";
			case DestinationBlendFunction::One_Minus_SourceAlpha: return "One_Minus_SourceAlpha";
			default: throw std::runtime_error("Destination blend function name could not be retrieved correctly");
		}
	}

	

}
