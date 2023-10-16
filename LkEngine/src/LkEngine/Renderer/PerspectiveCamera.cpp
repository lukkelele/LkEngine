#include "LKpch.h"
#include "LkEngine/Renderer/PerspectiveCamera.h"


namespace LkEngine {

	PerspectiveCamera::PerspectiveCamera(float FOV, float nearPlane, float farPlane)
		: m_FOV(FOV)
		, m_NearPlane(nearPlane)
		, m_FarPlane(farPlane)
	{
		LOG_DEBUG("New PerspectiveCamera, FOV: {0}", m_FOV);
		const glm::quat orientation = GetOrientation();

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), m_ViewportWidth, m_ViewportHeight, m_NearPlane, m_FarPlane);
		m_View = glm::translate(glm::mat4(1.0f), m_Pos) * glm::toMat4(orientation);
		m_ViewProjection = m_Projection * m_View;
	}

	void PerspectiveCamera::OnUpdate(float ts)
	{
	}

	glm::quat PerspectiveCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
	}

	void PerspectiveCamera::UpdateProjection()
	{
		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
	}

	void PerspectiveCamera::UpdateView()
	{
		m_View = glm::lookAt(m_Pos, m_Pos + m_ForwardDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void PerspectiveCamera::UpdateMousePosition()
	{
		const glm::vec2& mousePos{ Mouse::GetMouseX(), Mouse::GetMouseY() };
		m_MouseDelta = (mousePos - m_MousePos);
		m_MousePos = mousePos;

		if (Keyboard::IsKeyPressed(Key::Escape))
			m_MouseEnabled = false;
	}

	glm::vec3 PerspectiveCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 PerspectiveCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 PerspectiveCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 PerspectiveCamera::GetDirection()
	{
		glm::vec3 lookAt = m_Pos + GetForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Pos);
		return m_Direction;
	}

	glm::vec3 PerspectiveCamera::CalculatePosition() const
	{
		return m_Origin - GetForwardDirection() * m_Distance;
	}

	// remove
	void PerspectiveCamera::SetProjection(float fov, float _near, float _far)
	{
		m_FOV = fov;
		m_NearPlane = _near;
		m_FarPlane = _far;
		UpdateProjection();
	}


}
