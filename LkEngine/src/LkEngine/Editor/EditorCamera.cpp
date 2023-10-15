#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

	EditorCamera::EditorCamera(float FOV, float nearPlane, float farPlane)
	{
		m_FOV = FOV;
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;

		const glm::quat orientation = GetOrientation();

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), m_ViewportWidth, m_ViewportHeight, m_NearPlane, m_FarPlane);
		m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewProjection = m_Projection * m_View;

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
				m_Position += m_ForwardDirection * ts * m_TravelSpeed;
			if (Keyboard::IsKeyPressed(Key::S))
				m_Position -= m_ForwardDirection * ts * m_TravelSpeed;
			if (Keyboard::IsKeyPressed(Key::A))
				m_Position -= m_RightDirection * ts * m_TravelSpeed;
			if (Keyboard::IsKeyPressed(Key::D))
				m_Position += m_RightDirection * ts * m_TravelSpeed;
		}

		const float distance = glm::distance(m_Origin, m_Position);
		m_Origin = m_Position + GetForwardDirection() * distance;
		m_Distance = distance;
		

		// Release mouse focus
		if (Keyboard::IsKeyPressed(Key::Escape))
		{
			if (m_MouseEnabled)
			{
				//glfwSetInputMode(*Application::Get()->GetGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				m_MouseEnabled = false;
			}
		}
		// Reinstate mouse focus
		else if (Keyboard::IsKeyPressed(Key::G))
		{
			if (!m_MouseEnabled)
			{
				//glfwSetInputMode(*Application::Get()->GetGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				m_MouseEnabled = true;
			}
		}

		m_RightDirection = glm::cross(m_ForwardDirection, m_UpDirection);
		UpdateMousePosition();
		// If the mouse has moved since last frame, update camera rotation
		if (m_MouseEnabled)
		{
			if (m_MouseDelta.x != 0.0f || m_MouseDelta.y != 0.0f)
			{
				float yaw = -m_MouseDelta.x * m_RotationSpeed;
				float pitch = -m_MouseDelta.y * m_RotationSpeed;
				m_Yaw = yaw; m_Pitch = pitch;

				glm::quat quat = glm::normalize(glm::cross(glm::angleAxis(pitch, m_RightDirection),
					glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f))));

				m_ForwardDirection = glm::rotate(quat, m_ForwardDirection);
				HasMouseMoved = true;
			}

		}

		UpdateProjection();
		UpdateView();
		m_InverseView = glm::inverse(m_View);
		m_InverseProjection = glm::inverse(m_Projection);
		m_InverseViewProjection = m_InverseProjection * m_InverseView;
		m_ViewProjection = m_Projection * m_View;
	}

}
