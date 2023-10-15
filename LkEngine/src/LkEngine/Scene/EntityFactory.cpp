#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"


namespace LkEngine {

	void EntityFactory::CreateCube(EntityProperties::Cube props)
	{
	#ifdef LK_ENGINE_OLD_IMPL
		auto scene = Scene::GetActiveScene();
		Entity entity = scene->CreateEntity(props.name);
		std::string shaderPath = "assets/shaders/flat.shader";
		glm::vec3 inertia(0.0f, 0.0f, 0.0f);
		float restitution = 0.3f;
		float friction = 1.0f;

		UUID& entityID = entity.getUUID();
		Mesh& mesh = MeshFactory::CreateCube(props.dimensions);
		Rigidbody& rigidbody = RigidbodyFactory::CreateRigidbody(entityID, Rigidbody::Shape::Cube, props.rigidbodyType, props.dimensions, props.offset, 
											props.mass, friction, restitution, inertia);

		BoxColliderShape& boxColliderShape = BoxColliderShape(entityID, props.dimensions, props.offset, 
			props.mass, inertia, props.rigidbodyType);
		Material material;
		material.SetMaterialColor(props.color);

		entity.AddComponent<Mesh>(mesh);
		// entity.AddComponent<Rigidbody>(rigidbody);
		entity.AddComponent<BoxColliderShape>(boxColliderShape);
		entity.AddComponent<Material>(material);
		entity.AddComponent<TransformComponent>();
	#endif
	}

	void EntityFactory::CreateSphere(EntityProperties::Sphere props)
	{
	#ifdef LK_ENGINE_OLD_IMPL
		auto scene = Scene::GetActiveScene();
		Entity entity = scene->CreateEntity(props.name);
		std::string shaderPath = "assets/shaders/flat.shader";
		glm::vec3 inertia(0.0f, 0.0f, 0.0f);
		float restitution = 0.3f;
		float friction = 1.0f;

		// UUID& entityID = entity.getUUID();
		// Mesh& mesh = MeshFactory::CreateSphere(props.radius);
		// Rigidbody& rigidbody = RigidbodyFactory::CreateRigidbody(entityID, props.dimensions, props.offset, 
		// 									  props.mass, props.bodytype, friction, restitution, inertia);

		// Material material;
		// material.setMaterialColor(props.color);

		// entity.AddComponent<Mesh>(mesh);
		// entity.AddComponent<Rigidbody>(rigidbody);
		// entity.AddComponent<Material>(material);
		// entity.AddComponent<TransformComponent>();
	#endif 
	}


}