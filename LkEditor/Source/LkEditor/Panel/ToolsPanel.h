/******************************************************************
 * LToolsPanel
 * 
 * UI Panel
 *******************************************************************/
#pragma once

#include "LkEngine/Renderer/UI/Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorCore.h"

#include "LkEngine/Serialization/YamlSerialization.h"

namespace LkEngine {

	class LToolsPanel : public IPanel
	{
	public:
		LToolsPanel();
		~LToolsPanel() = default;

		virtual void Render() override {}
		virtual void RenderUI(bool& IsOpen) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		void UI_ObjectReferences();
		void UI_AssetRegistry();
		void UI_InputInfo();
		void UI_UserInterfaceTools();
		void UI_Fonts();

		void UI_InputInfo_SceneSelection(const LVector2& Size = LVector2(0, 0));
		void UI_InputInfo_ContentBrowserSelection(const LVector2& Size = LVector2(0, 0));

		void UI_Sandbox();
		void UI_Console();

	public:
		struct FWindow_ObjectReferences : public FWindow
		{
		};

		struct FWindow_InputInfo : public FWindow
		{
			bool bTreeNode_Selection = true;
			bool bTreeNode_KeyInfo = false;

			FWindow_InputInfo() 
				: bTreeNode_Selection(true)
				, bTreeNode_KeyInfo(true)
			{
			}
		};

		struct FWindow_UserInterfaceTools : public FWindow
		{
			bool bTreeNode_MessageBoxes = true;
			FWindow_UserInterfaceTools() 
				: bTreeNode_MessageBoxes(true) 
			{
			}
		};

		struct FWindow_AssetRegistry : public FWindow
		{
			FWindow_AssetRegistry() = default;
		};

		struct FWindow_Fonts : public FWindow
		{
		};

		struct FWindow_Sandbox : public FWindow
		{
		};

		struct FWindow_Console : public FWindow
		{
		};

		inline static FWindow_ObjectReferences Window_ObjectReferences;
		inline static FWindow_AssetRegistry Window_AssetRegistry;
		inline static FWindow_InputInfo Window_InputInfo;
		inline static FWindow_UserInterfaceTools Window_UserInterfaceTools;
		inline static FWindow_Fonts Window_Fonts;
		inline static FWindow_Sandbox Window_Sandbox;
		inline static FWindow_Console Window_Console;

	private:
		LPANEL(LToolsPanel);
	};

}
