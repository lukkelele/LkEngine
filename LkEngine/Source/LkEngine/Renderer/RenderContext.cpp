#include "LKpch.h"
#include "RenderContext.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Platform/OpenGL/OpenGLContext.h"
//#include "LkEngine/Platform/Vulkan/VulkanContext.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<RenderContext> RenderContext::Create(LWindow* WindowRef)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:
			{
				return Ref<OpenGLContext>::Create(WindowRef);
			}

			/// @FIXME:
			//case RendererAPIType::Vulkan: return Ref<VulkanContext>::Create(window);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	void RenderContext::SetProfile(const EProfile& InProfile)
	{
		ERendererAPI RenderAPI = LRendererAPI::Current();

		switch (InProfile)
		{
			case EProfile::Core:
			{
				if (RenderAPI == ERendererAPI::Vulkan)
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				}
				else if (RenderAPI == ERendererAPI::OpenGL)
				{
					/* Core Profile */
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				}

				break;
			}
			case EProfile::Compability:
			{
				if (RenderAPI == ERendererAPI::Vulkan)
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				}
				else if (RenderAPI == ERendererAPI::OpenGL)
				{
					/* Compability Profile */
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
				}
				break;
			}
		}

	}

	void RenderContext::SetVersion(const int MajorVersion, const int MinorVersion)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::Vulkan: 
			{
				/* -- Empty -- */
				return;
			}
			case ERendererAPI::OpenGL:
			{
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MajorVersion); 
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MinorVersion);
				return;
			}

		}

		LK_CORE_ASSERT(false, "SetVersion(MAJOR, MINOR) failed, neither Vulkan or OpenGL was detected");
	}

	void RenderContext::HandleViewportEvents()
	{
		LWindow& WindowRef = LWindow::Get();
		int ViewportWidth{};
		int ViewportHeight;
		glfwGetWindowSize(WindowRef.GetGlfwWindow(), &ViewportWidth, &ViewportHeight);
	}

	std::string RenderContext::GetSourceBlendFunctionName(const ESourceBlendFunction& InSourceFunction)
	{
		switch (InSourceFunction)
		{
			case ESourceBlendFunction::Zero:  return "Zero";
			case ESourceBlendFunction::One:   return "One";
			case ESourceBlendFunction::Alpha: return "Alpha";
			case ESourceBlendFunction::Color: return "Color";
			case ESourceBlendFunction::One_Minus_DestinationAlpha: return "One_Minus_DestinationAlpha";
			default: throw std::runtime_error("Source blend function name could not be retrieved correctly");
		}
	}

	std::string RenderContext::GetDestinationBlendFunctionName(const EDestinationBlendFunction& InDestinationFunction)
	{
		switch (InDestinationFunction)
		{
			case EDestinationBlendFunction::Zero:  return "Zero";
			case EDestinationBlendFunction::One:   return "One";
			case EDestinationBlendFunction::Alpha: return "Alpha";
			case EDestinationBlendFunction::Color: return "Color";
			case EDestinationBlendFunction::One_Minus_SourceAlpha: return "One_Minus_SourceAlpha";
			default: throw std::runtime_error("Destination blend function name could not be retrieved correctly");
		}
	}

	

}
