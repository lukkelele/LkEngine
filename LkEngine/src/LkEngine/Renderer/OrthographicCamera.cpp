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
		HandleInput(ts);
		//UpdateViewProjection();
		UpdateView(); // UpdateView sets m_ViewProjection in OrthographicCameras overridden func
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