#pragma once

#include "LkEngine/UI/Panel.h"
#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	namespace UI
	{
		enum EDragAndDropMode
		{
			Copy,
			Move,
			Swap
		};
	}

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
		virtual void OnRenderUI(bool& IsOpen) override;

	public:
		inline static TObjectPtr<LTexture> FileIconTexture;
		inline static TObjectPtr<LTexture> FolderIconTexture;

	private:
		LCLASS(LContentBrowser);
	};

}
