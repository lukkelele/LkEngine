#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Core/Application.h" /// REMOVE


namespace LkEngine
{
	void LSceneCamera::SetViewportSize(uint32_t Width, uint32_t Height)
	{
		UpdateView();
		switch (ProjectionType)
		{
			case ECameraProjection::Perspective:
				SetPerspectiveProjectionMatrix(glm::radians(
					m_DegPerspectiveFOV), 
					(float)Width, 
					(float)Height, 
					m_PerspectiveNear, 
					m_PerspectiveFar
				);
				break;

			case ECameraProjection::Orthographic:
				SetOrthoProjectionMatrix(Width, Height, m_OrthographicNear, m_OrthographicFar);
				break;
		}
	}

	void LSceneCamera::UpdateViewProjection()
	{
		UpdateView(); 
		UpdateProjection();
	}

	void LSceneCamera::Update(float ts)
	{
		if (bKeyboardEnabled)
		{
			/* WASD */
			if (LKeyboard::IsKeyPressed(EKey::W))
			{
				m_Pos += glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::A))
			{
				m_Pos -= glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::S))
			{
				m_Pos -= glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::D))
			{
				m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
			}

			if (LKeyboard::IsKeyPressed(EKey::Q))
			{
				//m_Zoom += 0.010f;
			}
			if (LKeyboard::IsKeyPressed(EKey::R))
			{
				//m_Zoom -= 0.010f;
			}
		}

		if (bMouseEnabled)
		{
		}

		/// FIXME: Use delegate here
        LWindow& Window = LWindow::Get();
		SetViewportSize(Window.GetViewportWidth(), Window.GetViewportHeight());
	}

	void LSceneCamera::UpdateProjection()
	{
		float Width, Height;
		float Left, Right, Bottom, Top;

		LWindow& Window = LWindow::Get();
		Width = Window.GetViewportWidth();
		Height = Window.GetViewportWidth();

		Left = -Width / 2.0f;
		Right = Width / 2.0f;
		Bottom = -Height / 2.0f;
		Top = Height / 2.0f;

		m_ProjectionMatrix = glm::ortho(Left, Right, Bottom, Top);
	}

	void LSceneCamera::UpdateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { (m_Pos.x + m_Offset.x), (m_Pos.y + m_Offset.y), (m_Pos.z + m_Offset.z) }) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}
