#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"
//#include "LkEngine/Scene/Scene.h"

#include <imgui/imgui.h>


namespace LkEngine {

	Camera::Camera(float FOV, float nearPlane, float farPlane)
		: m_FOV(FOV)
		, m_NearPlane(nearPlane)
		, m_FarPlane(farPlane)
	{
		//LOG_INFO("FPS Camera created | FOV: {0}", m_FOV);
		const glm::quat orientation = GetOrientation();

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), m_ViewportWidth, m_ViewportHeight, m_NearPlane, m_FarPlane);
		m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewProjection = m_Projection * m_View;
	}

	glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
	}

	void Camera::UpdateProjection()
	{
		m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
	}

	void Camera::UpdateView()
	{
		m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Camera::UpdateMousePosition()
	{
		const glm::vec2& mousePos { Mouse::GetMouseX(), Mouse::GetMouseY() };
		m_MouseDelta = (mousePos - m_MousePos);
		m_MousePos = mousePos;

		if (Keyboard::IsKeyPressed(Key::Escape))
			m_MouseEnabled = false;
	}

	glm::vec3 Camera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::GetDirection()
	{
		glm::vec3 lookAt = m_Position + GetForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Position);
		return m_Direction;
	}

	glm::vec3 Camera::CalculatePosition() const
	{
		return m_Origin - GetForwardDirection() * m_Distance;
	}

	// Basic Camera settings
	void Camera::OnImGuiRender()
	{
	 	 ImGui::Begin("Camera settings");

	 	 //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	 	 //if (ImGui::TreeNode("Draw mode"))
	 	 //{
	 	 //	if (ImGui::MenuItem("Lines"))
	 	 //		Renderer::s_DrawMode = LK_DRAW_LINES;
	 	 //	else if (ImGui::MenuItem("Triangles"))
	 	 //		Renderer::s_DrawMode = LK_DRAW_TRIANGLES;
	 	 //	ImGui::TreePop();
	 	 //}
	 	 ImGui::Separator();

		 // Sliders
	 	 ImGui::SliderFloat("Camera speed", &m_TravelSpeed, 0.010f, 2.0f);
		 float fov = GetFOV();
	 	 ImGui::SliderFloat("FOV", &fov, 40.0f, 105.0f);
		 SetFOV(fov);
		 glm::vec3 pos = GetPosition();
	 	 ImGui::SliderFloat3("Camera position", &pos.x, -50.0f, 50.0f);
		 SetPosition(pos);

		 ImGui::End();
	}

}