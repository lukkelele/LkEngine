#pragma once
#include "LkEngine/Input/MouseCodes.h"
#include <utility>
#include <glm/glm.hpp>

namespace LkEngine {

	class Mouse
	{
	public:
		static bool IsButtonPressed(MouseCode button);
		static glm::vec2 GetMousePos();
		static std::pair<float, float> GetMousePosition();
		static float Mouse::GetMouseX();
		static float Mouse::GetMouseY();
		static glm::vec2 GetMousePosNormalized();
		static void SetPos(float x, float y) { Pos.x = x; Pos.y = y; }

	public:
		//static std::pair<float, float> Pos;
		//static std::pair<float, float> PosScaled;
		//static std::pair<float, float> Scalers;
		static glm::vec2 Pos;
		static glm::vec2 ScaledPos;
		static glm::vec2 Scalers;
	};

}