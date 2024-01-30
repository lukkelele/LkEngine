#include "LKpch.h"
#include "OpenGLContext.h"

#include "LkEngine/Core/Window.h"

#include "OpenGLSwapChain.h"
#include "LkOpenGL.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	OpenGLContext::OpenGLContext(Window* window)
    {
		m_Instance = Ref<OpenGLContext>(this);
	    m_Window = s_ptr<Window>(window);
		m_GlfwWindow = m_Window->GetGlfwWindow();
	}

    OpenGLContext::~OpenGLContext()
    {
    }

    void OpenGLContext::Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc)
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
		SetBlendFunction(srcFunc, dstFunc);
    }

	void OpenGLContext::SetViewport(const glm::vec2& pos, const glm::vec2& size)
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}

	void OpenGLContext::SetBlendingEnabled(bool enabled)
	{
		m_BlendingEnabled = enabled;
		if (m_BlendingEnabled)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

    void OpenGLContext::Destroy()
    {
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
		m_DepthEnabled = enabled;
		if (m_DepthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
			return;
		}
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLContext::SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc)
	{
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));
		LK_CORE_DEBUG("Setting destination blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		glBlendFunc(GLUtils::GetOpenGLSourceBlendFunction(srcFunc), GLUtils::GetOpenGLDestinationBlendFunction(dstFunc));
		m_BlendFunction.Source = srcFunc;
		m_BlendFunction.Destination = dstFunc;
	}

	void OpenGLContext::SetSourceBlendFunction(const SourceBlendFunction& srcFunc)
	{
		m_BlendFunction.Source = srcFunc;
		LK_CORE_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));
		glBlendFunc(GLUtils::GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GLUtils::GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

    void OpenGLContext::SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc)
	{
		m_BlendFunction.Destination = dstFunc;
		LK_CORE_DEBUG("Setting source blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		glBlendFunc(GLUtils::GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GLUtils::GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

	std::string OpenGLContext::GetCurrentSourceBlendFunctionName() const
	{
		return GetSourceBlendFunctionName(m_BlendFunction.Source);
	}

	std::string OpenGLContext::GetCurrentDestinationBlendFunctionName() const
	{
		return GetDestinationBlendFunctionName(m_BlendFunction.Destination);
	}


}