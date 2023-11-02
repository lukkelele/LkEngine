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


    OpenGLContext::OpenGLContext(Window& window, const std::string& glslVersion)
    {
    	m_Context = this;
	    m_Window = std::shared_ptr<Window>(&window);
		m_GlfwWindow = m_Window->GetGlfwWindow();
		
		m_MainRenderWindowSize = ImVec2(0, 0);
	}

    OpenGLContext::~OpenGLContext()
    {
    }

    void OpenGLContext::Init()
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
		//glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	
		InitImGui(m_Window->GetGlslVersion().c_str());
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
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		//io.ConfigViewportsNoDecoration = false;
		//io.ConfigWindowsResizeFromEdges = false;
		io.ConfigDockingAlwaysTabBar = false;

	    ImGui_ImplGlfw_InitForOpenGL(*m_Window->GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
		LOG_INFO("ImGui Version: {0}", ImGui::GetVersion());
		//UILayer::Init();
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

}