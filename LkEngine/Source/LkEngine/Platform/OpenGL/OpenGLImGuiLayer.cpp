#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Application.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ImFileDialog/ImFileDialog.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

    LOpenGLImGuiLayer::LOpenGLImGuiLayer()
    {
		LCLASS_REGISTER();
    }

    void LOpenGLImGuiLayer::Initialize()
    {
        /// TODO: Get window config and apply settings from that.
	    ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     

		io.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceCodePro/SourceSansProSemibold.ttf", 20);
		io.ConfigDockingAlwaysTabBar = false;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

        /* Get reference to window. */
        LApplication& Application = *LApplication::Get();
        LWindow& Window = Application.GetWindow();

	    ImGui_ImplGlfw_InitForOpenGL(Window.GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(Window.GetShaderVersion().c_str());
		LK_CORE_DEBUG_TAG("ImGuiLayer - OpenGL", "ImGui Version: {}", ImGui::GetVersion());

		bInitialized = true;
    }

    void LOpenGLImGuiLayer::Shutdown()
    {
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
    }

    void LOpenGLImGuiLayer::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGuizmo::BeginFrame();
    }

    void LOpenGLImGuiLayer::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void LOpenGLImGuiLayer::SetDarkTheme()
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


    /*-----------------------------------------------------------------------------*/

    void UI::Image(const TObjectPtr<LTexture2D>& texture, 
                   const ImVec2& size, 
                   const ImVec2& uv0, 
                   const ImVec2& uv1, 
                   const ImVec4& tint_col, 
                   const ImVec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)texture->GetRendererID(), size, uv0, uv1, tint_col, BorderColumn);
    }

    void UI::Image(const TObjectPtr<LTexture2D>& texture, const glm::vec2& size, 
                   const glm::vec2& uv0, const glm::vec2& uv1, 
                   const glm::vec4& tint_col, const glm::vec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)(texture->GetRendererID()),
                     ImVec2(size.x, size.y), 
                     ImVec2(uv0.x, uv0.y), 
                     ImVec2(uv1.x, uv1.y), 
                     ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), 
                     ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

    void UI::Image(const TObjectPtr<LImage2D>& image, const ImVec2& size, 
                   const ImVec2& uv0, const ImVec2& uv1, 
                   const ImVec4& tint_col, const ImVec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)(image->GetRendererID()),
                     size, 
                     uv0, 
                     uv1, 
                     tint_col, 
                     BorderColumn);
    }

    void UI::Image(const TObjectPtr<LImage2D>& image, const glm::vec2& size, 
                   const glm::vec2& uv0, const glm::vec2& uv1, 
                   const glm::vec4& tint_col, const glm::vec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)image->GetRendererID(), 
                     ImVec2(size.x, size.y), 
                     ImVec2(uv0.x, uv0.y), 
                     ImVec2(uv1.x, uv1.y), 
                     ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), 
                     ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

}