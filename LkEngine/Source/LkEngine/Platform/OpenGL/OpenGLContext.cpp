#include "LKpch.h"
#include "OpenGLContext.h"

#include "LkEngine/Core/Window.h"

#include "OpenGLSwapChain.h"
#include "LkOpenGL.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	LOpenGLContext::LOpenGLContext(LWindow* InWindow)
    {
		Window = InWindow;
		m_GlfwWindow = Window->GetGlfwWindow();

		/* Attach window delegates. */
		LK_CORE_DEBUG_TAG("OpenGLContext", "Attaching to OnWindowSizeUpdated delegate");
		Window->OnWindowSizeUpdated.Add(this, &LOpenGLContext::UpdateResolution);
	}

    void LOpenGLContext::Destroy()
    {
    }

    void LOpenGLContext::Init(const ESourceBlendFunction& InSourceBlendFunction, 
		                     const EDestinationBlendFunction& InDestinationBlendFunction)
    {
		const GLenum GladError = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (GladError == 0)
		{
			printf("[ERROR] Error starting GLAD");
			exit(EXIT_FAILURE);
		}

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);

		SetBlendingEnabled(true);
		SetDepthEnabled(false);

		SetBlendFunction(InSourceBlendFunction, InDestinationBlendFunction);
    }

	void LOpenGLContext::SetViewport(const glm::vec2& pos, const glm::vec2& size)
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}

	void LOpenGLContext::SetBlendingEnabled(bool enabled)
	{
		bBlendingEnabled = enabled;
		if (bBlendingEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void LOpenGLContext::SetDepthFunction(const EDepthFunction& InDepthFunction)
	{
		DepthFunction = InDepthFunction;
		switch (DepthFunction)
		{
			case EDepthFunction::Never:
				glDepthFunc(GL_NEVER);
				break;
			case EDepthFunction::Less:
				glDepthFunc(GL_LESS);
				break;
			case EDepthFunction::Equal:
				glDepthFunc(GL_EQUAL);
				break;
			case EDepthFunction::LessOrEqual:
				glDepthFunc(GL_LEQUAL);
				break;
			case EDepthFunction::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case EDepthFunction::NotEqual:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case EDepthFunction::GreaterOrEqual:
				glDepthFunc(GL_GEQUAL);
				break;
			case EDepthFunction::Always:
				glDepthFunc(GL_ALWAYS);
				break;
		}
	}

	void LOpenGLContext::UpdateResolution(const uint16_t Width, const uint16_t Height)
	{
		LK_CORE_DEBUG_TAG("OpenGLContext", "UpdateResolution -> ({}, {})", Width, Height);
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		const ImVec2 Pos = viewport->WorkPos;
		glViewport(Pos.x, Pos.y, Width, Height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(Width, Height);
	}

	void LOpenGLContext::SetDepthEnabled(const bool InEnabled)
	{
		bDepthEnabled = InEnabled;
		if (bDepthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
			return;
		}
		glDisable(GL_DEPTH_TEST);
	}

	void LOpenGLContext::SetBlendFunction(const ESourceBlendFunction& InSourceBlendFunction, 
		                                 const EDestinationBlendFunction& InDestinationBlendFunction)
	{
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(InSourceBlendFunction));
		LK_CORE_DEBUG("Setting destination blend function: {}", GetDestinationBlendFunctionName(InDestinationBlendFunction));
		glBlendFunc(GLUtils::GetOpenGLSourceBlendFunction(InSourceBlendFunction), GLUtils::GetOpenGLDestinationBlendFunction(InDestinationBlendFunction));
		BlendFunction.Source = InSourceBlendFunction;
		BlendFunction.Destination = InDestinationBlendFunction;
	}

	void LOpenGLContext::SetSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction)
	{
		BlendFunction.Source = InSourceBlendFunction;
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(InSourceBlendFunction));
		glBlendFunc(
			GLUtils::GetOpenGLSourceBlendFunction(BlendFunction.Source), 
			GLUtils::GetOpenGLDestinationBlendFunction(BlendFunction.Destination)
		);
	}

    void LOpenGLContext::SetDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction)
	{
		BlendFunction.Destination = InDestinationBlendFunction;
		LK_CORE_DEBUG("Setting source blend function: {}", GetDestinationBlendFunctionName(InDestinationBlendFunction));
		glBlendFunc(
			GLUtils::GetOpenGLSourceBlendFunction(BlendFunction.Source), 
			GLUtils::GetOpenGLDestinationBlendFunction(BlendFunction.Destination)
		);
	}

	std::string LOpenGLContext::GetCurrentSourceBlendFunctionName() const
	{
		return GetSourceBlendFunctionName(BlendFunction.Source);
	}

	std::string LOpenGLContext::GetCurrentDestinationBlendFunctionName() const
	{
		return GetDestinationBlendFunctionName(BlendFunction.Destination);
	}


}