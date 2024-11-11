#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"
#include "Camera.h"


namespace LkEngine {

	LCamera::LCamera()
	{
		LCLASS_REGISTER();
	}

	LCamera::LCamera(const glm::mat4& InProjection)
		: m_ProjectionMatrix(InProjection)
	{
		LCLASS_REGISTER();
	}

	LCamera::LCamera(const float InDegFov, const float InWidth, const float InHeight, 
					 const float InNearP, const float InFarP)
		: LCamera(glm::perspectiveFov(glm::radians(InDegFov), InWidth, InHeight, InFarP, InNearP))
	{
		//: m_ProjectionMatrix(glm::perspectiveFov(glm::radians(InDegFov), InWidth, InHeight, InFarP, InNearP))
		//LCLASS_REGISTER();
	}

}