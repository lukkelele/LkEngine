#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Application.h"

#include <imgui_impl_opengl3.h>


namespace LkEngine {

    OpenGLImGuiLayer::OpenGLImGuiLayer()
    {
    }

    OpenGLImGuiLayer::~OpenGLImGuiLayer()
    {
    }

    void OpenGLImGuiLayer::Init()
    {
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     

		io.Fonts->AddFontFromFileTTF("assets/fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		io.ConfigDockingAlwaysTabBar = false;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

        Application& app = *Application::Get();

        Window& window = app.GetWindow();
	    ImGui_ImplGlfw_InitForOpenGL(window.GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(window.GetShaderVersion().c_str());
		LK_CORE_INFO_TAG("ImGuiLayer (OpenGL)", "ImGui Version: {}", ImGui::GetVersion());

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

    void UI::Image(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImGui::Image((ImTextureID)texture->GetRendererID(), size, uv0, uv1, tint_col, border_col);
    }

    void UI::Image(const Ref<Texture2D>& texture, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, const glm::vec4& border_col)
    {
        ImGui::Image((ImTextureID)texture->GetRendererID(), ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), ImVec4(border_col.r, border_col.g, border_col.b, border_col.a));
    }

    void UI::Image(const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
    {
        ImGui::Image((ImTextureID)image->GetRendererID(), size, uv0, uv1, tint_col, border_col);
    }

    void UI::Image(const Ref<Image2D>& image, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, const glm::vec4& border_col)
    {
        ImGui::Image((ImTextureID)image->GetRendererID(), ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), ImVec4(border_col.r, border_col.g, border_col.b, border_col.a));
    }

}