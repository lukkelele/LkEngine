#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Application.h"
#include <random>
#include <chrono>


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
		//mesh.AddVertex(p1_min, { 0.0f, 0.0f });
		//mesh.AddVertex({ p2_max.x, p1_min.y }, { 0.0f, 0.0f });
		//mesh.AddVertex(p2_max, { 0.0f, 0.0f });
		//mesh.AddVertex({ p1_min.x, p2_max.y }, { 0.0f, 0.0f });
		//LOG_DEBUG("Created rectangle ({}, {}) - ({}, {})", p1_min.x, p1_min.y, p2_max.x, p2_max.y);
		float rect_width = p2_max.x - p1_min.x;
		float rect_height = p2_max.y - p1_min.y;

		static std::mt19937 rng(std::time(0));
		static std::uniform_int_distribution<int> dist(0, 100);
		mesh.Color.x = (float)(dist(rng) * 0.01f);
		mesh.Color.y = (float)(dist(rng) * 0.01f);
		mesh.Color.z = (float)(dist(rng) * 0.01f);

		mesh.VAO = create_s_ptr<VertexArray>();
		mesh.VBO = create_s_ptr<VertexBuffer>(vertices, LK_ARRAYSIZE(vertices));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		mesh.VAO->AddBuffer(*mesh.VBO, layout);
		mesh.IBO = create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices));
		mesh.BaseShader = create_s_ptr<Shader>("assets/shaders/basic_model_view_proj.shader");
		mesh.BaseShader->Bind();
		mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
		mesh.BaseShader->Unbind();

		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		//auto app = Application::Get();
		entity.AddComponent<MeshComponent>(mesh);
		transform.Translation.x = p1_min.x;
		transform.Translation.y = p2_max.y;

		return entity;
	}





}