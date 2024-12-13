#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

    LOpenGLImGuiLayer::LOpenGLImGuiLayer()
    {
		LOBJECT_REGISTER();

		LayoutConfig = LFileSystem::GetConfigDir() / "EditorLayout.ini";
    }

    void LOpenGLImGuiLayer::Initialize()
    {
	    ImGui::CreateContext();
		ImGuiIO& IO = ImGui::GetIO();

		/* Configuration file. */
		IO.IniFilename = nullptr; /* No config. */
		const std::string ConfigStr = LayoutConfig.string();
		ImGui::LoadIniSettingsFromDisk(ConfigStr.c_str());

		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     

		IO.ConfigDockingAlwaysTabBar = false;

	#if defined(LK_ENGINE_DEBUG)
		IO.ConfigDebugHighlightIdConflicts = true;
	#endif

		ImGuiStyle& Style = ImGui::GetStyle();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			Style.WindowRounding = 0.0f;
			Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Style.DockingSeparatorSize = 2.0f;

		/* Initialize ImGui for GLFW and OpenGL. */
        LApplication& Application = *LApplication::Get();
        LWindow& Window = Application.GetWindow();
	    ImGui_ImplGlfw_InitForOpenGL(Window.GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(Window.GetShaderVersion().c_str());
		LK_CORE_TRACE("ImGui Version: {}", ImGui::GetVersion());

		/* Add fonts. */
		LK_CORE_INFO_TAG("UI", "Adding fonts");
		InitializeFonts();

		bInitialized = true;
    }

    void LOpenGLImGuiLayer::Destroy()
    {
		LK_CORE_DEBUG_TAG("OpenGLImGuiLayer", "Destroying");

		LK_CORE_DEBUG_TAG("OpenGLImGuiLayer", "Saving editor layout configuration to: {}", LayoutConfig.string());
		ImGui::SaveIniSettingsToDisk(LayoutConfig.string().c_str());

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
		auto& Colors = ImGui::GetStyle().Colors;

		/* Window background. */
		Colors[ImGuiCol_WindowBg]		= ImVec4(0.10f, 0.105f, 0.110f, 1.0f);

		/* Headers. */
		Colors[ImGuiCol_Header]			= ImVec4(0.20f, 0.205f,  0.210f, 1.0f);
		Colors[ImGuiCol_HeaderHovered]	= ImVec4(0.30f, 0.305f,  0.310f, 1.0f);
		Colors[ImGuiCol_HeaderActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		/* Buttons. */
		Colors[ImGuiCol_Button]			= ImVec4(0.20f, 0.205f, 0.210f, 1.0f);
		Colors[ImGuiCol_ButtonHovered]	= ImVec4(0.30f, 0.305f, 0.310f, 1.0f);
		Colors[ImGuiCol_ButtonActive]	= ImVec4(0.15f, 0.150f, 0.151f, 1.0f);

		/* Frame background. */
		Colors[ImGuiCol_FrameBg]		= ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);
		Colors[ImGuiCol_FrameBgHovered]	= ImVec4(0.30f, 0.3050f, 0.310f, 1.0f);
		Colors[ImGuiCol_FrameBgActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		/* Tabs. */
		Colors[ImGuiCol_Tab]				= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TabHovered]			= ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
		Colors[ImGuiCol_TabActive]			= ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
		Colors[ImGuiCol_TabUnfocused]		= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);

		/* Title. */
		Colors[ImGuiCol_TitleBg]		  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TitleBgActive]	  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);

		/* Scrollbar. */
		Colors[ImGuiCol_ScrollbarBg]		  = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		Colors[ImGuiCol_ScrollbarGrab]		  = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

		/* Checkmark. */
		Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

		/* Slider. */
		Colors[ImGuiCol_SliderGrab]		  = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
	}

	void LOpenGLImGuiLayer::InitializeFonts()
	{
		constexpr float DefaultFontSize = 20.0f;
		constexpr float LargeFontSize   = 26.0f;

		/*----------------------------------------------------
			Source Sans Code Pro
		-----------------------------------------------------*/
		/* Default font. */
		UI::FFontConfiguration SourceSansPro_Semibold;
		SourceSansPro_Semibold.FontName = "Default";
		SourceSansPro_Semibold.FilePath = "Assets/Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Semibold.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Semibold, true);

		UI::FFontConfiguration SourceSansPro_Bold;
		SourceSansPro_Bold.FontName = "Bold";
		SourceSansPro_Bold.FilePath = "Assets/Fonts/SourceCodePro/SourceSansPro-Bold.ttf";
		SourceSansPro_Bold.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Bold);

		UI::FFontConfiguration SourceSansPro_Large;
		SourceSansPro_Large.FontName = "Large";
		SourceSansPro_Large.FilePath = "Assets/Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Large.Size = LargeFontSize;
		UI::Font::Add(SourceSansPro_Large);

		UI::FFontConfiguration SourceSansPro_Italic;
		SourceSansPro_Italic.FontName = "Italic";
		SourceSansPro_Italic.FilePath = "Assets/Fonts/SourceCodePro/SourceSansPro-Italic.ttf";
		SourceSansPro_Italic.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Italic);


		/*----------------------------------------------------
			Roboto
		-----------------------------------------------------*/
		UI::FFontConfiguration Roboto_Bold;
		Roboto_Bold.FontName = "Roboto_Bold";
		Roboto_Bold.FilePath = "Assets/Fonts/Roboto/Roboto-Bold.ttf";
		Roboto_Bold.Size = DefaultFontSize;
		UI::Font::Add(Roboto_Bold);

		UI::FFontConfiguration Roboto_Large;
		Roboto_Large.FontName = "Roboto_Large";
		Roboto_Large.FilePath = "Assets/Fonts/Roboto/Roboto-Regular.ttf";
		Roboto_Large.Size = LargeFontSize;
		UI::Font::Add(Roboto_Large);

		UI::FFontConfiguration Roboto_SemiMedium;
		Roboto_SemiMedium.FontName = "Roboto_SemiMedium";
		Roboto_SemiMedium.FilePath = "Assets/Fonts/Roboto/Roboto-SemiMedium.ttf";
		Roboto_SemiMedium.Size = DefaultFontSize;
		UI::Font::Add(Roboto_SemiMedium);
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
