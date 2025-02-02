#pragma once

#include "LkEngine/Renderer/UI/Panel.h"
#include "LkEngine/Core/SelectionContext.h"


namespace LkEngine {

	class LEditorSettingsPanel : public IPanel
	{
	public:
		LEditorSettingsPanel();
		~LEditorSettingsPanel() = default;

		virtual void Render() override {}
		virtual void RenderUI(bool& IsOpen) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		void UI_CoreSettings();

	public:
		static bool bWindow_CoreSettings;

	private:
		LCLASS(LEditorSettingsPanel);
	};

}
