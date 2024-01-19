#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Window.h"

#include <imgui_impl_opengl3.h>


namespace LkEngine {

    OpenGLImGuiLayer::OpenGLImGuiLayer()
    {
    }

    OpenGLImGuiLayer::~OpenGLImGuiLayer()
    {
    }

    void OpenGLImGuiLayer::Init(Ref<Window>& window)
    {
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		io.ConfigDockingAlwaysTabBar = false;

	    ImGui_ImplGlfw_InitForOpenGL(window->GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(window->GetShaderVersion().c_str());
		LK_INFO("ImGui Version: {}", ImGui::GetVersion());

		m_Initialized = true;
    }

    void OpenGLImGuiLayer::Shutdown()
    {
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
    }

    void OpenGLImGuiLayer::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGuizmo::BeginFrame();
    }

    void OpenGLImGuiLayer::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

}