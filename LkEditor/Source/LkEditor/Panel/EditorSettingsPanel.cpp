#include "EditorSettingsPanel.h"

#include <LkEngine/Editor/EditorSettings.h>

#include <LkEngine/Editor/PanelManager.h>
#include <LkEngine/Editor/ThemeManagerPanel.h>


namespace LkEngine {

	bool LEditorSettingsPanel::bWindow_CoreSettings = false;

	LEditorSettingsPanel::LEditorSettingsPanel()
	{
		LOBJECT_REGISTER();
	}

	void LEditorSettingsPanel::RenderUI(bool& IsOpen)
	{
		if (bWindow_CoreSettings)
		{
			UI_CoreSettings();
		}

		IsOpen |=
			bWindow_CoreSettings
		;
	}

	void LEditorSettingsPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_CORE_TRACE_TAG("EditorSettingsPanel", "Serializing to yaml");
		Out << YAML::Key << "Windows" << YAML::Value << YAML::BeginMap;
		{
			LK_SERIALIZE_PROPERTY(CoreSettings, bWindow_CoreSettings, Out);
		}
		Out << YAML::EndMap;
	}

	void LEditorSettingsPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		const YAML::Node& WindowsNode = Data["Windows"];
		if (!WindowsNode)
		{
			LK_CORE_ERROR_TAG("EditorSettingsPanel", "Node 'Windows' is not valid");
			return;
		}
		
		LK_DESERIALIZE_PROPERTY(CoreSettings, bWindow_CoreSettings, WindowsNode, false);
	}

	void LEditorSettingsPanel::UI_CoreSettings()
	{
		ImGuiContext& G = *GImGui;
		FEditorSettings& EditorSettings = FEditorSettings::Get();
		bool SaveSettings = false;

		static const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		static constexpr const char* WindowName = "Editor Settings";

		if (!UI::IsWindowDocked<UI::EFindType::Name>(WindowName))
		{
			/* Only constraint the window size when the window is undocked. */
			ImGui::SetNextWindowSizeConstraints(ImVec2(460, 520), UI::SizeConstraint<ImVec2>::Max);
		}

		if (!UI::Begin(WindowName, &bWindow_CoreSettings, WindowFlags))
		{
			return;
		}

		if (UI::PropertyGridHeader("Appearance", true)) 
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

			UI::Font::Push("Larger");
			if (ImGui::Button("Theme Manager"))
			{
				const bool ThemeManagerOpen = IsPanelOpen.Execute(PanelID::ThemeManager);
				LK_CORE_INFO("{} the theme manager", ThemeManagerOpen ? "Closing" : "Opening");
				OnSetPanelOpen.Broadcast(PanelID::ThemeManager, !ThemeManagerOpen);
			}
			UI::Font::Pop();

			if (ImGui::Button("Set default theme"))
			{
				LThemeManagerPanel::LoadDefaultTheme();
			}

			if (UI::PropertyGridHeader("Theme Colors", false))
			{
				static constexpr float ColumnWidth = 100.0f;
				auto& Colors = ImGui::GetStyle().Colors;
				UI::FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(G.Style.CellPadding.x, 0.0f));

				UI::BeginPropertyGrid();
				for (const ImGuiCol_ ImGuiColor : TEnumRange<ImGuiCol_>())
				{
					UI::Draw::Vec4Control<UI::EVectorSemantic::RGBA>(Enum::ToString(ImGuiColor), Colors[ImGuiColor], 0.010f, 1.0f, 0.0f, 1.0f);
				}
				UI::EndPropertyGrid();

				UI::EndPropertyGridHeader();
			}

			ImGui::PopStyleVar(2); /* FramePadding, FrameRounding */

			UI::EndPropertyGridHeader();
		}

		if (UI::PropertyGridHeader("Gizmo Settings"))
		{
			UI::BeginPropertyGrid();
			{
				SaveSettings |= UI::Property("Translation", EditorSettings.TranslationSnapValue, 0.010f, 0.0f, 1000.0f, "The translation snap value.");
				SaveSettings |= UI::Property("Rotation", EditorSettings.RotationSnapValue, 0.10f, 0.050f, 360.0f, "The rotation snap value in degrees");
				SaveSettings |= UI::Property("Scale", EditorSettings.ScaleSnapValue, 0.10f, 0.0010f, 1000.0f, "The scale snap value.");
			}
			UI::EndPropertyGrid();

			UI::EndPropertyGridHeader();
		}

		if (ImGui::Button("Load Default Settings"))
		{
			LK_CORE_INFO_TAG("EditorSettingsPanel", "Loading default settings");
		}

		///ImGui::End();
		UI::End();
	}

}
