#pragma once

#include "Panel.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	class Texture;

	class ContentBrowser : public Panel
	{
	public:
		ContentBrowser();
		~ContentBrowser() = default;

		void OnRender() override;
		void OnImGuiRender() override;

	public:
		inline static Ref<Texture> FileIconTexture;
		inline static Ref<Texture> FolderIconTexture;

	};

}