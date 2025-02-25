#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"
#include "Camera.h"


namespace LkEngine {

	LCamera::LCamera()
	{
		LOBJECT_REGISTER();
	}

	LCamera::LCamera(const glm::mat4& InProjection)
		: ProjectionMatrix(InProjection)
	{
		LOBJECT_REGISTER();
	}

	LCamera::LCamera(const float InDegFov, 
					 const float InWidth, 
					 const float InHeight, 
					 const float InNearP, 
					 const float InFarP)
		: LCamera(glm::perspectiveFov(glm::radians(InDegFov), InWidth, InHeight, InFarP, InNearP))
	{
	}

}
