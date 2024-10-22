#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	LCamera::LCamera(const glm::mat4& InProjection)
		: m_ProjectionMatrix(InProjection)
	{
	}

	LCamera::LCamera(const float InDegFov, 
					 const float InWidth, 
					 const float InHeight, 
					 const float InNearP, 
					 const float InFarP)
		: m_ProjectionMatrix(glm::perspectiveFov(glm::radians(InDegFov), InWidth, InHeight, InFarP, InNearP))
	{
	}

}