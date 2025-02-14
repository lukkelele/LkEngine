#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Core/Application.h" /// REMOVE


namespace LkEngine
{
	void LSceneCamera::Tick(const float InDeltaTime)
	{
		if (bKeyboardEnabled)
		{
			if (LKeyboard::IsKeyPressed(EKey::W))
			{
				Position += glm::vec3(0, 1, 0) * InDeltaTime * TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::A))
			{
				Position -= glm::vec3(1, 0, 0) * InDeltaTime * TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::S))
			{
				Position -= glm::vec3(0, 1, 0) * InDeltaTime * TravelSpeed;
			}
			if (LKeyboard::IsKeyPressed(EKey::D))
			{
				Position += glm::vec3(1, 0, 0) * InDeltaTime * TravelSpeed;
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
			/* EMPTY */
		}

		/// FIXME: Use delegate here
        LWindow& Window = LWindow::Get();
		SetViewportSize(Window.GetViewportWidth(), Window.GetViewportHeight());
	}

	void LSceneCamera::SetViewportSize(uint32_t Width, uint32_t Height)
	{
		UpdateView();
		switch (ProjectionType)
		{
			case ECameraProjection::Perspective:
				SetPerspectiveProjectionMatrix(glm::radians(
					DegPerspectiveFOV), 
					(float)Width, 
					(float)Height, 
					PerspectiveNear, 
					PerspectiveFar
				);
				break;

			case ECameraProjection::Orthographic:
				SetOrthoProjectionMatrix(Width, Height, OrthographicNear, OrthographicFar);
				break;
		}
	}

	void LSceneCamera::UpdateViewProjection()
	{
		UpdateView(); 
		UpdateProjection();
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

		ProjectionMatrix = glm::ortho(Left, Right, Bottom, Top);
	}

	void LSceneCamera::UpdateView()
	{
		const glm::mat4 Transform = glm::translate( 
			glm::mat4(1.0f), 
			{ (Position.x + CameraBoom.x), (Position.y + CameraBoom.y), (Position.z + CameraBoom.z) }
		) * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0, 0, 1));

		ViewMatrix = glm::inverse(Transform);
	}

}
