#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/IndexBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
//#include "LkEngine/Renderer/Mesh.h"

#include <glad/glad.h>

#define LK_DRAW_TRIANGLES  GL_TRIANGLES
#define LK_DRAW_LINES	   GL_LINES


namespace LkEngine {

	class Entity;

	class Renderer
	{
	public:
		static void Clear();

		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawTriangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawIndexed(const s_ptr<VertexArray>& va);
		static void SetDrawMode(int mode);

		// Static because debugging for raycast
		//static void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::mat4& viewProj, const glm::vec4& color = glm::vec4(1.0f));
		//void DrawWireframe(Entity& entity, glm::vec4 color) const;
		//static void DrawShape(Entity& entity);
		//static void OnWindowResize(uint16_t width, uint16_t height);

	public:
		static int DrawMode;
		static glm::vec4 BackgroundColor;
	};

}
