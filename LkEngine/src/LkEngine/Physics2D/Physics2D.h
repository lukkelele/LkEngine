#pragma once

#include "ContactListener2D.h"
#include "LkEngine/Scene/Entity.h"

//#include <box2d/b2_world.h>

#include <glm/glm.hpp>


namespace LkEngine {

	// Forward declarations
	class Scene;
	//class Entity;

	struct Raycast2DResult
	{
		Entity HitEntity;
		glm::vec2 Point;
		glm::vec2 Normal;
		float Distance; // World units

		Raycast2DResult(Entity entity, glm::vec2 point, glm::vec2 normal, float distance)
			: HitEntity(entity)
			, Point(point)
			, Normal(normal)
			, Distance(distance) {}
	};

	class Physics2D
	{
	public:
		static std::vector<Raycast2DResult> Raycast(Scene& scene, const glm::vec2& point0, const glm::vec2& point1);
		static std::vector<Raycast2DResult> RaycastFromScreen(Scene& scene);
	};


}