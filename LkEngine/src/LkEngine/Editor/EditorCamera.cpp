#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Core/Application.h"

//FIXME

namespace LkEngine {

	EditorCamera::EditorCamera(float FOV_deg, float width, float height, float nearPlane, float farPlane)
	{
		auto mouse_pos = Mouse::GetMousePosition();
		m_InitialMousePos = { mouse_pos.first, mouse_pos.second };
	}

	void EditorCamera::Update(float ts)
	{
		HasMouseMoved = false;
		//HandleInput(ts);

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
				printf("Key -> S: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
			if (Keyboard::IsKeyPressed(Key::D))
			{
				//LOG_DEBUG("Key: D");
				m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
				printf("Key -> D: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
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

	void EditorCamera::UpdateView()
	{

	}

	void EditorCamera::UpdateProjection()
	{

	}

}
