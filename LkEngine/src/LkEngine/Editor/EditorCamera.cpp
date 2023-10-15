#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Application.h"


namespace LkEngine {

	EditorCamera::EditorCamera()
	{
		LOG_DEBUG("[ CREATED ] EditorCamera");
	}

	EditorCamera::EditorCamera(float FOV, float nearPlane, float farPlane)
	{
#ifdef LK_ENGINE_OLD_IMPL
		m_FOV = FOV;
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;

		const glm::quat orientation = GetOrientation();

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), m_ViewportWidth, m_ViewportHeight, m_NearPlane, m_FarPlane);
		m_View = glm::translate(glm::mat4(1.0f), m_Pos) * glm::toMat4(orientation);
		m_ViewProjection = m_Projection * m_View;
#endif

		auto mouse_pos = Mouse::GetMousePosition();
		m_InitialMousePos = { mouse_pos.first, mouse_pos.second };
	}

	void EditorCamera::OnUpdate(float ts)
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
				printf("Key -> S: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
			if (Keyboard::IsKeyPressed(Key::D))
			{
				//LOG_DEBUG("Key: D");
				m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
				printf("Key -> D: m_Pos == (%f, %f, %f)\n", m_Pos.x, m_Pos.y, m_Pos.z);
			}
		}

		UpdateMousePosition();
		// If the mouse has moved since last frame, update camera rotation
		UpdateProjection();
		UpdateView();
		m_InverseView = glm::inverse(m_View);
		m_InverseProjection = glm::inverse(m_Projection);
		m_InverseViewProjection = m_InverseProjection * m_InverseView;
		m_ViewProjection = m_Projection * m_View;
	}

	void EditorCamera::UpdateView()
	{

	}

	void EditorCamera::UpdateProjection()
	{

	}

}
