#include "LKpch.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/Platform/Windows/Windows_Window.h"
#include "LkEngine/UI/UI.h"
#include <imgui/imgui.h>
#include <imgui_impl_opengl3.h>


namespace LkEngine {

	GraphicsContext* GraphicsContext::m_Context = nullptr;

	GraphicsContext::GraphicsContext(void* _windowHandle)
	{
		m_Context = this;
	    Window* window = static_cast<Window*>(_windowHandle);
	    m_Window = std::shared_ptr<Window>(window);
		GlfwWindow = m_Window->GetGlfwWindow();
	}
	
	void GraphicsContext::Init()
	{
		GLenum err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (err == 0)
		{
			printf("[ERROR] Error starting GLAD");
			return;
		}
		else
		{
			printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
			printf("GraphicsContext attached to window with title: %s\n", m_Window->GetTitle().c_str());
		}
	
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		InitImGui(m_Window->GetGlslVersion().c_str());
		// UI::Viewport::Init();
		UI::Init();
	}

	void GraphicsContext::Destroy()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	std::shared_ptr<GLFWwindow*> GraphicsContext::GetGlfwWindow()
	{
		return GlfwWindow;
	}
	
	
	void GraphicsContext::InitImGui(const std::string& glslVersion)
	{
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);

	    ImGui_ImplGlfw_InitForOpenGL(*m_Window->GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
		UI::Init();
	}
	
	void GraphicsContext::BeginImGuiFrame()
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		//--------------- Base UI ----------------
		// UI::Viewport::BeginViewport(); // Dockspace
		UI::BeginViewportDockSpace();
		//UI::TopBar();
		//UI::BottomBar();
        //UI::LeftSidebar();
        //UI::RightSidebar();
		//----------------------------------------
		UI::BeginMainRenderWindow(); // Drawn content
	}

	void GraphicsContext::EndImGuiFrame()
	{
		UI::EndMainRenderWindow();
		UI::EndViewportDockSpace();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	
	void GraphicsContext::SetDarkTheme()
	{
		ImGui::StyleColorsDark();
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		// Check Mark
		colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
		// Slider
		colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
	}

}
