#include "LKpch.h"
#include "OpenGLImGuiLayer.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>

#include "LkEngine/Renderer/UI/FontAwesome.h"


namespace LkEngine {

    LOpenGLImGuiLayer::LOpenGLImGuiLayer()
    {
		LOBJECT_REGISTER();

		LayoutConfig = LFileSystem::ConvertToUnixPath(LFileSystem::GetConfigDir() / "ImGuiLayout.ini");
		LK_CORE_TRACE_TAG("OpenGLImGuiLayer", "Config: {}", LayoutConfig.string());
    }

    void LOpenGLImGuiLayer::Initialize()
    {
		LObject::Initialize();

	    ImGui::CreateContext();
		ImGuiIO& IO = ImGui::GetIO();

		/* Configuration file. */
		IO.IniFilename = nullptr; /* No config. */
		const std::string ConfigStr = LayoutConfig.string();
		ImGui::LoadIniSettingsFromDisk(ConfigStr.c_str());

		//IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
		IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
		//IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     

		IO.ConfigDockingAlwaysTabBar = false;

	#if defined(LK_UI_HIGHLIGHT_ID_CONFLICTS)
		IO.ConfigDebugHighlightIdConflicts = true;
	#else
		IO.ConfigDebugHighlightIdConflicts = false;
	#endif

		ImGuiStyle& Style = ImGui::GetStyle();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			Style.WindowRounding = 0.0f;
			Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Style.DockingSeparatorSize = 2.0f;
		Style.WindowMenuButtonPosition = ImGuiDir_Right;

		/* Initialize ImGui for GLFW and OpenGL. */
        LWindow& Window = LApplication::Get().GetWindow();
	    ImGui_ImplGlfw_InitForOpenGL(Window.GetGlfwWindow(), true);
	    ImGui_ImplOpenGL3_Init(Window.GetShaderVersion().c_str());
		LK_CORE_TRACE_TAG("OpenGLImGuiLayer", "ImGui Version: {}", ImGui::GetVersion());

		/* Add fonts. */
		LK_CORE_TRACE_TAG("UI", "Adding fonts");
		InitializeFonts();

		Window.OnWindowSizeUpdated.Add([&](const uint16_t NewWidth, const uint16_t NewHeight)
		{
			ImGuiIO& IO = ImGui::GetIO();
			IO.DisplaySize = ImVec2(NewWidth, NewHeight);
		});
    }

    void LOpenGLImGuiLayer::Destroy()
    {
		LK_CORE_DEBUG_TAG("OpenGLImGuiLayer", "Saving UI configuration: {}", 
						  std::filesystem::relative(LayoutConfig, LFileSystem::GetEngineDir()).string());
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
		constexpr float LargerFontSize  = 24.0f;
		constexpr float LargeFontSize   = 30.0f;
		constexpr float TitleFontSize   = 42.0f;

		/*----------------------------------------------------
			Source Sans Code Pro
		-----------------------------------------------------*/
		/* Default font. */
		FFontConfiguration SourceSansPro_Semibold;
		SourceSansPro_Semibold.FontName = "Default";
		SourceSansPro_Semibold.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Semibold.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Semibold, true);

		/* FontAwesome. */
		static const ImWchar FontAwesomeRanges[] = { LK_ICON_MIN, LK_ICON_MAX, 0 };
		FFontConfiguration FontAwesome;
		FontAwesome.FontName = "FontAwesome";
		FontAwesome.FilePath = LFileSystem::GetResourcesDir() / "Fonts/FontAwesome/fontawesome-webfont.ttf";
		FontAwesome.Size = 16.0f;
		FontAwesome.GlyphRanges = FontAwesomeRanges;
		FontAwesome.MergeWithLast = true;
		UI::Font::Add(FontAwesome);

		FFontConfiguration SourceSansPro_Bold;
		SourceSansPro_Bold.FontName = "Bold";
		SourceSansPro_Bold.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Bold.ttf";
		SourceSansPro_Bold.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Bold);

		FFontConfiguration SourceSansPro_Large;
		SourceSansPro_Large.FontName = "Large";
		SourceSansPro_Large.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Large.Size = LargeFontSize;
		UI::Font::Add(SourceSansPro_Large);

		FFontConfiguration SourceSansPro_Larger;
		SourceSansPro_Larger.FontName = "Larger";
		SourceSansPro_Larger.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Larger.Size = LargerFontSize;
		UI::Font::Add(SourceSansPro_Larger);

		FFontConfiguration SourceSansPro_LargeBold;
		SourceSansPro_LargeBold.FontName = "LargeBold";
		SourceSansPro_LargeBold.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Bold.ttf";
		SourceSansPro_LargeBold.Size = LargeFontSize;
		UI::Font::Add(SourceSansPro_LargeBold);

		FFontConfiguration SourceSansPro_LargeBoldItalic;
		SourceSansPro_LargeBoldItalic.FontName = "LargeBoldItalic";
		SourceSansPro_LargeBoldItalic.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-BoldItalic.ttf";
		SourceSansPro_LargeBoldItalic.Size = LargeFontSize;
		UI::Font::Add(SourceSansPro_LargeBoldItalic);

		FFontConfiguration SourceSansPro_Italic;
		SourceSansPro_Italic.FontName = "Italic";
		SourceSansPro_Italic.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Italic.ttf";
		SourceSansPro_Italic.Size = DefaultFontSize;
		UI::Font::Add(SourceSansPro_Italic);

		FFontConfiguration SourceSansPro_LargeItalic;
		SourceSansPro_LargeItalic.FontName = "LargeItalic";
		SourceSansPro_LargeItalic.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Italic.ttf";
		SourceSansPro_LargeItalic.Size = LargeFontSize;
		UI::Font::Add(SourceSansPro_LargeItalic);

		FFontConfiguration SourceSansPro_Title;
		SourceSansPro_Title.FontName = "Title";
		SourceSansPro_Title.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Semibold.ttf";
		SourceSansPro_Title.Size = TitleFontSize;
		UI::Font::Add(SourceSansPro_Title);

		FFontConfiguration SourceSansPro_TitleBold;
		SourceSansPro_TitleBold.FontName = "TitleBold";
		SourceSansPro_TitleBold.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Bold.ttf";
		SourceSansPro_TitleBold.Size = TitleFontSize;
		UI::Font::Add(SourceSansPro_TitleBold);

		FFontConfiguration SourceSansPro_TitleBoldItalic;
		SourceSansPro_TitleBoldItalic.FontName = "TitleBoldItalic";
		SourceSansPro_TitleBoldItalic.FilePath = LFileSystem::GetResourcesDir() / "Fonts/SourceCodePro/SourceSansPro-Bold.ttf";
		SourceSansPro_TitleBoldItalic.Size = TitleFontSize;
		UI::Font::Add(SourceSansPro_TitleBoldItalic);

		/*----------------------------------------------------
			Roboto
		-----------------------------------------------------*/
		FFontConfiguration Roboto_Bold;
		Roboto_Bold.FontName = "Roboto_Bold";
		Roboto_Bold.FilePath = LFileSystem::GetResourcesDir() / "Fonts/Roboto/Roboto-Bold.ttf";
		Roboto_Bold.Size = DefaultFontSize;
		UI::Font::Add(Roboto_Bold);

		FFontConfiguration Roboto_Large;
		Roboto_Large.FontName = "Roboto_Large";
		Roboto_Large.FilePath = LFileSystem::GetResourcesDir() / "Fonts/Roboto/Roboto-Regular.ttf";
		Roboto_Large.Size = LargeFontSize;
		UI::Font::Add(Roboto_Large);

		FFontConfiguration Roboto_SemiMedium;
		Roboto_SemiMedium.FontName = "Roboto_SemiMedium";
		Roboto_SemiMedium.FilePath = LFileSystem::GetResourcesDir() / "Fonts/Roboto/Roboto-SemiMedium.ttf";
		Roboto_SemiMedium.Size = DefaultFontSize;
		UI::Font::Add(Roboto_SemiMedium);
	}

    /*-----------------------------------------------------------------------------*/

    void UI::Image(const TObjectPtr<LTexture2D>& Texture, 
                   const ImVec2& Size, 
                   const ImVec2& UV0, 
                   const ImVec2& UV1, 
                   const ImVec4& TintColor, 
                   const ImVec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)Texture->GetRendererID(), Size, UV0, UV1, TintColor, BorderColumn);
    }

    void UI::Image(const TObjectPtr<LTexture2D>& Texture, 
				   const glm::vec2& Size, 
                   const glm::vec2& UV0, 
				   const glm::vec2& UV1, 
                   const glm::vec4& TintColor, 
				   const glm::vec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)(Texture->GetRendererID()),
					 ImVec2(Size.x, Size.y), 
					 ImVec2(UV0.x, UV0.y), 
					 ImVec2(UV1.x, UV1.y), 
					 ImVec4(TintColor.r, TintColor.g, TintColor.b, TintColor.a), 
					 ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

    void UI::Image(const TObjectPtr<LImage2D>& Image, 
				   const ImVec2& Size, 
				   const ImVec2& UV0, 
				   const ImVec2& UV1, 
                   const ImVec4& TintColor, 
				   const ImVec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)(Image->GetRendererID()),
                     Size, 
                     UV0, 
                     UV1, 
                     TintColor, 
                     BorderColumn);
    }

    void UI::Image(const TObjectPtr<LImage2D>& Image, 
				   const glm::vec2& Size, 
                   const glm::vec2& UV0, 
				   const glm::vec2& UV1, 
                   const glm::vec4& TintColor, 
				   const glm::vec4& BorderColumn)
    {
        ImGui::Image((ImTextureID)Image->GetRendererID(), 
                     ImVec2(Size.x, Size.y), 
                     ImVec2(UV0.x, UV0.y), 
                     ImVec2(UV1.x, UV1.y), 
                     ImVec4(TintColor.r, TintColor.g, TintColor.b, TintColor.a), 
                     ImVec4(BorderColumn.r, BorderColumn.g, BorderColumn.b, BorderColumn.a));
    }

}
