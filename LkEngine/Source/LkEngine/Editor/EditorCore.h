#pragma once

#include "LkEngine/Core/CoreMacros.h"


namespace LkEngine {

	/** 
     * EGizmo 
     */
	enum class EGizmo : uint16_t
	{ 
		Translate	= LK_BIT_FIELD(0),
		Rotate		= LK_BIT_FIELD(1),
		Scale		= LK_BIT_FIELD(2),
	};


}