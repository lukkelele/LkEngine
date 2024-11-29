#pragma once

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	/** 
     * EGizmo 
     */
	enum class EGizmo : uint16_t
	{ 
		Translate	= LK_BIT(0),
		Rotate		= LK_BIT(1),
		Scale		= LK_BIT(2),
	};


}
