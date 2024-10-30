#pragma once

#include "Panel.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	/**
	 * LContentBrowser
	 */
	class LContentBrowser : public IPanel
	{
	public:
		LContentBrowser();
		~LContentBrowser() = default;

		virtual void Initialize() override;

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

	public:
		inline static TObjectPtr<LTexture> FileIconTexture;
		inline static TObjectPtr<LTexture> FolderIconTexture;

	private:
		LCLASS(LContentBrowser);
	};

}