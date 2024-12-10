/******************************************************************
 * LObjectPtr
 * 
 *******************************************************************/
#pragma once

#include "Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class LDebugPanel : public IPanel
	{
	public:
		LDebugPanel();
		~LDebugPanel() = default;

		virtual void OnRender() override;
		virtual void OnRenderUI(bool& IsOpen) override;

	public:
		inline static bool bWindow_ObjectReferences = false;

	private:
		/**
		 * @brief Object references window.
		 */
		void UI_ObjectReferences();

	private:
		LCLASS(LDebugPanel);
	};

}
