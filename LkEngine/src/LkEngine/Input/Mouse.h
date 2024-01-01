#pragma once

#include <utility>

#include <glm/glm.hpp>

#include "LkEngine/Input/MouseCodes.h"


namespace LkEngine {

	class Mouse
	{
	public:
		static void Init();
		static bool IsButtonPressed(MouseCode button);
		static glm::vec2 GetMousePos();
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		static glm::vec2 GetMousePosNormalized();
		static void SetPos(float x, float y) { Pos.x = x; Pos.y = y; }
		static void SetCenterPos(float x, float y); // { CenterPos.x = x; CenterPos.y = y; }
		//static void SetScalerX(float x);
		//static void SetScalerY(float y);
		//static void SetScalers(float x, float y);
		static glm::vec2 GetPos() { return Pos; }
		static glm::vec2 GetScaledPos() { return ScaledPos; }
		static glm::vec2 GetCenterPos(); // { return CenterPos; }
		static bool IsMouseButtonProcessed(const MouseCode& button);

		// Setters for window width and height, necessary for scaling cursor pos
		static void SetWindowWidth(float x);
		static void SetWindowHeight(float y);
		static void SetWindowResolution(float x, float y);

	public:
		inline static glm::vec2 Pos = { 0.0f, 0.0f };

		inline static glm::vec2 ScaledPos = { 1.0f, 1.0f };
		//inline static glm::vec2 Scalers = { 1.0f, 1.0f };

		// Mouse position with origin (0, 0) in the center of the screen
		inline static glm::vec2 CenterPos = { 0.0f, 0.0f }; 
		inline static glm::vec2 ScaledCenterPos = { 0.0f, 0.0f };

		inline static bool LeftMouseButtonProcessed = false;
		inline static bool RightMouseButtonProcessed = false;

	private:
		// Window width and height is set in WindowResizeCallback
		inline static float m_WindowWidth = 0.0f;
		inline static float m_WindowHeight = 0.0f;

		friend class Window;
	};

}