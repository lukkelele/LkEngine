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
		static float GetMouseX();
		static float GetMouseY();
		static glm::vec2 GetMousePosNormalized();
		static void SetPos(float x, float y) { Pos.x = x; Pos.y = y; }
		glm::vec2 GetPos() const { return Pos; }
		glm::vec2 GetScaledPos() const { return ScaledPos; }
		glm::vec2 GetCenterPos() const { return CenterPos; }

	public:
		static glm::vec2 Pos;
		static glm::vec2 ScaledPos;
		static glm::vec2 Scalers;
		static glm::vec2 CenterPos;
		static glm::vec2 ScaledCenterPos;
	};

}