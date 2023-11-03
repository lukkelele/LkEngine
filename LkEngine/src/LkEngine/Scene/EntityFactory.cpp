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

	Entity EntityFactory::CreateGrid(Scene& scene, int rows, int columns, float spacingX, float spacingY)
	{
		int entityCount = scene.GetEntityCount();
		std::string name = "Quad_" + std::to_string(entityCount);
		Entity entity = scene.CreateEntity(std::string(name));

		glm::vec2 windowSize = Window::Get()->GetSize();
		glm::vec2 windowPos = Window::Get()->GetPos();
		//glm::vec2 spriteSize = Window::Get()->GetSize();
		//glm::vec2 spritePos = Window::Get()->GetPos();

		float windowWidth = windowSize.x;
		float windowHeight = windowSize.y;

        float quadWidth = windowWidth / columns;
        float quadHeight = windowHeight / rows;
        glm::vec2 quadSize = { quadWidth, quadHeight };
        glm::vec2 smallerQuadSize = { quadWidth - 10.0f, quadHeight - 10.f };

		SpriteComponent sc = SpriteComponent(quadSize);
		TransformComponent tc = TransformComponent({ windowPos.x, windowPos.y, 0.0f });
		entity.AddComponent<SpriteComponent>(sc);
		entity.AddComponent<TransformComponent>(tc);

		return entity;
	}



}