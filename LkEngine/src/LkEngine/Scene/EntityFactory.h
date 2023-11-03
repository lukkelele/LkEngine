#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

namespace LkEngine {

	class Scene;

	class EntityFactory
	{
	public:
		EntityFactory() = default;
		~EntityFactory() = default;

		static Entity CreateQuad(Scene& scene, const glm::vec2 pos, const glm::vec2 size, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static Entity CreateQuad(Scene& scene, const glm::vec3 pos, const glm::vec2 size, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static Entity CreateLine(Scene& scene, const glm::vec2 p0, const glm::vec2 p1, float lineWidth = 2.0f, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static Entity CreateRectangle(Scene& scene, glm::vec2 xy_min, glm::vec2 xy_max);
		static Entity CreateRectangle(const std::string& name, Scene& scene, glm::vec2 xy_min, glm::vec2 xy_max);
		static Entity CreateGrid(Scene& scene, int rows, int columns, float spacingX = 10.0f, float spacingY = 10.0f);
	};

}