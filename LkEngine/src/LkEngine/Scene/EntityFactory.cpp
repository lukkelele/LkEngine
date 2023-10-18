#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Application.h"


namespace LkEngine {

	//
	// 3 - 2
	// |  /|
	// | / |
	// |/  |
	// 0 - 1
	//
	// [ Bottom Left ] <--> [ Top Right ]
	Entity EntityFactory::CreateRectangle(Scene& scene, glm::vec2 p1_min, glm::vec2 p2_max)
	{
		int entityCount = scene.GetEntityCount();
		std::string name = "rectangle_" + std::to_string(entityCount);
		return CreateRectangle(name, scene, p1_min, p2_max);
	}

	//
	// 3 - 2
	// |  /|
	// | / |
	// |/  |
	// 0 - 1
	//
	// [ Bottom Left ] <--> [ Top Right ]
	Entity EntityFactory::CreateRectangle(const std::string& name, Scene& scene, glm::vec2 p1_min, glm::vec2 p2_max)
	{
		int entityCount = scene.GetEntityCount();
		Entity entity = scene.CreateEntity(std::string(name));
		entity.AddComponent<TransformComponent>();
		MeshComponent mesh = MeshComponent();
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

		//LOG_DEBUG("Created rectangle ({}, {}) - ({}, {})", p1_min.x, p1_min.y, p2_max.x, p2_max.y);
		float rect_width = p2_max.x - p1_min.x;
		float rect_height = p2_max.y - p1_min.y;

		mesh.VAO = create_s_ptr<VertexArray>();
		mesh.VBO = create_s_ptr<VertexBuffer>(vertices, LK_ARRAYSIZE(vertices));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		mesh.VAO->AddBuffer(*mesh.VBO, layout);
		mesh.IBO = create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices));
		mesh.BaseShader = create_s_ptr<Shader>("assets/shaders/basic_transform.shader");
		mesh.BaseShader->Bind();
		mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
		mesh.BaseShader->Unbind();

		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		auto app = Application::Get();
		int screen_width = app->GetWidth();   // for 'render window'
		int screen_height = app->GetHeight(); // for 'render window'
		//float center_x = (screen_width - rect_width) * 0.50f;
		//float center_y = (screen_height - rect_height) * 0.50f;
		float center_x = (screen_width * 0.50f) - rect_width * 0.50f;
		float center_y = (screen_height - rect_height) * 0.50f;
		transform.Translation.x = (2.0f * center_x) / screen_width - 1.0f;
		transform.Translation.y = (2.0f * center_y) / screen_height - 1.0f;

		entity.AddComponent<MeshComponent>(mesh);

		return entity;
	}





}