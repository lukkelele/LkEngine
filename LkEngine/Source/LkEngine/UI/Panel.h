#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/UI/UICore.h"


namespace LkEngine {

	class IPanel : public LObject
	{
	public:
		virtual ~IPanel() = default;

		virtual void Initialize() {};
		
		virtual void OnRender() = 0;
		virtual void OnRenderUI() = 0;
	};

}