#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	std::string Camera::GetTypeStr() const
	{
		switch (m_Type)
		{
			case Type::Scene:  return "Scene";
			case Type::Editor: return "Editor";
			case Type::None:   return "None";
		}
		return "NULL";
	}

#if 0
	void Camera::UpdateViewProjection()
	{
		UpdateView(); 
		UpdateProjection();
	}

	void Camera::HandleInput(float ts)
	{
		HasMouseMoved = false;

		if (m_KeyboardEnabled)
		{
			// WASD
			if (Keyboard::IsKeyPressed(Key::W))
			{
				m_Pos += glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
				printf("Key -> A: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
			if (Keyboard::IsKeyPressed(Key::A))
			{
				m_Pos -= glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
				printf("Key -> A: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
			if (Keyboard::IsKeyPressed(Key::S))
			{
				m_Pos -= glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
				//printf("Key -> S: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
			if (Keyboard::IsKeyPressed(Key::D))
			{
				m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
				//printf("Key -> D: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
		}

		//UpdateMousePosition();
		// If the mouse has moved since last frame, update camera rotation
		UpdateProjection();
		UpdateView();
		m_InverseViewMatrix = glm::inverse(m_ViewMatrix);
		m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
		m_InverseViewProjectionMatrix = m_InverseProjectionMatrix * m_InverseViewMatrix;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
#endif


}