#include "LKpch.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/Platform/Windows/Windows_Window.h"
#include "LkEngine/UI/UI.h"


namespace LkEngine {

	GraphicsContext* GraphicsContext::m_Context = nullptr;

	GraphicsContext::GraphicsContext(void* window_handle)
	{
		m_Context = this;
	    Window* window = static_cast<Window*>(window_handle);
	    m_Window = std::shared_ptr<Window>(window);
		m_GlfwWindow = m_Window->GetGlfwWindow();
		
		m_MainRenderWindowSize = ImVec2(0, 0);
	}
	
	void GraphicsContext::Init()
	{
		GLenum err = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (err == 0)
		{
			printf("[ERROR] Error starting GLAD");
			exit(EXIT_FAILURE);
		}
		printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
		glEnable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST); 
		glEnable(GL_LINE_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	
		InitImGui(m_Window->GetGlslVersion().c_str());
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
		return m_GlfwWindow;
	}
	
	void GraphicsContext::InitImGui(const std::string& glslVersion)
	{
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		io.ConfigViewportsNoDecoration = false;
		io.ConfigWindowsResizeFromEdges = false;
		io.ConfigDockingAlwaysTabBar = false;

	    ImGui_ImplGlfw_InitForOpenGL(*m_Window->GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
		LOG_INFO("ImGui Version: {0}", ImGui::GetVersion());
		UI::Init();
	}
	
	void GraphicsContext::BeginImGuiFrame()
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		//--------------- Base UI ----------------
		UI::BeginViewportDockSpace();
		UI::TopBar();
		UI::BottomBar();
        UI::LeftSidebar();
        UI::RightSidebar();

		UI::AppInfo();

		UI::BeginMainRenderWindow(); 
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		m_MainRenderWindowSize = window->Size;
		m_MainRenderWindowPos = window->Pos;
	}

	void GraphicsContext::EndImGuiFrame()
	{
		auto ctx = GraphicsContext::Get();
		auto size = ctx->GetMainRenderWindowSize();
		auto pos = ctx->GetMainRenderWindowPos();
		glViewport(pos.x, pos.y, size.x, size.y);
		//LOG_DEBUG("Pos: ({}, {})  Size: ({}, {})", pos.x, pos.y, size.x, size.y);

		UI::EndMainRenderWindow();

		UI::EndViewportDockSpace();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GraphicsContext::HandleViewportEvents()
	{
		auto window = Window::Get();
		int viewport_width, viewport_height;
		glfwGetWindowSize(*window->GetGlfwWindow(), &viewport_width, &viewport_height);
	}

	ImVec2 GraphicsContext::GetMainRenderWindowSize()
	{
		ImGuiDockNode* center_node = ImGui::DockBuilderGetNode(UI::RenderWindowDockID);
		LK_ASSERT(center_node);
		return center_node->Size;
	}

	ImVec2 GraphicsContext::GetMainRenderWindowPos()
	{
		ImGuiDockNode* center_node = ImGui::DockBuilderGetNode(UI::RenderWindowDockID);
		ImGuiDockNode* bottom_node = ImGui::DockBuilderGetNode(UI::BottomBarDockID);
		LK_ASSERT(center_node);
		LK_ASSERT(bottom_node);
		//LOG_DEBUG("Bottom Node Size: ({} {})", bottom_node->Size.x, bottom_node->Size.y);
		ImVec2 pos = ImVec2(center_node->Pos.x, bottom_node->Size.y);
		return pos;
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
