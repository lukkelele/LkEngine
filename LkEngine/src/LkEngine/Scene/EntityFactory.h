#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

namespace LkEngine {

	class Scene;

	namespace EntityProperties {

		struct Cube
		{
		};

		struct Sphere
		{
		};
	}

	class EntityFactory
	{
	public:
		EntityFactory() = default;
		~EntityFactory() = default;

		static void CreateCube(EntityProperties::Cube props);
		static void CreateSphere(EntityProperties::Sphere props);
	};

}