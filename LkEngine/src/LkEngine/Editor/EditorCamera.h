#pragma once

//#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float FOV_deg, float width, float height, float nearPlane, float farPlane);
		~EditorCamera() = default;

		void Update(float ts);
		void UpdateView();
		void UpdateProjection();

	private:
		glm::vec3 m_Pos, m_Direction, m_FocalPoint;

		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};
		glm::vec2 m_MouseDelta = { 0.0f, 0.0f };
		glm::vec2 m_MousePos = { 0.0f, 0.0f };

		uint32_t m_ViewportWidth{ 1280 }, m_ViewportHeight{ 720 };
	};

}
