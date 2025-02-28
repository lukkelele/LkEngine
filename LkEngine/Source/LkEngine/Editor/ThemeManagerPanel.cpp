#include "LKpch.h"
#include "ThemeManagerPanel.h"

#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

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
		LPANEL_REGISTER();

		std::memset(ThemeName.data(), 0, ThemeName.max_size());
	}

	void LThemeManagerPanel::RenderUI(bool& IsOpen)
	{
		ImGuiContext& G = *GImGui;

		ImGui::SetNextWindowPos(ImVec2(180, 120), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(540, 780), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(440, 200), UI::SizeConstraint<ImVec2>::Max);

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (!UI::Begin(PanelID::ThemeManager, &IsOpen, WindowFlags))
		{
			return;
		}

		static ImGuiColorEditFlags EditFlags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar;
		static ImGuiTableFlags GridFlags = ImGuiTableFlags_SizingFixedSame;
		static bool bGridTable_UseHeaderLabels = false;

		UI::Property("Theme Selector", bSelectorEnabled);

		ImGui::SameLine(0, 8.0f);
		if (ImGui::Button("Save Current Theme"))
		{
			LK_CORE_INFO("Saving current theme");
			SaveCurrentTheme();
		}

		ImGui::SetNextItemWidth(280.0f);
		ImGui::InputTextWithHint("##ThemeManager-ThemeName", "Theme Name", ThemeName.data(), ThemeName.size());

		/**
		 * Themes array for the dropdown. 
		 * Subtract one for the removal of the 'Custom' entry.
		 */
		static std::array<const char*, (uint16_t)ETheme::COUNT> Themes;
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

		/* ImGuiColors. */
		static std::array<const char*, (int)ImGuiCol_COUNT> ImGuiColors;
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

		static ETheme SelectedTheme = ETheme::Dark;
		ImGui::SetNextItemWidth(280.0f);
		if (UI::PropertyDropdown("Theme", Themes, SelectedTheme))
		{
			LK_CORE_CONSOLE_INFO("Selected theme: {}", Enum::ToString(SelectedTheme));
			SetTheme(SelectedTheme);
		}

		auto& Colors = ImGui::GetStyle().Colors;

		/// TODO: Store initial reset values when starting theme modification.
		const ImVec2 ModifierTableSize = ImMin(ImVec2(640, 0.0f), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
		UI::BeginPropertyGrid("##ThemeModifier", ModifierTableSize, ImGuiTableFlags_SizingStretchProp);
		{
			UI::FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(G.Style.CellPadding.x, 0.0f));

			UI::Draw::Vec4Control("WindowBg",           Colors[ImGuiCol_WindowBg], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("ChildBg",            Colors[ImGuiCol_ChildBg], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("Text",               Colors[ImGuiCol_Text], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("TextDisabled",       Colors[ImGuiCol_TextDisabled], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("Border",             Colors[ImGuiCol_Border], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("BorderShadow",       Colors[ImGuiCol_BorderShadow], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("TitleBg",            Colors[ImGuiCol_TitleBg], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("TitleBgActive",      Colors[ImGuiCol_TitleBgActive], 0.010f, 1.0f, 0.0f, 1.0f);
			UI::Draw::Vec4Control("TitleBgCollapsed",   Colors[ImGuiCol_TitleBgCollapsed], 0.010f, 1.0f, 0.0f, 1.0f);
		}
		UI::EndPropertyGrid();

		const ImVec2 AvailContent = ImGui::GetContentRegionAvail();
		//ImGui::BeginChild("##ColorPicker4", ImVec2(ImGui::GetContentRegionAvail().x, 540), ImGuiChildFlags_AlwaysUseWindowPadding);
		//ImGui::BeginGroup();
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
					if (CurrentTheme != ETheme::Custom)
					{
						CurrentTheme = ETheme::Custom;
					}
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
					//ImGui::SameLine(0, 5.0f);

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
					//ImGui::SameLine(0, 5.0f);

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
						if (bPropertyValue) GridFlags|= ImGuiTableFlags_Resizable;
						else GridFlags &= ~ImGuiTableFlags_Resizable;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Row Background", bPropertyValue))
					{
						if (bPropertyValue) GridFlags|= ImGuiTableFlags_RowBg;
						else GridFlags &= ~ImGuiTableFlags_RowBg;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Precise Widths", bPropertyValue))
					{
						if (bPropertyValue) GridFlags|= ImGuiTableFlags_PreciseWidths;
						else GridFlags &= ~ImGuiTableFlags_PreciseWidths;
					}
				}

				{
					static bool bPropertyValue = false;
					if (UI::Property("Borders", bPropertyValue))
					{
						if (bPropertyValue) GridFlags|= ImGuiTableFlags_Borders;
						else GridFlags &= ~ImGuiTableFlags_Borders;
					}
				}

				UI::Property("Header Labels", bGridTable_UseHeaderLabels);

			}
			UI::EndPropertyGrid();

			ImGui::TreePop();
		}


		UI::End();
	}

	void LThemeManagerPanel::SetSelectorEnabled(const bool InEnabled)
	{
		bSelectorEnabled = InEnabled;
	}

	bool LThemeManagerPanel::SaveCurrentTheme()
	{
		LK_CORE_ASSERT(CurrentTheme != ETheme::COUNT);
		//if (CurrentTheme == ETheme::Custom)
		if (true)
		{
			LK_CORE_DEBUG("std::strlen(ThemeName.data()) == 0: {}", std::strlen(ThemeName.data()) == 0 ? "True" : "False");
			const std::size_t ThemeNameLen = std::strlen(ThemeName.data());
			if (ThemeNameLen <= 0)
			{
				std::vector<std::filesystem::path> SimilarFiles;
				if (LFileSystem::FindSimilarFiles("Custom", LFileSystem::GetWorkingDir(), SimilarFiles))
				{
					LK_CORE_DEBUG_TAG("ThemeManager", "Found {} files in directory: '{}'", SimilarFiles.size(), LFileSystem::GetWorkingDir());
					std::snprintf(ThemeName.data(), ThemeName.max_size(), "Custom-%lld", SimilarFiles.size());;
				}
				else
				{
					LK_CORE_ERROR_TAG("ThemeManager", "Failed to find files similar with name 'Custom'");
					std::snprintf(ThemeName.data(), ThemeName.max_size(), "Custom-1");
				}
			}

			LK_CORE_INFO_TAG("ThemeManager", "Saving current theme: {}", ThemeName.data());
		}
		else
		{
		}

		const std::string ThemeNameStr(ThemeName.data());
		YAML::Emitter Out;
		//Out << YAML::Key << "Theme" << YAML::Value << ThemeNameStr;
		LK_SERIALIZE_PROPERTY(Theme, ThemeNameStr, Out);

		auto& Colors = ImGui::GetStyle().Colors;
		for (const ImGuiCol_ ImGuiColor : TEnumRange<ImGuiCol_>())
		{
			LK_SERIALIZE_PROPERTY_VALUE(Enum::ToString(ImGuiColor), Colors[ImGuiColor], Out);
		}

		LK_CORE_WARN("Serialized theme: {}\n\n{}", ThemeNameStr, Out.c_str());

		return true;
	}

	void LThemeManagerPanel::OnMouseButtonPressed(const FMouseButtonData& ButtonData)
	{
		LK_CORE_DEBUG_TAG("ThemeManager", "OnMouseButtonPressed: {}", Enum::ToString(ButtonData.Button));

		/* TODO: Queue mouse event to be executed in the ImGui Begin/End */
		ImGuiContext& G = *GImGui;
		//LK_CORE_CONSOLE_INFO("Hovered ID: {}", G.HoveredId);
		//LK_CORE_CONSOLE_INFO("ActiveId: {}   ActiveIdWindow: {}", G.ActiveId, (G.ActiveIdWindow ? G.ActiveIdWindow->Name : "NULL"));
		//LK_CORE_CONSOLE_WARN("Current Window: {}", (G.CurrentWindow ? G.CurrentWindow->Name : "NULL"));

		//ImGui::DebugLocateItemOnHover(G.ActiveId);

		if (ImGuiWindow* Window = ImGui::FindWindowByID(G.ActiveId); Window != nullptr)
		{
			LK_CORE_CONSOLE_INFO("ID {} is a window", G.ActiveId);
		}
	}

	void LThemeManagerPanel::SetTheme(const ETheme Theme)
	{
		if (Theme == ETheme::Dark)
		{
			CurrentTheme = ETheme::Dark;

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
			CurrentTheme = ETheme::Light;

			ImGui::StyleColorsLight();
			auto& Colors = ImGui::GetStyle().Colors;
		}
		else if (Theme == ETheme::GruvboxMaterial)
		{
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

	void LThemeManagerPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		/* TODO: */
		/* Save last used theme. */
	}

	void LThemeManagerPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		/* TODO: */
		/* Load last theme. */
	}

}
