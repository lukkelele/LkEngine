#pragma once

#include "LkEngine/UI/UICore.h"


namespace LkEngine {

	class IPanel
	{
	public:
		virtual ~IPanel() = default;

		virtual void Init() = 0;
		
		virtual void OnRender() = 0;
		virtual void OnImGuiRender() = 0;
	};

}