#pragma once
#include "LkEngine/Input/MouseCodes.h"
#include <utility>
#include <glm/glm.hpp>

namespace LkEngine {

	class Mouse
	{
	public:
		static bool IsButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float Mouse::GetMouseX();
		static float Mouse::GetMouseY();
	};

}