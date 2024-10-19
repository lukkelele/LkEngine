#pragma once

#include "LkEngine/UI/UICore.h"


namespace LkEngine {

	class IPanel
	{
	public:
		virtual ~IPanel() = default;

		virtual void Initialize() = 0;
		
		virtual void OnRender() = 0;
		virtual void OnRenderUI() = 0;
	};

}