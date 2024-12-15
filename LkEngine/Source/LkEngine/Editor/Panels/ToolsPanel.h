/******************************************************************
 * LToolsPanel
 * 
 * UI Panel.
 *******************************************************************/
#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Serialization/YAMLSerialization.h"


namespace LkEngine {

	class LToolsPanel : public IPanel
	{
	public:
		LToolsPanel();
		~LToolsPanel() = default;

		virtual void OnRender() override {}
		virtual void OnRenderUI(bool& IsOpen) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		void UI_ObjectReferences();
		void UI_AssetRegistry();
		void UI_RegisteredFonts();

	public:
		static bool bWindow_ObjectReferences;
		static bool bWindow_RegisteredFonts;
		static bool bWindow_AssetRegistry;
	private:
		LCLASS(LToolsPanel);
	};

}
