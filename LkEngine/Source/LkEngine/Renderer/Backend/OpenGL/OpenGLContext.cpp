#include "LKpch.h"
#include "OpenGLContext.h"

#include "LkEngine/Core/Window.h"

#include "OpenGLSwapChain.h"
#include "LkOpenGL.h"

#include "LkEngine/Renderer/UI/UILayer.h"
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"


namespace LkEngine {

	LOpenGLContext::LOpenGLContext(LWindow* InWindow)
		: Window(InWindow)
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(InWindow);

		m_GlfwWindow = Window->GetGlfwWindow();
	}

	void LOpenGLContext::Destroy() 
	{
		LK_CORE_DEBUG_TAG("OpenGLContext", "Destroying");
	}

	void LOpenGLContext::Initialize(const ESourceBlendFunction InSourceBlendFunction,
									const EDestinationBlendFunction InDestinationBlendFunction)
	{
		const GLenum GladInitResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (GladInitResult == 0)
		{
			LK_CORE_ERROR("Failed to initialize Glad, error: {}", static_cast<int>(GladInitResult));
			LK_CORE_ASSERT(false);
		}

		LK_OpenGL_Verify(glEnable(GL_LINE_SMOOTH));
		LK_OpenGL_Verify(glEnable(GL_BLEND));

		SetBlendingEnabled(true);
		SetDepthEnabled(false);

		SetBlendFunction(InSourceBlendFunction, InDestinationBlendFunction);

		/* Fetch backend info. */
		LOpenGL::LoadInfo();

		/* Attach window delegates. */
		LK_CORE_TRACE_TAG("OpenGLContext", "Attaching to OnWindowSizeUpdated delegate");
		Window->OnWindowSizeUpdated.Add(this, &LOpenGLContext::UpdateResolution);
	}

	void LOpenGLContext::SetViewport(const glm::vec2& ViewportPos, const glm::vec2& ViewportSize)
	{
		LK_OpenGL_Verify(glViewport(ViewportPos.x, ViewportPos.y, ViewportSize.x, ViewportSize.y));
	}

	void LOpenGLContext::SetBlendingEnabled(const bool InEnabled)
	{
		bBlendingEnabled = InEnabled;
		if (bBlendingEnabled)
		{
			LK_OpenGL_Verify(glEnable(GL_BLEND));
		}
		else
		{
			LK_OpenGL_Verify(glDisable(GL_BLEND));
		}
	}

	void LOpenGLContext::SetDepthFunction(const EDepthFunction InDepthFunction)
	{
		DepthFunction = InDepthFunction;
		switch (DepthFunction)
		{
			case EDepthFunction::Never:
				LK_OpenGL_Verify(glDepthFunc(GL_NEVER));
				break;

			case EDepthFunction::Less:
				LK_OpenGL_Verify(glDepthFunc(GL_LESS));
				break;

			case EDepthFunction::Equal:
				LK_OpenGL_Verify(glDepthFunc(GL_EQUAL));
				break;

			case EDepthFunction::LessOrEqual:
				LK_OpenGL_Verify(glDepthFunc(GL_LEQUAL));
				break;

			case EDepthFunction::Greater:
				LK_OpenGL_Verify(glDepthFunc(GL_GREATER));
				break;

			case EDepthFunction::NotEqual:
				LK_OpenGL_Verify(glDepthFunc(GL_NOTEQUAL));
				break;

			case EDepthFunction::GreaterOrEqual:
				LK_OpenGL_Verify(glDepthFunc(GL_GEQUAL));
				break;

			case EDepthFunction::Always:
				LK_OpenGL_Verify(glDepthFunc(GL_ALWAYS));
				break;
		}
	}

	void LOpenGLContext::UpdateResolution(const uint16_t Width, const uint16_t Height)
	{
		LK_CORE_ASSERT(m_GlfwWindow);
		int PosX, PosY;
        glfwGetWindowPos(m_GlfwWindow, &PosX, &PosY);

        LK_OpenGL_Verify(glViewport(PosX, PosY, Width, Height));
	}

	void LOpenGLContext::SetDepthEnabled(const bool InEnabled)
	{
		LK_CORE_TRACE_TAG("OpenGLContext", "{} depth testing", (InEnabled ? "Enabling" : "Disabling"));
		bDepthEnabled = InEnabled;
		if (bDepthEnabled)
		{
			LK_OpenGL_Verify(glEnable(GL_DEPTH_TEST));
			return;
		}

		LK_OpenGL_Verify(glDisable(GL_DEPTH_TEST));
	}

	void LOpenGLContext::SetBlendFunction(const ESourceBlendFunction InSourceBlendFunction,
										  const EDestinationBlendFunction InDestinationBlendFunction)
	{
		LK_CORE_TRACE_TAG("OpenGLContext", "Setting source blend function: {}", Enum::ToString(InSourceBlendFunction));
		LK_CORE_TRACE_TAG("OpenGLContext", "Setting destination blend function: {}", Enum::ToString(InDestinationBlendFunction));
		LK_OpenGL_Verify(glBlendFunc(
			LOpenGL::GetSourceBlendFunction(InSourceBlendFunction),
			LOpenGL::GetDestinationBlendFunction(InDestinationBlendFunction))
		);
		BlendFunction.Source = InSourceBlendFunction;
		BlendFunction.Destination = InDestinationBlendFunction;
	}

	void LOpenGLContext::SetSourceBlendFunction(const ESourceBlendFunction InSourceBlendFunction)
	{
		BlendFunction.Source = InSourceBlendFunction;
		LK_CORE_TRACE_TAG("OpenGLContext", "Setting source blend function: {}", Enum::ToString(InSourceBlendFunction));
		LK_OpenGL_Verify(glBlendFunc(
			LOpenGL::GetSourceBlendFunction(BlendFunction.Source), 
			LOpenGL::GetDestinationBlendFunction(BlendFunction.Destination))
		);
	}

	void LOpenGLContext::SetDestinationBlendFunction(const EDestinationBlendFunction InDestinationBlendFunction)
	{
		BlendFunction.Destination = InDestinationBlendFunction;
		LK_CORE_TRACE_TAG("OpenGLContext", "Setting source blend function: {}", Enum::ToString(InDestinationBlendFunction));
		LK_OpenGL_Verify(glBlendFunc(
			LOpenGL::GetSourceBlendFunction(BlendFunction.Source), 
			LOpenGL::GetDestinationBlendFunction(BlendFunction.Destination))
		);
	}

}
