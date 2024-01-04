#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Core/Application.h"

//FIXME

namespace LkEngine {

	void EditorCamera::SetPerspective(float degVerticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_DegPerspectiveFOV = degVerticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	void EditorCamera::SetOrthographic(float width, float height, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		UpdateView();
		switch (m_ProjectionType)
		{
			case ProjectionType::Perspective:
				SetPerspectiveProjectionMatrix(glm::radians(m_DegPerspectiveFOV), (float)width, (float)height, m_PerspectiveNear, m_PerspectiveFar);
				break;

			case ProjectionType::Orthographic:
				SetOrthoProjectionMatrix(width, height, m_OrthographicNear, m_OrthographicFar);
				break;
		}
	}

	void EditorCamera::UpdateViewProjection()
	{
		UpdateView(); 
		UpdateProjection();
	}

	void EditorCamera::Update(float ts)
	{
		HasMouseMoved = false;
		if (m_KeyboardEnabled)
		{
			// WASD
			if (Keyboard::IsKeyPressed(Key::W))
			{
				m_Pos += glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
			}
			if (Keyboard::IsKeyPressed(Key::A))
			{
				m_Pos -= glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
			}
			if (Keyboard::IsKeyPressed(Key::S))
			{
				m_Pos -= glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
			}
			if (Keyboard::IsKeyPressed(Key::D))
			{
				m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
			}
			if (Keyboard::IsKeyPressed(Key::Q))
			{
				m_Zoom += 0.010f;
			}
			if (Keyboard::IsKeyPressed(Key::R))
			{
				m_Zoom -= 0.010f;
			}

		}
		if (m_MouseEnabled)
		{
		}

		auto* window = Window::Get();
		SetViewportSize(window->GetViewportWidth(), window->GetViewportHeight());

		m_InverseViewMatrix = glm::inverse(m_ViewMatrix);
		m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
		m_InverseViewProjectionMatrix = m_InverseProjectionMatrix * m_InverseViewMatrix;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void EditorCamera::UpdateProjection()
	{
		float left, right, bottom, top;
		float width, height;

		auto window = Application::Get()->GetWindow();
		width = window->GetWidth();
		height = window->GetWidth();

		left = -width / 2.0f;
		right = width / 2.0f;
		bottom = -height / 2.0f;
		top = height / 2.0f;

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);
	}

	// FIXME: ortographic here, should be able to switch 
	void EditorCamera::UpdateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { m_Pos.x + m_Offset.x, m_Pos.y + m_Offset.y, m_Pos.z + m_Offset.z }) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void EditorCamera::SetPos(const glm::vec3& pos)
	{
		m_Pos = pos;
	}

	void EditorCamera::SetOffset(const glm::vec2& offset)
	{
		SetOffset({ offset.x, offset.y, 0.0f });
	}

	void EditorCamera::SetOffset(const glm::vec3& offset)
	{
		m_Offset = offset;
	}

	glm::vec3 EditorCamera::GetPosWithOffset()
	{
		return { 
			m_Pos.x + m_Offset.x, 
			m_Pos.y + m_Offset.y, 
			m_Pos.z + m_Offset.z
		};
	}

}
