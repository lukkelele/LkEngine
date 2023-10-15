#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Application.h"


namespace LkEngine {

	SceneCamera::SceneCamera()
	{
		m_View = glm::mat4(1.0f);
		m_Projection = glm::mat4(1.0f);
	}

	void SceneCamera::OnUpdate(float ts)
	{
		UpdateView();
	}

	void SceneCamera::UpdateProjection()
	{
		//m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
	}

	// FIXME: ortographic here, should be able to switch 
	void SceneCamera::UpdateView()
	{
		//m_View = glm::lookAt(m_Pos, m_Pos + m_ForwardDirection, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Pos) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}


}