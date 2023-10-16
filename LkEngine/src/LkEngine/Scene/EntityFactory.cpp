#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

	//
	// 3 - 2
	// |  /|
	// | / |
	// |/  |
	// 0 - 1
	//
	// [ Bottom Left ] <--> [ Top Right ]
	Entity& EntityFactory::CreateRectangle(std::string_view name, Scene& scene, glm::vec2 p1_min, glm::vec2 p2_max)
	{
		int entityCount = scene.GetEntityCount();
		Entity entity = scene.CreateEntity(std::string(name));
		entity.AddComponent<TransformComponent>();
		MeshComponent mesh = MeshComponent();
		//float vertices[4] = { 
		//	p1_min.x, p1_min.y, 
		//	p2_max.x, p2_max.y 
		//};
		float vertices[] = { 
			p1_min.x, p1_min.y, 
			p2_max.x, p1_min.y,
			p2_max.x, p2_max.y,
			p1_min.x, p2_max.y
		};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		mesh.AddVertex(p1_min, { 0.0f, 0.0f });
		mesh.AddVertex({ p2_max.x, p1_min.y }, { 0.0f, 0.0f });
		mesh.AddVertex(p2_max, { 0.0f, 0.0f });
		mesh.AddVertex({ p1_min.x, p2_max.y }, { 0.0f, 0.0f });

		LOG_DEBUG("[CREATED] (entity) Rectangle ({}, {}) - ({}, {})", p1_min.x, p1_min.y, p2_max.x, p2_max.y);

		mesh.VAO = create_s_ptr<VertexArray>();
		mesh.VBO = create_s_ptr<VertexBuffer>(vertices, LK_ARRAYSIZE(vertices));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		mesh.VAO->AddBuffer(*mesh.VBO, layout);
		mesh.IBO = create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices));
		mesh.Shader = create_s_ptr<Shader>("assets/shaders/basic_transform.shader");

		entity.AddComponent<MeshComponent>(mesh);

		return entity;
	}


	// Entity& EntityFactory::CreateRectangle(Scene* scene, float width, float height, float pos_x, float pos_y)
	// {
	// 	int entityCount = scene->GetEntityCount();
	// 	std::string entity_name = std::to_string(entityCount++);
	// 	Entity entity = scene->CreateEntity(entity_name);
	// 	entity.AddComponent<TransformComponent>();
	// 	glm::vec3 pos = { pos_x, pos_y, 0.0f };
	// 	glm::vec2 uv = { 0.0f, 0.0f };
	// 	MeshComponent mesh = MeshComponent();
	// 	mesh.AddVertex(glm::vec3(pos)
	// 	//entity.AddComponent()
	// 	return entity;
	// }


	#ifdef LK_ENGINE_OLD_IMPL
	void EntityFactory::CreateCube(EntityProperties::Cube props)
	{
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
	#endif


}