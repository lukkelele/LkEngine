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
		static void SetCenterPos(float x, float y) { CenterPos.x = x; CenterPos.y = y; }
		static glm::vec2 GetPos() { return Pos; }
		static glm::vec2 GetScaledPos() { return ScaledPos; }
		static glm::vec2 GetCenterPos() { return CenterPos; }
		static bool IsMouseButtonProcessed(const MouseCode& button);

	public:
		inline static bool LeftMouseButtonProcessed = false;
		inline static bool RightMouseButtonProcessed = false;
		inline static glm::vec2 Pos = { 0.0f, 0.0f };
		inline static glm::vec2 ScaledPos = { 1.0f, 1.0f };
		inline static glm::vec2 Scalers = { 1.0f, 1.0f };
		inline static glm::vec2 CenterPos = { 0.0f, 0.0f };
		inline static glm::vec2 ScaledCenterPos = { 0.0f, 0.0f };
	};

}