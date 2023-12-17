#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

	SceneCamera::SceneCamera()
	{
		m_View = glm::mat4(1.0f);
		m_Projection = glm::mat4(1.0f);
		ID = UUID();
	}

	void SceneCamera::Update(float ts)
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

		//UpdateView();
	}

	void SceneCamera::UpdateProjection()
	{
		//m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
		// TEMPORARY FOR 2D
        //auto window = ImGui::DockBuilderGetNode(LkEngine::DockSpace::RenderWindowDockID);
		//float left = -window->Size.x;
		//float right = window->Size.x;
		//float bottom = -window->Size.y;
		//float top = window->Size.y;
		float left, right, bottom, top;
		float width, height;
		auto window = Application::Get()->GetWindow();
		width = window->GetWidth();
		height = window->GetWidth();
		left = -width / 2.0f;
		right = width / 2.0f;
		bottom = -height / 2.0f;
		top = height / 2.0f;
		m_Projection = glm::ortho(left, right, bottom, top);
	}

	// FIXME: ortographic here, should be able to switch 
	void SceneCamera::UpdateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Pos) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}


}