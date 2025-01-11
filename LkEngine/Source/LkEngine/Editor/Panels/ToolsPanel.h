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
		void UI_Fonts();

	public:
		struct FWindow_ObjectReferences : public FWindow
		{
		};

		struct FWindow_InputInfo : public FWindow
		{
			bool bTreeNode_Selection = true;
			bool bTreeNode_KeyInfo = false;
		};

		struct FWindow_UserInterfaceTools : public FWindow
		{
			bool bTreeNode_MessageBoxes = true;
		};

		struct FWindow_AssetRegistry : public FWindow
		{
		};

		struct FWindow_Fonts : public FWindow
		{
		};

		inline static FWindow_ObjectReferences Window_ObjectReferences{};
		inline static FWindow_AssetRegistry Window_AssetRegistry{};
		inline static FWindow_InputInfo Window_InputInfo{};
		inline static FWindow_UserInterfaceTools Window_UserInterfaceTools{};
		inline static FWindow_Fonts Window_Fonts{};

	private:
		LCLASS(LToolsPanel);
	};

}
