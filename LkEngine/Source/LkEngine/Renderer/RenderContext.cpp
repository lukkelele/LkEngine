#include "LKpch.h"
#include "RenderContext.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLContext.h"

#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LRenderContext> LRenderContext::Create(LWindow* WindowRef)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:
			{
				return TObjectPtr<LOpenGLContext>::Create(WindowRef);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	void LRenderContext::SetProfile(const ERenderProfile InProfile)
	{
		ERendererAPI RenderAPI = LRendererAPI::Current();

		switch (InProfile)
		{
			case ERenderProfile::Core:
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

			case ERenderProfile::Compability:
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

	void LRenderContext::SetVersion(const int MajorVersion, const int MinorVersion)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:
			{
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MajorVersion); 
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MinorVersion);
				return;
			}

			case ERendererAPI::Vulkan: 
			{
				/* Empty for now. */
				return;
			}
		}

		LK_CORE_ASSERT(false, "SetVersion(MAJOR, MINOR) failed, neither Vulkan or OpenGL was detected");
	}

}
