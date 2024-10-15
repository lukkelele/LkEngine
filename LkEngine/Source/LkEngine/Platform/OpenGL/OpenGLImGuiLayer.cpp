#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Application.h"

#include <imgui_impl_opengl3.h>


namespace LkEngine {

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

        LApplication& ApplicationRef = *LApplication::Get();

        LWindow& Window = ApplicationRef.GetWindow();
	    ImGui_ImplGlfw_InitForOpenGL(Window.GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(Window.GetShaderVersion().c_str());
		LK_CORE_INFO_TAG("ImGuiLayer (OpenGL)", "ImGui Version: {}", ImGui::GetVersion());

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

    void UI::Image(const TObjectPtr<LTexture2D>& texture, 
                   const ImVec2& size, 
                   const ImVec2& uv0, 
                   const ImVec2& uv1, 
                   const ImVec4& tint_col, 
                   const ImVec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)texture->GetRendererID(), size, uv0, uv1, tint_col, BorderColumn);
    }

    void UI::Image(const TObjectPtr<LTexture2D>& texture, 
                   const glm::vec2& size, 
                   const glm::vec2& uv0, 
                   const glm::vec2& uv1, 
                   const glm::vec4& tint_col, 
                   const glm::vec4& BorderColumn)
    {
        //ImGui::Image((ImTextureID)texture->GetRendererID(), 
        ImGui::Image(reinterpret_cast<ImTextureID>(texture->GetRendererID()),
                     ImVec2(size.x, size.y), 
                     ImVec2(uv0.x, uv0.y), 
                     ImVec2(uv1.x, uv1.y), 
                     ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), 
                     ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

    void UI::Image(const TObjectPtr<LImage2D>& image, 
                   const ImVec2& size, 
                   const ImVec2& uv0, 
                   const ImVec2& uv1, 
                   const ImVec4& tint_col, 
                   const ImVec4& BorderColumn)
    {
        //ImGui::Image((ImTextureID)image->GetRendererID(), 
        ImGui::Image(reinterpret_cast<ImTextureID>(image->GetRendererID()),
                     size, 
                     uv0, 
                     uv1, 
                     tint_col, 
                     BorderColumn);
    }

    void UI::Image(const TObjectPtr<LImage2D>& image, 
                   const glm::vec2& size, 
                   const glm::vec2& uv0, 
                   const glm::vec2& uv1, 
                   const glm::vec4& tint_col, 
                   const glm::vec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)image->GetRendererID(), 
                     ImVec2(size.x, size.y), 
                     ImVec2(uv0.x, uv0.y), 
                     ImVec2(uv1.x, uv1.y), 
                     ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a), 
                     ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

}