#pragma once

#include "LkEngine/Core/Core.h"

#if 0
#include "ContactListener2D.h"
#include "LkEngine/Scene/Entity.h"

#include <glm/glm.hpp>


namespace LkEngine {

	class LScene;

	// Data structure used for creating 2D worlds after deserializing scene data
	struct Physics2DSpecification
	{
		bool Paused = false;
		bool DebugDrawer = true;
		glm::vec2 Gravity = glm::vec2(0.0f, -9.20f);
	};

	struct Raycast2DResult
	{
		LEntity HitEntity;
		glm::vec2 Point;
		glm::vec2 Normal;
		float Distance; // World units

		Raycast2DResult(LEntity entity, glm::vec2 point, glm::vec2 normal, float distance)
			: HitEntity(entity)
			, Point(point)
			, Normal(normal)
			, Distance(distance) 
		{
		}

		FORCEINLINE std::string ToString() const
		{
			std::ostringstream oss;
			oss << "Normal: " << Normal.x << ", " << Normal.y << "\n";

			return oss.str();
		}
	};

	enum class EPhysics2D
	{ 
		None = 0, 
		Box2D, 
	};

	class Physics2D
	{
	public:
	};
}

#endif