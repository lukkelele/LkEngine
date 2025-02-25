#include "LKpch.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine
{
	void LSceneCamera::SetViewportSize(uint32_t Width, uint32_t Height)
	{
		switch (ProjectionType)
		{
			case ECameraProjection::Perspective:
				SetPerspectiveProjectionMatrix(
					glm::radians(DegPerspectiveFov), 
					(float)Width, 
					(float)Height, 
					PerspectiveNear, 
					PerspectiveFar
				);
				break;

			case ECameraProjection::Orthographic:
				SetOrthoProjectionMatrix((float)Width, (float)Height, OrthographicNear, OrthographicFar);
				break;
		}
	}

}
