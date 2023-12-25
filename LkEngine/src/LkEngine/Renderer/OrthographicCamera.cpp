#include "LKpch.h"
#if 0
#include "LkEngine/Renderer/OrthographicCamera.h"


namespace LkEngine {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewMatrix = glm::mat4(1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::Update(float ts)
	{
		//HandleInput(ts);
		//UpdateViewProjection();
		UpdateView(); // UpdateView sets m_ViewProjection in OrthographicCameras overridden func
	}

	void OrthographicCamera::UpdateProjection()
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UpdateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Pos) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
#endif