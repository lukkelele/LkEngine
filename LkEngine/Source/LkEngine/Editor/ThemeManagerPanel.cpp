#include "LKpch.h"
#include "ThemeManagerPanel.h"

#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

	inline static LThemeManagerPanel* Instance{};

	namespace 
	{
		constexpr int THEME_NAME_LENGTH_MIN = 4;
		constexpr int THEME_NAME_LENGTH_MAX = 120;
		constexpr int THEME_FILEPATH_LENGTH_MAX = 512;

		/**
		 * Themes array for the dropdown. 
		 * Subtract one for the removal of the 'Custom' entry.
		 */
		std::array<const char*, (uint16_t)ETheme::COUNT> Themes;

		/* ImGuiColors. */
		std::array<const char*, (int)ImGuiCol_COUNT> ImGuiColors;

		ImGuiColorEditFlags EditFlags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar;
		ImGuiTableFlags GridFlags = ImGuiTableFlags_SizingFixedSame;
		bool bGridTable_UseHeaderLabels = false;
	}

	namespace InputBuffer 
	{
		char* ThemeName     = new char[THEME_NAME_LENGTH_MAX];
		char* ThemeFilePath = new char[THEME_FILEPATH_LENGTH_MAX];
	}

	static int OnInputTextCallback(ImGuiInputTextCallbackData* Data)
	{
		LK_CORE_WARN("ThemeName Input Text: {}", Data->Buf);
		return 0;
	}

	static constexpr std::array<UI::TPropertyMapping<uint32_t>, 7> ColorReferences = {
		std::pair("White",     RGBA32::White),
		std::pair("Black",     RGBA32::Black),
		std::pair("Gray",      RGBA32::Gray),
		std::pair("Red",       RGBA32::Red),
		std::pair("Green",     RGBA32::Green),
		std::pair("Blue",      RGBA32::Blue),
		std::pair("Lightblue", RGBA32::NiceBlue)
	};

	LThemeManagerPanel::LThemeManagerPanel()
	{
		LK_CORE_VERIFY(Instance == nullptr);
		Instance = this;
		LPANEL_REGISTER();

		/* Initialize enum arrays. */
		if (Themes.at(0) == nullptr)
		{
			int Idx = 0;
			for (const ETheme Theme : TEnumRange<ETheme>())
			{
				LK_CORE_TRACE("Adding theme to array: {}", Enum::ToString(Theme));
				LK_CORE_ASSERT(Idx != Themes.size());
				Themes[Idx++] = Enum::ToString(Theme);
			}
		}

		if (ImGuiColors.at(0) == nullptr)
		{
			int Idx = 0;
			for (const ImGuiCol_ Color : TEnumRange<ImGuiCol_>())
			{
				LK_CORE_TRACE("Adding ImGuiColor to array: {} ({})", Enum::ToString(Color), (int)Color);
				LK_CORE_ASSERT(Idx != ImGuiColors.size());
				ImGuiColors[Idx++] = Enum::ToString(Color);
			}
		}

		ThemesDirectory = LFileSystem::GetResourcesDir() / "Themes";
		if (!LFileSystem::Exists(ThemesDirectory))
		{
			LFileSystem::CreateDirectory(ThemesDirectory);
			/* Store the default themes in the directory. */
			/* TODO: Serialize the default themes here. */
		}

		std::memset(InputBuffer::ThemeName, 0, THEME_NAME_LENGTH_MAX);
		std::sprintf(
			InputBuffer::ThemeFilePath, 
			"%s", 
			LFileSystem::ConvertToUnixPath(std::filesystem::relative(ThemesDirectory, LFileSystem::GetEngineDir())).c_str()
		);
	}

	void LThemeManagerPanel::RenderUI(bool& IsOpen)
	{
		ImGuiContext& G = *GImGui;
		auto& Colors = ImGui::GetStyle().Colors;

		ImGui::SetNextWindowPos(ImVec2(180, 120), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(540, 780), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(440, 200), UI::SizeConstraint<ImVec2>::Max);

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (!UI::Begin(PanelID::ThemeManager, &IsOpen, WindowFlags))
		{
			return;
		}

		UI::Property("Item Selector", bSelectorEnabled);

		if (ImGui::Button("Popup: Save Theme"))
		{
			UI_SaveThemePopup();
		}
		ImGui::SameLine(0, 8.0f);
		if (ImGui::Button("Popup: Select Theme"))
		{
			UI_SelectThemePopup();
		}

		if (ImGui::Button("Save Current Theme"))
		{
			LK_CORE_INFO("Saving current theme");
			SaveCurrentTheme();
		}

		ImGui::Text("Theme Name: %s", CurrentThemeName.c_str());

		const ImVec2 ModifierTableSize = ImMin(ImVec2(640, 0.0f), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("ImGui Colors"))
		{
			static constexpr float ColumnWidth = 100.0f;
			UI::FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(G.Style.CellPadding.x, 0.0f));

			UI::BeginPropertyGrid("##ImGuiColors", ModifierTableSize, ImGuiTableFlags_SizingStretchProp);
			for (const ImGuiCol_ ImGuiColor : TEnumRange<ImGuiCol_>())
			{
				UI::Draw::Vec4Control<UI::EVectorSemantic::XYZW>(Enum::ToString(ImGuiColor), Colors[ImGuiColor], 0.010f, 1.0f, 0.0f, 1.0f);
			}
			UI::EndPropertyGrid();

			ImGui::TreePop();
		}

		UI_ColorPicker();

		UI::End();
	}

	void LThemeManagerPanel::SetSelectorEnabled(const bool InEnabled)
	{
		bSelectorEnabled = InEnabled;
	}

	bool LThemeManagerPanel::SaveCurrentTheme()
	{
		std::filesystem::path ThemePath;

		/**
		 * Step 1: Determine if a theme already exists with the current theme name.
		 *         Prompt the user for a theme name if none exists.
		 */
		std::string ThemeFileName = CurrentThemeName;

		const std::size_t ThemeNameLen = CurrentThemeName.size();
		if (ThemeNameLen <= 0)
		{
			/* Check if the file exists. */
			std::vector<std::filesystem::path> SimilarFiles;
			if (LFileSystem::FindSimilarFiles("Custom", ThemesDirectory, SimilarFiles))
			{
				LK_CORE_DEBUG_TAG("ThemeManager", "Found {} files in directory: '{}'", SimilarFiles.size(), LFileSystem::GetWorkingDir());
				ThemeFileName = std::format("Custom-%{}", SimilarFiles.size());
			}
			else
			{
				LK_CORE_ERROR_TAG("ThemeManager", "Failed to find files similar with name 'Custom'");
				ThemeFileName = "Custom-1";
			}
		}
		else
		{
			bool IsDefaultTheme = false;
			for (const ETheme Theme : TEnumRange<ETheme>())
			{
				if (CurrentThemeName == Enum::ToString(Theme))
				{
					IsDefaultTheme = true;
					break;
				}
			}

			if (IsDefaultTheme)
			{
				int Num = 1;
				std::string ThemeFile = std::format("{}-{}.{}", CurrentThemeName, Num, THEME_FILE_EXTENSION);
				while (LFileSystem::Exists(ThemeFile))
				{
					Num++;
					ThemeFile = std::format("{}-{}.{}", CurrentThemeName, Num, THEME_FILE_EXTENSION);
					LK_CORE_TRACE("-> Theme file: {}", ThemeFile);
				}

				/* Remove .lktheme from the string. */
				ThemeFileName = ThemeFile.substr(0, ThemeFile.find(THEME_FILE_EXTENSION) - 1);
				LK_CORE_DEBUG("Theme filename: {}", ThemeFileName);
			}
		}

		LK_CORE_ASSERT(!ThemeFileName.empty(), "Theme filename is empty");
		ThemePath = ThemesDirectory / std::format("{}.{}", ThemeFileName, THEME_FILE_EXTENSION);
		LK_CORE_INFO_TAG("ThemeManager", "Saving current theme: {} ({})", CurrentThemeName, ThemePath);
		if (ThemePath.empty())
		{
			LK_CORE_ERROR_TAG("ThemeManager", "Theme path is empty, cannot save current theme");
			return false;
		}

		CurrentThemeName = ThemeFileName;
		std::memcpy(InputBuffer::ThemeFilePath, CurrentThemeName.data(), CurrentThemeName.length());
		if (LFileSystem::Exists(ThemePath))
		{
			LK_CORE_INFO("Overwriting existing theme: {}", ThemePath);
		}

		const std::string ThemeNameStr = ThemeFileName;
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		LK_SERIALIZE_PROPERTY(Theme, ThemeNameStr, Out);

		Out << YAML::Key << "Colors" << YAML::BeginMap << YAML::Value;
		auto& Colors = ImGui::GetStyle().Colors;
		for (const ImGuiCol_ ImGuiColor : TEnumRange<ImGuiCol_>())
		{
			LK_SERIALIZE_PROPERTY_VALUE(Enum::ToString(ImGuiColor), Colors[ImGuiColor], Out);
		}

		Out << YAML::EndMap;
		Out << YAML::EndMap;

		std::ofstream FileOut(ThemePath);
		FileOut << Out.c_str();

		return true;
	}

	bool LThemeManagerPanel::LoadSavedTheme(std::string_view ThemeName)
	{
		if (ThemeName.empty())
		{
			LK_CORE_ERROR_TAG("ThemeManager", "Cannot load theme, passed name is empty");
			return false;
		}

		for (const ETheme Theme : TEnumRange<ETheme>())
		{
			if (ThemeName == Enum::ToString(Theme))
			{
				LK_CORE_INFO("The theme '{}' is a default theme, loading it", Enum::ToString(Theme));
				SetTheme(Theme);
				return true;
			}
		}

		/* Store a string for the theme name with the file extension and without it. */
		std::string ThemeNameStr(ThemeName);
		std::string ThemeNameWithoutExtension = ThemeNameStr;
		if (const std::size_t Pos = ThemeNameStr.find(THEME_FILE_EXTENSION); Pos != std::string::npos)
		{
			ThemeNameWithoutExtension = ThemeNameStr.substr(0, Pos - 1);
		}
		if (ThemeNameStr.find(THEME_FILE_EXTENSION) == std::string::npos)
		{
			ThemeNameStr += std::format(".{}", THEME_FILE_EXTENSION);
		}

		const std::filesystem::path ThemePath(ThemesDirectory / ThemeNameStr);
		LK_CORE_DEBUG("Loading saved theme: {} ({})", ThemeNameStr, ThemePath);
		if (!LFileSystem::Exists(ThemePath))
		{
			LK_CORE_WARN_TAG("ThemeManager", "Cannot load theme {}, the file doesn't exist ({})", ThemeName, ThemePath);
			return false;
		}

		std::ifstream InputStream(ThemePath);
		LK_CORE_VERIFY(InputStream, "Invalid filestream: {}", ThemePath);
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();
		InputStream.close();

		const YAML::Node Data = YAML::Load(StringStream.str());
		Instance->LoadThemeFromYaml(Data);
		
		CurrentThemeName = ThemeNameWithoutExtension;
		std::memcpy(InputBuffer::ThemeName, CurrentThemeName.data(), CurrentThemeName.length());

		return true;
	}

	void LThemeManagerPanel::OnMouseButtonPressed(const FMouseButtonData& ButtonData)
	{
		LK_CORE_TRACE_TAG("ThemeManager", "OnMouseButtonPressed: {}", Enum::ToString(ButtonData.Button));
	}

	void LThemeManagerPanel::SetTheme(const ETheme Theme)
	{
		if (Theme == ETheme::Dark)
		{
			CurrentThemeName = "Dark";

			ImGui::StyleColorsDark();
			auto& Colors = ImGui::GetStyle().Colors;

			Colors[ImGuiCol_WindowBg]		      = ImVec4(0.10f, 0.105f, 0.110f, 1.0f);
			Colors[ImGuiCol_Header]			      = ImVec4(0.20f, 0.205f,  0.210f, 1.0f);
			Colors[ImGuiCol_HeaderHovered]	      = ImVec4(0.30f, 0.305f,  0.310f, 1.0f);
			Colors[ImGuiCol_HeaderActive]	      = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_Button]			      = ImVec4(0.20f, 0.205f, 0.210f, 1.0f);
			Colors[ImGuiCol_ButtonHovered]	      = ImVec4(0.30f, 0.305f, 0.310f, 1.0f);
			Colors[ImGuiCol_ButtonActive]	      = ImVec4(0.15f, 0.150f, 0.151f, 1.0f);
			Colors[ImGuiCol_FrameBg]		      = ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);
			Colors[ImGuiCol_FrameBgHovered]	      = ImVec4(0.30f, 0.3050f, 0.310f, 1.0f);
			Colors[ImGuiCol_FrameBgActive]	      = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_Tab]				  = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabHovered]			  = ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
			Colors[ImGuiCol_TabActive]			  = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
			Colors[ImGuiCol_TabUnfocused]		  = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);
			Colors[ImGuiCol_TitleBg]		      = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgActive]	      = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_ScrollbarBg]		  = ImVec4(0.020f, 0.020f, 0.020f, 0.53f);
			Colors[ImGuiCol_ScrollbarGrab]		  = ImVec4(0.310f, 0.310f, 0.310f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.410f, 0.410f, 0.410f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.510f, 0.510f, 0.510f, 1.0f);
			Colors[ImGuiCol_CheckMark]            = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
			Colors[ImGuiCol_SliderGrab]		      = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
			Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		}
		else if (Theme == ETheme::Light)
		{
			CurrentThemeName = "Light";

			ImGui::StyleColorsLight();
			auto& Colors = ImGui::GetStyle().Colors;
		}
		else if (Theme == ETheme::GruvboxMaterial)
		{
			CurrentThemeName = "GruvboxMaterial";

			auto& Colors = ImGui::GetStyle().Colors;
			Colors[ImGuiCol_Text]                    = ImVec4(0.9216f, 0.8588f, 0.6980f, 1.0000f);
			Colors[ImGuiCol_TextDisabled]            = ImVec4(0.5726f, 0.5137f, 0.4549f, 1.0000f);
			Colors[ImGuiCol_WindowBg]                = ImVec4(0.1137f, 0.1255f, 0.1294f, 0.9412f);
			Colors[ImGuiCol_ChildBg]                 = ImVec4(0.1137f, 0.1255f, 0.1294f, 1.0000f);
			Colors[ImGuiCol_PopupBg]                 = ImVec4(0.1137f, 0.1255f, 0.1294f, 0.9412f);
			Colors[ImGuiCol_Border]                  = ImVec4(0.1137f, 0.1255f, 0.1294f, 1.0000f);
			Colors[ImGuiCol_BorderShadow]            = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
			Colors[ImGuiCol_FrameBg]                 = ImVec4(0.2353f, 0.2196f, 0.2118f, 0.5647f);
			Colors[ImGuiCol_FrameBgHovered]          = ImVec4(0.3137f, 0.2863f, 0.2706f, 1.0000f);
			Colors[ImGuiCol_FrameBgActive]           = ImVec4(0.4000f, 0.3608f, 0.3294f, 0.6588f);
			Colors[ImGuiCol_TitleBg]                 = ImVec4(0.3200f, 0.2800f, 0.2200f, 1.0000f);
			Colors[ImGuiCol_TitleBgActive]           = ImVec4(0.3200f, 0.2800f, 0.2200f, 1.0000f);
			Colors[ImGuiCol_TitleBgCollapsed]        = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.6118f);
			Colors[ImGuiCol_MenuBarBg]               = ImVec4(0.1569f, 0.1569f, 0.1569f, 0.9412f);
			Colors[ImGuiCol_ScrollbarBg]             = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.1569f);
			Colors[ImGuiCol_ScrollbarGrab]           = ImVec4(0.2353f, 0.2196f, 0.2118f, 1.0000f);
			Colors[ImGuiCol_ScrollbarGrabHovered]    = ImVec4(0.3137f, 0.2863f, 0.2706f, 1.0000f);
			Colors[ImGuiCol_ScrollbarGrabActive]     = ImVec4(0.4000f, 0.3608f, 0.3294f, 1.0000f);
			Colors[ImGuiCol_CheckMark]               = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.6196f);
			Colors[ImGuiCol_SliderGrab]              = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.4392f);
			Colors[ImGuiCol_SliderGrabActive]        = ImVec4(0.9961f, 0.5020f, 0.0980f, 1.0000f);
			Colors[ImGuiCol_Button]                  = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.4000f);
			Colors[ImGuiCol_ButtonHovered]           = ImVec4(0.9961f, 0.5020f, 0.0980f, 0.6196f);
			Colors[ImGuiCol_ButtonActive]            = ImVec4(0.9961f, 0.5020f, 0.0980f, 1.0000f);
			Colors[ImGuiCol_Header]                  = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.4000f);
			Colors[ImGuiCol_HeaderHovered]           = ImVec4(0.9961f, 0.5020f, 0.0980f, 0.8000f);
			Colors[ImGuiCol_HeaderActive]            = ImVec4(0.9961f, 0.5020f, 0.0980f, 1.0000f);
			Colors[ImGuiCol_Separator]               = ImVec4(0.4000f, 0.3608f, 0.3294f, 0.5000f);
			Colors[ImGuiCol_SeparatorHovered]        = ImVec4(0.3137f, 0.2863f, 0.2706f, 0.7800f);
			Colors[ImGuiCol_SeparatorActive]         = ImVec4(0.4000f, 0.3608f, 0.3294f, 1.0000f);
			Colors[ImGuiCol_ResizeGrip]              = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.2510f);
			Colors[ImGuiCol_ResizeGripHovered]       = ImVec4(0.9961f, 0.5020f, 0.0980f, 0.6667f);
			Colors[ImGuiCol_ResizeGripActive]        = ImVec4(0.9961f, 0.5020f, 0.0980f, 0.9490f);
			Colors[ImGuiCol_Tab]                     = ImVec4(0.8392f, 0.3647f, 0.0549f, 0.8471f);
			Colors[ImGuiCol_TabHovered]              = ImVec4(0.9961f, 0.5020f, 0.0980f, 0.8000f);
			Colors[ImGuiCol_TabActive]               = ImVec4(0.9961f, 0.5020f, 0.0980f, 1.0000f);
			Colors[ImGuiCol_TabUnfocused]            = ImVec4(0.1137f, 0.1255f, 0.1294f, 0.9700f);
			Colors[ImGuiCol_TabUnfocusedActive]      = ImVec4(0.1137f, 0.1255f, 0.1294f, 1.0000f);
			Colors[ImGuiCol_PlotLines]               = ImVec4(0.8392f, 0.3647f, 0.0549f, 1.0000f);
			Colors[ImGuiCol_PlotLinesHovered]        = ImVec4(0.9961f, 0.5020f, 0.0980f, 1.0000f);
			Colors[ImGuiCol_PlotHistogram]           = ImVec4(0.5961f, 0.5922f, 0.1020f, 1.0000f);
			Colors[ImGuiCol_PlotHistogramHovered]    = ImVec4(0.7216f, 0.7333f, 0.1490f, 1.0000f);
			Colors[ImGuiCol_TextSelectedBg]          = ImVec4(0.2706f, 0.5216f, 0.5333f, 0.9412f);
			Colors[ImGuiCol_DragDropTarget]          = ImVec4(0.5961f, 0.5922f, 0.1020f, 0.9000f);
			Colors[ImGuiCol_TableHeaderBg]           = ImVec4(0.2196f, 0.2353f, 0.2118f, 1.0000f);
			Colors[ImGuiCol_TableBorderStrong]       = ImVec4(0.1569f, 0.1569f, 0.1569f, 1.0000f);
			Colors[ImGuiCol_TableBorderLight]        = ImVec4(0.2196f, 0.2353f, 0.2118f, 1.0000f);
			Colors[ImGuiCol_TableRowBg]              = ImVec4(0.1137f, 0.1255f, 0.1294f, 1.0000f);
			Colors[ImGuiCol_TableRowBgAlt]           = ImVec4(0.1569f, 0.1569f, 0.1569f, 1.0000f);
			Colors[ImGuiCol_NavHighlight]            = ImVec4(0.5137f, 0.6471f, 0.5961f, 1.0000f);
			Colors[ImGuiCol_NavWindowingHighlight]   = ImVec4(0.9843f, 0.9451f, 0.7804f, 0.6980f);
			Colors[ImGuiCol_NavWindowingDimBg]       = ImVec4(0.4863f, 0.4353f, 0.3922f, 0.2000f);
			Colors[ImGuiCol_ModalWindowDimBg]        = ImVec4(0.1137f, 0.1255f, 0.1294f, 0.3490f);
		}
	}

	void LThemeManagerPanel::LoadDefaultTheme()
	{
		SetTheme(ETheme::Dark);
	}

	void LThemeManagerPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		/* Empty for now, this is for serialization of the actual panel window. */
	}

	void LThemeManagerPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		/* Empty for now, this is for serialization of the actual panel window. */
	}

	void LThemeManagerPanel::LoadThemeFromYaml(const YAML::Node& Data)
	{
		LK_CORE_ASSERT(Data["Theme"], "Node 'Theme' is missing");
		LK_CORE_ASSERT(Data["Colors"], "Node 'Colors' is missing");
		const YAML::Node& ColorsNode = Data["Colors"];
		auto& Colors = ImGui::GetStyle().Colors;
		for (const ImGuiCol_ ImGuiColor : TEnumRange<ImGuiCol_>())
		{
			LK_DESERIALIZE_PROPERTY_VALUE(Enum::ToString(ImGuiColor), Colors[ImGuiColor], ColorsNode, Colors[ImGuiColor]);
		}
	}

	void LThemeManagerPanel::UI_SaveThemePopup()
	{
		static constexpr uint16_t PopupWidth = 520;
		UI::ShowMessageBox("Save Theme", [&]()
		{
			UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
			UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 3.0f);

			UI::Font::Push("Bold");
			std::string FullThemePath;
			if (std::strlen(InputBuffer::ThemeFilePath) > 0)
			{
				FullThemePath = std::string(InputBuffer::ThemeFilePath) + "/" + std::string(InputBuffer::ThemeName);
			}

			LFileSystem::ConvertToUnixPath(FullThemePath);
			ImGui::Text("Theme: %s", FullThemePath.c_str());
			UI::Font::Pop();

			const bool bIsDefaultTheme = IsDefaultTheme(InputBuffer::ThemeName);
			const std::size_t ThemeNameLen = std::strlen(InputBuffer::ThemeName);
			const bool ThemeCanBeSaved = (!bIsDefaultTheme && (ThemeNameLen > 0));
			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##NewThemeName", "Theme Name", InputBuffer::ThemeName, THEME_NAME_LENGTH_MAX);

			ImGuiStyle& Style = ImGui::GetStyle();
			const ImVec2 LabelSize = ImGui::CalcTextSize("...", nullptr, true);
			const ImVec2 ButtonSize = ImGui::CalcItemSize(
				ImVec2(0, 0), 
				(LabelSize.x + Style.FramePadding.x * 2.0f), 
				(LabelSize.y + Style.FramePadding.y * 2.0f)
			);

			ImGui::SetNextItemWidth((PopupWidth - 10) - ButtonSize.x - Style.FramePadding.x * 2.0f - Style.ItemInnerSpacing.x - 1);
			ImGui::InputTextWithHint("##NewThemeLocation", "Location", InputBuffer::ThemeFilePath, THEME_NAME_LENGTH_MAX);

			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				std::string ThemeLocation = LFileSystem::OpenFolderDialog().string();
				LFileSystem::ConvertToPlatformPath(ThemeLocation);
				std::memcpy(InputBuffer::ThemeFilePath, ThemeLocation.data(), ThemeLocation.length());
			}

			UI::Font::Push("Bold");

			if (!ThemeCanBeSaved)
			{
				ImGui::BeginDisabled(true);
			}
			if (ImGui::Button("Save"))
			{
				CurrentThemeName = InputBuffer::ThemeName;
				SaveCurrentTheme();
				ImGui::CloseCurrentPopup();
			}
			if (!ThemeCanBeSaved)
			{
				ImGui::EndDisabled();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				LK_CORE_CONSOLE_INFO("Cancelled creation of new project");
				std::memset(InputBuffer::ThemeFilePath, 0, THEME_NAME_LENGTH_MAX);
				ImGui::CloseCurrentPopup();
			}
			UI::Font::Pop();

		}, (uint32_t)EMessageBoxFlag::AutoSize, PopupWidth);
	}

	void LThemeManagerPanel::UI_SelectThemePopup()
	{
		static constexpr uint16_t PopupWidth = 540;
		UI::ShowMessageBox("Theme Selection", [&]()
		{
			ImGuiContext& G = *GImGui;
			bool ShouldClosePopup = false;

			UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
			UI::FScopedStyle FrameRounding(ImGuiStyleVar_FrameRounding, 3.0f);

			{
				const ImVec2 CursorPos = ImGui::GetCursorPos();
				const ImVec2 AvailContent = ImGui::GetContentRegionAvail();
				static const char* DebugMarker = "[DEBUG]";
				UI::ShiftCursorX(AvailContent.x - ImGui::CalcTextSize(DebugMarker).x - (G.Style.FramePadding.x * 0.50f));
				ImGui::TextDisabled(DebugMarker);
				if (ImGui::IsItemHovered())
				{
					static constexpr float WrapPosOffset = 35.0f;
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * WrapPosOffset);
					ImGui::Text("Frame Padding: (%.2f, %.2f)", G.Style.FramePadding.x, G.Style.FramePadding.y);
					ImGui::Text("Frame Rounding: %.2f", G.Style.FrameRounding);
					ImGui::Text("In Grid: %s", (UI::UIContext.bInGrid ? "Yes" : "No"));
					ImGui::Text("In Table: %s", (ImGui::GetCurrentTable() != nullptr) ? "Yes" : "No");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				/* Restore the cursor. */
				ImGui::SetCursorPos(CursorPos);
			}

			static ETheme SelectedTheme = ETheme::Dark;
			ImGui::SetNextItemWidth(160.0f);
			UI::SetNextComboFlags(ImGuiComboFlags_NoArrowButton);
			if (UI::PropertyDropdown("Default Theme", Themes, SelectedTheme))
			{
				LK_CORE_CONSOLE_INFO("Selected theme: {}", Enum::ToString(SelectedTheme));
				SetTheme(SelectedTheme);
			}
			/* TODO: Should be able to align the button position entirely based on the dropdown/combo position. */
			//const ImRect DropdownRect = UI::GetItemRect();

			ImGui::SameLine(0, 8.0f);
			static constexpr float DistY = -4.0f;
			UI::ShiftCursorY(DistY);

			if (ImGui::Button("Load Custom Theme"))
			{
				const std::filesystem::path ThemePath = LFileSystem::OpenFileDialog(
					{{ "LkEngine Theme File", THEME_FILE_EXTENSION.data() }}
				);
				LK_CORE_WARN_TAG("ThemeManager", "Selected theme: {} (Filename: {})", ThemePath, ThemePath.filename());

				if (!ThemePath.empty())
				{
					const std::string SelectedTheme = ThemePath.filename().string();
					if (SelectedTheme != CurrentThemeName)
					{
						LoadSavedTheme(SelectedTheme);
					}

					ShouldClosePopup = true;
				}
			}

			if (ShouldClosePopup)
			{
				ImGui::CloseCurrentPopup();
			}

		}, (uint32_t)EMessageBoxFlag::OkButton, PopupWidth, 0, PopupWidth, 100);
	}

	void LThemeManagerPanel::UI_ColorPicker()
	{
		auto& Colors = ImGui::GetStyle().Colors;
		const ImVec2 AvailContent = ImGui::GetContentRegionAvail();
		static constexpr ImGuiTableFlags ColorPickerTableFlags = ImGuiTableFlags_SizingFixedFit
			| ImGuiTableFlags_NoSavedSettings
			| ImGuiTableFlags_BordersInnerV;
		if (ImGui::BeginTable("##ColorPicker", 2, ColorPickerTableFlags))
		{
			ImGui::TableSetupColumn("##Column0", ImGuiTableFlags_None, AvailContent.x * 0.45f);
			ImGui::TableSetupColumn("##Column1", ImGuiTableFlags_None, AvailContent.x * 0.55f);

			ImGui::TableNextRow();

			/* Left Side. */
			ImGui::TableSetColumnIndex(0);
			{
				static ImGuiCol_ SelectedImGuiColor = (ImGuiCol_)0;
				ImVec4& ColorSelection = Colors[SelectedImGuiColor];

				ImGui::SetNextItemWidth(220.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
				if (UI::PropertyDropdown("ImGui Color", ImGuiColors, SelectedImGuiColor))
				{
					LK_CORE_INFO_TAG("ThemeManager", "Selected ImGuiColor: {}", Enum::ToString(SelectedImGuiColor));
				}
				ImGui::PopStyleVar(1);

				static float* RefColor = nullptr;
				static int32_t RefColorIdx = 0;

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::ColorPicker4("##ThemeManager-ColorPicker", &ColorSelection.x, EditFlags, RefColor))
				{
					/* Theme has changed. */
				}

				/* FIXME: The internal appliance of columns for PropertyDropdown. */
				if (UI::PropertyDropdown("Color Reference", ColorReferences, &RefColorIdx))
				{
					LK_CORE_CONSOLE_INFO("Set color reference: {}", ColorReferences.at(RefColorIdx).first);
				}
			}

			/* Right Side. */
			ImGui::TableSetColumnIndex(1);
			{
				UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 4.0f));

				UI::Font::Push("Large");
				ImGui::Text("Theme Preview");
				UI::Font::Pop();
				
				/// TODO: Show test widgets here.
				{ 
					if (ImGui::Button("Button")) {}
					ImGui::SameLine(0, 5.0f);
					static bool bRadioButton = false;
					if (ImGui::RadioButton("Radio Button", bRadioButton))
					{
						bRadioButton = !bRadioButton;
					}

					/* SliderFloat */
					{
						static float Value = 0.0f;
						ImGui::SliderFloat("Slider", &Value, 0.0f, 100.0f);
					}

					/* SliderFloat2 */
					{
						static glm::vec2 Value(0.0f, 0.0f);
						ImGui::SliderFloat2("Slider2", &Value.x, 0.0f, 100.0f);
					}

					/* SliderFloat3 */
					{
						static glm::vec3 Value(0.0f, 0.0f, 0.0f);
						ImGui::SliderFloat3("Slider3", &Value.x, 0.0f, 100.0f);
					}

					/* SliderFloat4 */
					{
						static glm::vec4 Value(0.0f, 0.0f, 0.0f, 0.0f);
						ImGui::SliderFloat4("Slider4", &Value.x, 0.0f, 100.0f);
					}

					/* Combo */
					{
						static const char* Options[] = { "Option 1", "Option 2", };
						static int SelectedIdx = 0;

						const char* SelectedOption = Options[SelectedIdx];
						if (ImGui::BeginCombo("##Combo", SelectedOption))
						{
							for (int Idx = 0; Idx < LK_ARRAYSIZE(Options); Idx++)
							{
								const bool IsSelected = (SelectedIdx == Idx);
								if (ImGui::Selectable(Options[Idx], IsSelected, ImGuiSelectableFlags_SpanAvailWidth))
								{
									SelectedIdx = Idx;
								}
							}

							ImGui::EndCombo();
						}
					}
				}
			}

			ImGui::EndTable();
		}

		if (ImGui::TreeNode("Edit Flags"))
		{
			static constexpr int FlagBits = sizeof(decltype(EditFlags)) * 8;
			std::bitset<FlagBits> EditFlagsBits(EditFlags);
			ImGui::Text("Edit Flags: %s", EditFlagsBits.to_string().c_str());

			UI::BeginPropertyGrid(nullptr, ImVec2(0, 0), GridFlags, bGridTable_UseHeaderLabels);
			{
				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_Float;
				if (UI::Property("Float", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_Float;
					else EditFlags &= ~ImGuiColorEditFlags_Float;
				}
			}

			{
				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_AlphaBar;
				if (UI::Property("Alpha Bar", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_AlphaBar;
					else EditFlags &= ~ImGuiColorEditFlags_AlphaBar;
				}
			}

			{
				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_AlphaPreview;
				if (UI::Property("Alpha Preview", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_AlphaPreview;
					else EditFlags &= ~ImGuiColorEditFlags_AlphaPreview;
				}
			}

			{
				if (EditFlags & ImGuiColorEditFlags_PickerHueWheel)
				{
					ImGui::BeginDisabled();
				}

				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_PickerHueBar;
				if (UI::Property("Hue Bar", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_PickerHueBar;
					else EditFlags &= ~ImGuiColorEditFlags_PickerHueBar;
				}

				if (EditFlags & ImGuiColorEditFlags_PickerHueWheel)
				{
					ImGui::EndDisabled();
				}
			}

			{
				if (EditFlags & ImGuiColorEditFlags_PickerHueBar)
				{
					ImGui::BeginDisabled();
				}

				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_PickerHueWheel;
				if (UI::Property("Hue Wheel", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_PickerHueWheel;
					else EditFlags &= ~ImGuiColorEditFlags_PickerHueWheel;
				}

				if (EditFlags & ImGuiColorEditFlags_PickerHueBar)
				{
					ImGui::EndDisabled();
				}
			}

			{
				static bool bPropertyValue = EditFlags & ImGuiColorEditFlags_NoSidePreview;
				if (UI::Property("No Side Preview", bPropertyValue))
				{
					if (bPropertyValue) EditFlags |= ImGuiColorEditFlags_NoSidePreview;
					else EditFlags &= ~ImGuiColorEditFlags_NoSidePreview;
				}
			}
			UI::EndPropertyGrid();

			ImGui::TreePop();
		}


		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Separator();

		if (ImGui::TreeNode("Table Settings"))
		{
			UI::BeginPropertyGrid("GridFlags", ImVec2(0, 0));
			{
				UI::Font::Push("Large");
				ImGui::Text("Settings");
				UI::Font::Pop();

				{
					static bool bPropertyValue = false;
					if (UI::Property("Resizeable", bPropertyValue))
					{
						if (bPropertyValue) GridFlags |= ImGuiTableFlags_Resizable;
						else GridFlags &= ~ImGuiTableFlags_Resizable;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Row Background", bPropertyValue))
					{
						if (bPropertyValue) GridFlags |= ImGuiTableFlags_RowBg;
						else GridFlags &= ~ImGuiTableFlags_RowBg;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Precise Widths", bPropertyValue))
					{
						if (bPropertyValue) GridFlags |= ImGuiTableFlags_PreciseWidths;
						else GridFlags &= ~ImGuiTableFlags_PreciseWidths;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Borders", bPropertyValue))
					{
						if (bPropertyValue) GridFlags |= ImGuiTableFlags_Borders;
						else GridFlags &= ~ImGuiTableFlags_Borders;
					}
				}

				UI::Property("Header Labels", bGridTable_UseHeaderLabels);

			}
			UI::EndPropertyGrid();

			ImGui::TreePop();
		}

	}

}
