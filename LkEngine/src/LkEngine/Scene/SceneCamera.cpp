#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

	SceneCamera::SceneCamera()
	{
	}

	void SceneCamera::OnUpdate(float ts)
	{
	}

	void SceneCamera::UpdateProjection()
	{
		//m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
	}

	void SceneCamera::UpdateView()
	{
		//m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	}


}