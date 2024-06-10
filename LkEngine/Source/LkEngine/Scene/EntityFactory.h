#pragma once

#include "LkEngine/Core/Base.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	class Scene;

	class EntityFactory
	{
	public:
		static LEntity CreateQuad(Scene& scene, const glm::vec2 pos, const glm::vec2 size, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static LEntity CreateQuad(Scene& scene, const glm::vec3 pos, const glm::vec2 size, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static LEntity CreateLine(Scene& scene, const glm::vec2 p0, const glm::vec2 p1, float lineWidth = 2.0f, const glm::vec4& color = { 0.50f, 0.50f, 0.50f, 1.0f });
		static LEntity CreateGrid(Scene& scene, int rows, int columns, float spacingX = 10.0f, float spacingY = 10.0f);
	};

}