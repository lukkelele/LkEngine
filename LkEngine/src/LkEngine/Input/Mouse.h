#pragma once

#include <utility>

#include <glm/glm.hpp>

#include "LkEngine/Input/MouseCodes.h"


namespace LkEngine {

	// Forward declaration
	class Window;

	class Mouse
	{
	public:
		static void Init();
		static void SetActiveWindow(Window* window);

		static glm::vec2 GetPos();
		static glm::vec2 GetRawPos();
		static bool IsButtonPressed(MouseCode button);
		static float GetMouseX();
		static float GetMouseY();
		static void SetPos(float x, float y); 
		static void SetScaledPos(float x, float y);
		static glm::vec2 GetMousePosNormalized();
		static glm::vec2 GetCenterPos(); // { return CenterPos; }
		static glm::vec2 GetScaledPos();
		static glm::vec2 GetScaledCenterPos();

	public:
		inline static glm::vec2 Pos = { 0.0f, 0.0f };
		inline static glm::vec2 ScaledPos = { 1.0f, 1.0f };

		// Mouse position with origin (0, 0) in the center of the screen
		inline static glm::vec2 CenterPos = { 0.0f, 0.0f }; 
		inline static glm::vec2 ScaledCenterPos = { 0.0f, 0.0f };
	private:
		inline static Window* m_ActiveWindow = nullptr;
		friend class Window;
	};

}