#include "LKpch.h"
#include "LkEngine/Renderer/OrthographicCamera.h"



namespace LkEngine {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_View = glm::mat4(1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::OnUpdate(float ts)
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

	void OrthographicCamera::UpdateProjection()
	{
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::UpdateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Pos) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}
}