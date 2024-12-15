/******************************************************************
 * LToolsPanel
 * 
 * UI Panel.
 *******************************************************************/
#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class LToolsPanel : public IPanel
	{
	public:
		LToolsPanel();
		~LToolsPanel() = default;

		virtual void OnRender() override {}
		virtual void OnRenderUI(bool& IsOpen) override;

	private:
		void UI_ObjectReferences();
		void UI_RegisteredFonts();

	public:
		static bool bWindow_ObjectReferences;
		static bool bWindow_RegisteredFonts;
	private:
		LCLASS(LToolsPanel);
	};

}
