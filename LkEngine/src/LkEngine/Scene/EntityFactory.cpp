#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Application.h"
#include <random>
#include <chrono>
#include "LkEngine/Renderer/Color.h"


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
		float rect_width = p2_max.x - p1_min.x;
		float rect_height = p2_max.y - p1_min.y;

		int entityCount = scene.GetEntityCount();
		Entity entity = scene.CreateEntity(std::string(name));
		TransformComponent tc;
		MeshComponent mesh;

		glm::vec2 rect_size = { rect_width, rect_height };
		SpriteComponent sprite = SpriteComponent(rect_size, Color::Generate());

		auto app = Application::Get();
		auto window = app->GetWindow();
		float width = window->GetWidth();
		float height = window->GetHeight();
		float viewport_height = window->GetViewportHeight();

		float vertices[] = {
			0.0f,         0.0f,
			rect_width,   0.0f,
			rect_width,   rect_height,
			0.0f,         rect_height 
		};
		//float vertices[] = {
		//	-rect_width * 0.50f + (width * 0.50f),   -rect_height * 0.50f + (height * 0.50f),
		//	 rect_width * 0.50f + (width * 0.50f),   -rect_height * 0.50f + (height * 0.50f), 
		//	 rect_width * 0.50f + (width * 0.50f),    rect_height * 0.50f + (height * 0.50f), 
		//	-rect_width * 0.50f + (width * 0.50f),    rect_height * 0.50f + (height * 0.50f) 
		//};
		//float vertices[] = {
		//	-rect_width * 0.50f,   -rect_height * 0.50f,
		//	 rect_width * 0.50f,   -rect_height * 0.50f, 
		//	 rect_width * 0.50f,    rect_height * 0.50f, 
		//	-rect_width * 0.50f,    rect_height * 0.50f 
		//};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		mesh.Color = Color::Generate();
		mesh.VAO = create_s_ptr<VertexArray>();
		mesh.VBO = create_s_ptr<VertexBuffer>(vertices, LK_ARRAYSIZE(vertices));
		//VertexBufferLayout layout;
		//layout.Push<float>(2);
		mesh.VBO->SetLayout({ 
			{ "pos", ShaderDataType::Float2 } 
		});
		//mesh.VAO->AddBuffer(*mesh.VBO, layout);
		mesh.VAO->AddVertexBuffer(*mesh.VBO);
		//auto IBO = create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices));
		//mesh.VAO->SetIndexBuffer(create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices)));
		//mesh.VAO->SetIndexBuffer(create_s_ptr<IndexBuffer>(indices, LK_ARRAYSIZE(indices) * sizeof(unsigned int)));
		mesh.VAO->SetIndexBuffer(create_s_ptr<IndexBuffer>(indices, sizeof(indices)));
		mesh.BaseShader = create_s_ptr<Shader>("assets/shaders/basic_model_view_proj.shader");
		mesh.BaseShader->Bind();
		mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
		mesh.BaseShader->Unbind();
		//mesh.VAO->SetIndexBuffer(mesh.VAO->I);

		tc.Translation.x = p1_min.x; // + width / 2;
		tc.Translation.y = p1_min.y; // + height / 2;

		entity.AddComponent<MeshComponent>(mesh);
		entity.AddComponent<TransformComponent>(tc);
		entity.AddComponent<SpriteComponent>(sprite);

		return entity;
	}





}