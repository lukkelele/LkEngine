#include "LKpch.h"
#include "EditorSettingsPanel.h"

#include "LkEngine/Editor/EditorSettings.h"


namespace LkEngine {

	bool LEditorSettingsPanel::bWindow_CoreSettings = false;

	LEditorSettingsPanel::LEditorSettingsPanel()
	{
		LOBJECT_REGISTER();
	}

	void LEditorSettingsPanel::OnRenderUI(bool& IsOpen)
	{
		if (bWindow_CoreSettings)
		{
			UI_CoreSettings();
		}

		IsOpen = (bWindow_CoreSettings
		);
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
		FEditorSettings& EditorSettings = FEditorSettings::Get();
		bool SaveSettings = false;

		static const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		static constexpr const char* WindowName = "LkEngine Editor Settings";
		if (!UI::IsWindowDocked<UI::EFindType::Name>(WindowName))
		{
			/* Only constraint the window size when the window undocked. */
			ImGui::SetNextWindowSizeConstraints(ImVec2(460, 320), ImVec2(6000, 6000));
		}
		ImGui::Begin(WindowName, &bWindow_CoreSettings, WindowFlags);

		if (UI::PropertyGridHeader("Gizmo Settings"))
		{
			UI::BeginPropertyGrid();
			{
				SaveSettings |= UI::Property("Translation", EditorSettings.TranslationSnapValue, 0.010f, 0.0f, 1000.0f, "The translation snap value.");
				SaveSettings |= UI::Property("Rotation", EditorSettings.RotationSnapValue, 0.10f, 0.050f, 360.0f, "The rotation snap value in degrees");
				SaveSettings |= UI::Property("Scale", EditorSettings.ScaleSnapValue, 0.10f, 0.0010f, 1000.0f, "The scale snap value.");
				UI::EndPropertyGrid();
			}

			UI::EndPropertyGridHeader();
		}

		if (ImGui::Button("Load Default Settings"))
		{
			LK_CORE_INFO_TAG("EditorSettingsPanel", "Loading default settings");
		}

		ImGui::End();
	}

}
