/******************************************************************
 * LToolsPanel
 * 
 * UI Panel.
 *******************************************************************/
#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Serialization/YamlSerialization.h"


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
		void UI_InputInfo();
		void UI_UserInterfaceTools();
		void UI_RegisteredFonts();

	public:
		static bool bWindow_ObjectReferences;
		static bool bWindow_AssetRegistry;
		static bool bWindow_InputInfo;
		static bool bWindow_UserInterfaceTools;
		static bool bWindow_RegisteredFonts;

		/**
		 * @brief Message boxes node in the 'UserInterfaceTools' window.
		 */
		static bool bUserInterfaceTools_TreeNode_MessageBoxes;
	private:
		LCLASS(LToolsPanel);
	};

}
