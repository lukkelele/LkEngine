#pragma once

#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/UI/Panel.h"


namespace LkEngine {

	class LEditorSettingsPanel : public IPanel
	{
	public:
		LEditorSettingsPanel();
		~LEditorSettingsPanel() = default;

		virtual void OnRender() override {}
		virtual void OnRenderUI(bool& IsOpen) override;

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
