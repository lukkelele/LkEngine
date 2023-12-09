#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLContext.h"
#include "LkEngine/Core/Window.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>

#ifdef LK_PLATFORM_WINDOWS
#include "LkEngine/Platform/Windows/Windows_Window.h"
#elif defined(LK_PLATFORM_LINUX)
#include "LkEngine/Platform/Linux/Linux_Window.h"
#endif


namespace LkEngine {

	void OpenGL_ClearError() { while (glGetError() != GL_NO_ERROR); }

    bool OpenGL_LogCall(const char* function, const char* file, int line)
    {
        while (GLenum error = glGetError())
        {
            std::cout << "[OpenGL Error] (" << error << ")\nFUNCTION: " << function <<
                "\nFILE: " << file << "\nLINE: " << line << std::endl;
            return false;
        }
        return true;
    }

	int GetOpenGLSourceBlendFunction(const SourceBlendFunction& srcFunc)
	{
		switch (srcFunc)
		{
			case SourceBlendFunction::Zero:  return GL_ZERO;
			case SourceBlendFunction::One:   return GL_ONE;
			case SourceBlendFunction::Alpha: return GL_ALPHA;
			case SourceBlendFunction::Color: return GL_COLOR;
			case SourceBlendFunction::One_Minus_DestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
			default: throw std::runtime_error("Source blend function could not be retrieved correctly");
		}
	}

	int GetOpenGLDestinationBlendFunction(const DestinationBlendFunction& dstFunc)
	{
		switch (dstFunc)
		{
			case DestinationBlendFunction::Zero:  return GL_ZERO;
			case DestinationBlendFunction::One:   return GL_ONE;
			case DestinationBlendFunction::Alpha: return GL_ALPHA;
			case DestinationBlendFunction::Color: return GL_COLOR;
			case DestinationBlendFunction::One_Minus_SourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			default: throw std::runtime_error("Destination blend function could not be retrieved correctly");
		}
	}

    OpenGLContext::OpenGLContext(Window& window, const std::string& glslVersion)
    {
    	m_Instance = this;
	    m_Window = std::shared_ptr<Window>(&window);
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
		printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		// TODO: Add condition to check if 2D or 3D, to enable depth test or not
		//glEnable(GL_DEPTH_TEST);

		//glDepthFunc(GL_LESS);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		m_BlendFunction.Source = srcFunc;
		m_BlendFunction.Destination = dstFunc;
		SetBlendFunction(srcFunc, dstFunc);

	
		InitImGui(m_Window->GetGlslVersion().c_str());

		const char* glVersion = (char*)glGetString(GL_VERSION);
		if (glVersion)
		{
			LOG_DEBUG("OpenGL Version: {}", glVersion);
		}
    }

    void OpenGLContext::Destroy()
    {
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
    }

    void OpenGLContext::InitImGui(const std::string& glslVersion)
    {
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		io.ConfigDockingAlwaysTabBar = false;

	    ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
		LOG_INFO("ImGui Version: {0}", ImGui::GetVersion());
    }

	void OpenGLContext::BeginImGuiFrame()
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void OpenGLContext::EndImGuiFrame()
	{
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		LOG_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));
		LOG_DEBUG("Setting destination blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		glBlendFunc(GetOpenGLSourceBlendFunction(srcFunc), GetOpenGLDestinationBlendFunction(dstFunc));
	}

	void OpenGLContext::SetSourceBlendFunction(const SourceBlendFunction& srcFunc)
	{
		m_BlendFunction.Source = srcFunc;
		LOG_DEBUG("Setting source blend function: {}", GetSourceBlendFunctionName(srcFunc));
		glBlendFunc(GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

    void OpenGLContext::SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc)
	{
		m_BlendFunction.Destination = dstFunc;
		LOG_DEBUG("Setting source blend function: {}", GetDestinationBlendFunctionName(dstFunc));
		glBlendFunc(GetOpenGLSourceBlendFunction(m_BlendFunction.Source), GetOpenGLDestinationBlendFunction(m_BlendFunction.Destination));
	}

	std::string OpenGLContext::GetSourceBlendFunctionName()
	{
		return GetSourceBlendFunctionName(m_BlendFunction.Source);
	}

	std::string OpenGLContext::GetDestinationBlendFunctionName()
	{
		return GetDestinationBlendFunctionName(m_BlendFunction.Destination);
	}

	std::string OpenGLContext::GetSourceBlendFunctionName(const SourceBlendFunction& srcFunc)
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

	std::string OpenGLContext::GetDestinationBlendFunctionName(const DestinationBlendFunction& dstFunc)
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