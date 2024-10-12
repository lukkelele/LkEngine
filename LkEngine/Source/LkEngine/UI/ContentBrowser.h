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
		LContentBrowser() = default;
		~LContentBrowser() = default;

		void Init() override;
		void OnRender() override;
		void OnImGuiRender() override;

	public:
		inline static TObjectPtr<LTexture> FileIconTexture;
		inline static TObjectPtr<LTexture> FolderIconTexture;
	};

}