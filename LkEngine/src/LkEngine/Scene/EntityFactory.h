#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

namespace LkEngine {

	class Scene;

	class EntityFactory
	{
	public:
		EntityFactory() = default;
		~EntityFactory() = default;

		static Entity CreateRectangle(Scene& scene, glm::vec2 xy_min, glm::vec2 xy_max);

	private:
		static Entity CreateRectangle(const std::string& name, Scene& scene, glm::vec2 xy_min, glm::vec2 xy_max);
	};

}