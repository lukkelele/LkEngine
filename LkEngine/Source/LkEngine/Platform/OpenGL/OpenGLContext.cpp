#include "LKpch.h"
#include "OpenGLContext.h"

#include "LkEngine/Core/Window.h"

#include "OpenGLSwapChain.h"
#include "LkOpenGL.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	OpenGLContext::OpenGLContext(LWindow* InWindowRef)
    {
		m_Window = InWindowRef;
		m_GlfwWindow = m_Window->GetGlfwWindow();
	}

    OpenGLContext::~OpenGLContext()
    {
    }

    void OpenGLContext::Destroy()
    {
    }

    void OpenGLContext::Init(const ESourceBlendFunction& InSourceBlendFunction, 
		                     const EDestinationBlendFunction& InDestinationBlendFunction)
    {
		GLenum err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (err == 0)
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

	void OpenGLContext::SetViewport(const glm::vec2& pos, const glm::vec2& size)
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}

	void OpenGLContext::SetBlendingEnabled(bool enabled)
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

	void OpenGLContext::SetDepthFunction(const EDepthFunction& InDepthFunction)
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

	void OpenGLContext::UpdateResolution(uint16_t width, uint16_t height)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos = viewport->WorkPos;
		glViewport(pos.x, pos.y, width, height);

		auto& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
	}

	void OpenGLContext::SetDepthEnabled(bool enabled)
	{
		bDepthEnabled = enabled;
		if (bDepthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
			return;
		}
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLContext::SetBlendFunction(const ESourceBlendFunction& InSourceBlendFunction, 
		                                 const EDestinationBlendFunction& InDestinationBlendFunction)
	{
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(InSourceBlendFunction));
		LK_CORE_DEBUG("Setting destination blend function: {}", GetDestinationBlendFunctionName(InDestinationBlendFunction));
		glBlendFunc(GLUtils::GetOpenGLSourceBlendFunction(InSourceBlendFunction), GLUtils::GetOpenGLDestinationBlendFunction(InDestinationBlendFunction));
		BlendFunction.Source = InSourceBlendFunction;
		BlendFunction.Destination = InDestinationBlendFunction;
	}

	void OpenGLContext::SetSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction)
	{
		BlendFunction.Source = InSourceBlendFunction;
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(InSourceBlendFunction));
		glBlendFunc(
			GLUtils::GetOpenGLSourceBlendFunction(BlendFunction.Source), 
			GLUtils::GetOpenGLDestinationBlendFunction(BlendFunction.Destination)
		);
	}

    void OpenGLContext::SetDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction)
	{
		BlendFunction.Destination = InDestinationBlendFunction;
		LK_CORE_DEBUG("Setting source blend function: {}", GetDestinationBlendFunctionName(InDestinationBlendFunction));
		glBlendFunc(
			GLUtils::GetOpenGLSourceBlendFunction(BlendFunction.Source), 
			GLUtils::GetOpenGLDestinationBlendFunction(BlendFunction.Destination)
		);
	}

	std::string OpenGLContext::GetCurrentSourceBlendFunctionName() const
	{
		return GetSourceBlendFunctionName(BlendFunction.Source);
	}

	std::string OpenGLContext::GetCurrentDestinationBlendFunctionName() const
	{
		return GetDestinationBlendFunctionName(BlendFunction.Destination);
	}


}