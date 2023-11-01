#include "LKpch.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Core/Application.h"
#include <random>
#include <chrono>
#include "LkEngine/Renderer/Color.h"


namespace LkEngine {

	Entity EntityFactory::CreateQuad(Scene& scene, const glm::vec2 pos, const glm::vec2 size, const glm::vec4& color)
	{
		return CreateQuad(scene, { pos.x, pos.y, 0.0f }, size, color);
	}

	Entity EntityFactory::CreateQuad(Scene& scene, const glm::vec3 pos, const glm::vec2 size, const glm::vec4& color)
	{
		int entityCount = scene.GetEntityCount();
		std::string name = "Quad_" + std::to_string(entityCount);
		Entity entity = scene.CreateEntity(std::string(name));

		SpriteComponent sc = SpriteComponent(size, color);
		TransformComponent tc = TransformComponent(pos);
		entity.AddComponent<SpriteComponent>(sc);
		entity.AddComponent<TransformComponent>(tc);

		return entity;
	}

	// TODO: Fix this so the angle between p0 and p1 makes it so size in x axis equals lineWidth and then rotate
	Entity EntityFactory::CreateLine(Scene& scene, const glm::vec2 p0, const glm::vec2 p1, float lineWidth, const glm::vec4& color)
	{
		int entityCount = scene.GetEntityCount();
		std::string name = "Line_" + std::to_string(entityCount);
		Entity entity = scene.CreateEntity(std::string(name));

		//glm::vec2 size = { p1.x - p0.x, p1.y - p0.y };
		glm::vec2 size = { lineWidth, p1.y - p0.y };
		float angleRad = atan2((p1.x - p0.x), (p1.y - p0.y));
		float angleDeg = glm::degrees(angleRad);
		LOG_DEBUG("Created Line entity with rotation: {} (degrees)", angleDeg);

		SpriteComponent sc = SpriteComponent(size, color);
		TransformComponent tc = TransformComponent({ p0.x, p0.y, 0.0f });
		tc.Rotation2D = angleDeg;

		entity.AddComponent<SpriteComponent>(sc);
		entity.AddComponent<TransformComponent>(tc);

		return entity;
	}



	// 3 - 2
	// |  /|
	// | / |
	// |/  |
	// 0 - 1
	Entity EntityFactory::CreateRectangle(Scene& scene, glm::vec2 p1_min, glm::vec2 p2_max)
	{
		int entityCount = scene.GetEntityCount();
		std::string name = "Rectangle_" + std::to_string(entityCount);
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
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		mesh.Color = Color::Generate();
		mesh.VAO = create_s_ptr<VertexArray>();
		mesh.VBO = VertexBuffer::Create(vertices, LK_ARRAYSIZE(vertices));
		mesh.VBO->SetLayout({ 
			{ "pos", ShaderDataType::Float2 } 
		});

		mesh.VAO->AddVertexBuffer(*mesh.VBO);
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