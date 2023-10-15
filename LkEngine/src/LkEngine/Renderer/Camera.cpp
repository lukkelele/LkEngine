#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	void Camera::UpdateMousePosition()
	{
		const glm::vec2& mousePos { Mouse::GetMouseX(), Mouse::GetMouseY() };
		m_MouseDelta = (mousePos - m_MousePos);
		m_MousePos = mousePos;

		if (Keyboard::IsKeyPressed(Key::Escape))
			m_MouseEnabled = false;
	}


}