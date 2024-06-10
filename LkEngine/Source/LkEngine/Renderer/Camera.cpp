#include "LKpch.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	Camera::Camera(const glm::mat4& projection)
		: m_ProjectionMatrix(projection)
	{
	}

	Camera::Camera(const float degFov, const float width, const float height, const float nearP, const float farP)
		: m_ProjectionMatrix(glm::perspectiveFov(glm::radians(degFov), width, height, farP, nearP))
	{
	}

	std::string Camera::GetTypeStr() const
	{
		switch (m_Type)
		{
			case Type::Scene:  return "Scene";
			case Type::Editor: return "Editor";
			case Type::None:   return "None";
		}
		return "NULL";
	}


}