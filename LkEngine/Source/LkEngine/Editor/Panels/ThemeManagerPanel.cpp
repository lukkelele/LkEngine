#include "LKpch.h"
#include "ThemeManagerPanel.h"

#include <bitset>


namespace LkEngine {

	LThemeManagerPanel::LThemeManagerPanel()
	{
		LPANEL_REGISTER();
	}

	void LThemeManagerPanel::RenderUI(bool& IsOpen)
	{
		ImGui::SetNextWindowPos(ImVec2(180, 120), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(540, 780), ImGuiCond_FirstUseEver);

		static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (!UI::Begin(PanelID::ThemeManager, &IsOpen, WindowFlags))
		{
			return;
		}

		static ImGuiTableFlags GridFlags = ImGuiTableFlags_SizingFixedSame;
		static bool bGridTable_UseHeaderLabels = false;

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

		UI::Property("Theme Selector", bSelectorEnabled);

		/* Populate themes array for the dropdown. */
		static std::array<const char*, (uint16_t)ETheme::COUNT> Themes;
		if (Themes.at(0) == nullptr)
		{
			int Idx = 0;
			for (const ETheme Theme : TEnumRange<ETheme>())
			{
				LK_CORE_DEBUG("Adding theme to array: {}", Enum::ToString(Theme));
				LK_CORE_ASSERT(Idx != Themes.size());
				Themes[Idx++] = Enum::ToString(Theme);
			}
		}

		static ETheme SelectedTheme = ETheme::Dark;
		if (UI::PropertyDropdown("Theme", Themes, SelectedTheme))
		{
			LK_CORE_CONSOLE_INFO("Selected theme: {}", Enum::ToString(SelectedTheme));
			if (SelectedTheme == ETheme::Dark)
			{
				SetTheme<ETheme::Dark>();
			}
			else if (SelectedTheme == ETheme::Light)
			{
				SetTheme<ETheme::Light>();
			}
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

		static float* RefColor = nullptr;
		static int32_t RefColorIdx = 0;

		if (UI::PropertyDropdown("Color Reference", ColorReferences, &RefColorIdx))
		{
			LK_CORE_CONSOLE_INFO("Set color reference: {}", ColorReferences.at(RefColorIdx).first);
		}

		{
			static ImGuiColorEditFlags EditFlags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar;
			static constexpr int FlagBits = sizeof(decltype(EditFlags)) * 8;
			std::bitset<FlagBits> EditFlagsBits(EditFlags);
			ImGui::Text("Edit Flags: %s", EditFlagsBits.to_string().c_str());

			UI::BeginPropertyGrid("ColorPicker4", ImVec2(0, 0), GridFlags, bGridTable_UseHeaderLabels);
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

			ImGui::BeginChild("##ColorPicker4", ImVec2(ImGui::GetContentRegionAvail().x, 540), ImGuiChildFlags_AlwaysUseWindowPadding);
			{
				static glm::vec4 Color4{};
				ImGui::SetNextItemWidth(360);
				ImGui::ColorPicker4("##ThemeManager-ColorPicker", &Color4.x, EditFlags, RefColor);
			}
			ImGui::EndChild();
		}

		ImGui::Separator();

		UI::End();
	}

	void LThemeManagerPanel::SetSelectorEnabled(const bool InEnabled)
	{
		bSelectorEnabled = InEnabled;
	}

	void LThemeManagerPanel::OnMouseButtonPressed(const FMouseButtonData& ButtonData)
	{
		LK_CORE_DEBUG_TAG("ThemeManager", "OnMouseButtonPressed: {}", Enum::ToString(ButtonData.Button));

		ImGuiContext& Ctx = *GImGui;
		LK_CORE_CONSOLE_DEBUG("Hovered ID: {}", Ctx.HoveredId);
		LK_CORE_CONSOLE_DEBUG("Active ID: {}", Ctx.ActiveId);

		if (ImGuiWindow* Window = ImGui::FindWindowByID(Ctx.ActiveId); Window != nullptr)
		{
			LK_CORE_CONSOLE_INFO("ID {} is a window", Ctx.ActiveId);
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
