#pragma once

#include "LkEngine/Core/Math/Vector.h"


namespace LkEngine {

	struct FAABB
	{
		LVector Min{};
		LVector Max{};

		FAABB()
			: Min(0.0f)
			, Max(0.0f)
		{
		}

		FAABB(const LVector& InMin, const LVector& InMax)
			: Min(InMin)
			, Max(InMax)
		{
		}
	};

}
