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

		static Entity& CreateRectangle(std::string_view name, Scene& scene, glm::vec2 xy_min, glm::vec2 xy_max);

		//static Entity& CreateRectangle(Scene* scene, float width, float height, float pos_x = 0.0f, float pos_y = 0.0f);
	};

}