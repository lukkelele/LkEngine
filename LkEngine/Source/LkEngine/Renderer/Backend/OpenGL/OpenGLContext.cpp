#include "LKpch.h"
#include "OpenGLContext.h"

#include "LkEngine/Core/Window.h"

#include "OpenGLSwapChain.h"
#include "LkOpenGL.h"

#include "LkEngine/UI/UILayer.h"
#include "LkEngine/Renderer/Backend/OpenGL/OpenGLImGuiLayer.h"


namespace LkEngine {

	LOpenGLContext::LOpenGLContext(LWindow* InWindow)
		: Window(InWindow)
    {
		LCLASS_REGISTER();

		m_GlfwWindow = Window->GetGlfwWindow();

		/* Attach window delegates. */
		LK_CORE_DEBUG_TAG("OpenGLContext", "Attaching to OnWindowSizeUpdated delegate");
		Window->OnWindowSizeUpdated.Add(this, &LOpenGLContext::UpdateResolution);
	}

    void LOpenGLContext::Destroy()
    {
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

		LK_OpenGL(glEnable(GL_LINE_SMOOTH));
		LK_OpenGL(glEnable(GL_BLEND));

		SetBlendingEnabled(true);
		SetDepthEnabled(false);

		SetBlendFunction(InSourceBlendFunction, InDestinationBlendFunction);
    }

	void LOpenGLContext::SetViewport(const glm::vec2& ViewportPos, const glm::vec2& ViewportSize)
	{
		LK_OpenGL(glViewport(ViewportPos.x, ViewportPos.y, ViewportSize.x, ViewportSize.y));
	}

	void LOpenGLContext::SetBlendingEnabled(const bool InEnabled)
	{
		bBlendingEnabled = InEnabled;
		if (bBlendingEnabled)
		{
			LK_OpenGL(glEnable(GL_BLEND));
		}
		else
		{
			LK_OpenGL(glDisable(GL_BLEND));
		}
	}

	void LOpenGLContext::SetDepthFunction(const EDepthFunction InDepthFunction)
	{
		DepthFunction = InDepthFunction;
		switch (DepthFunction)
		{
			case EDepthFunction::Never:
				LK_OpenGL(glDepthFunc(GL_NEVER));
				break;

			case EDepthFunction::Less:
				LK_OpenGL(glDepthFunc(GL_LESS));
				break;

			case EDepthFunction::Equal:
				LK_OpenGL(glDepthFunc(GL_EQUAL));
				break;

			case EDepthFunction::LessOrEqual:
				LK_OpenGL(glDepthFunc(GL_LEQUAL));
				break;

			case EDepthFunction::Greater:
				LK_OpenGL(glDepthFunc(GL_GREATER));
				break;

			case EDepthFunction::NotEqual:
				LK_OpenGL(glDepthFunc(GL_NOTEQUAL));
				break;

			case EDepthFunction::GreaterOrEqual:
				LK_OpenGL(glDepthFunc(GL_GEQUAL));
				break;

			case EDepthFunction::Always:
				LK_OpenGL(glDepthFunc(GL_ALWAYS));
				break;
		}
	}

	/* TODO: Fix the use of ImGui here. Should not be like that. */
	void LOpenGLContext::UpdateResolution(const uint16_t Width, const uint16_t Height)
	{
		LK_CORE_DEBUG_TAG("OpenGLContext", "Update resolution to ({}, {})", Width, Height);
		ImGuiViewport* Viewport = ImGui::GetMainViewport();
		const ImVec2 Pos = Viewport->WorkPos;
		LK_OpenGL(glViewport(Pos.x, Pos.y, Width, Height));

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(Width, Height);
	}

	void LOpenGLContext::SetDepthEnabled(const bool InEnabled)
	{
		bDepthEnabled = InEnabled;
		if (bDepthEnabled)
		{
			LK_OpenGL(glEnable(GL_DEPTH_TEST));
			return;
		}

		LK_OpenGL(glDisable(GL_DEPTH_TEST));
	}

	void LOpenGLContext::SetBlendFunction(const ESourceBlendFunction InSourceBlendFunction, 
		                                  const EDestinationBlendFunction InDestinationBlendFunction)
	{
		LK_CORE_DEBUG_TAG("OpenGLContext", "Setting source blend function: {}", Enum::ToString(InSourceBlendFunction));
		LK_CORE_DEBUG_TAG("OpenGLContext", "Setting destination blend function: {}", Enum::ToString(InDestinationBlendFunction));
		LK_OpenGL(glBlendFunc(
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
		LK_OpenGL(glBlendFunc(
			LOpenGL::GetSourceBlendFunction(BlendFunction.Source), 
			LOpenGL::GetDestinationBlendFunction(BlendFunction.Destination)
		));
	}

    void LOpenGLContext::SetDestinationBlendFunction(const EDestinationBlendFunction InDestinationBlendFunction)
	{
		BlendFunction.Destination = InDestinationBlendFunction;
		LK_CORE_TRACE_TAG("OpenGLContext", "Setting source blend function: {}", Enum::ToString(InDestinationBlendFunction));
		LK_OpenGL(glBlendFunc( 
			LOpenGL::GetSourceBlendFunction(BlendFunction.Source), 
			LOpenGL::GetDestinationBlendFunction(BlendFunction.Destination)
		));
	}

}