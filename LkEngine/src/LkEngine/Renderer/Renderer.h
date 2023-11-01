#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/IndexBuffer.h"
#include "LkEngine/Renderer/UniformBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Scene/Components.h"
#include <glad/glad.h>

#define LK_DRAW_TRIANGLES  GL_TRIANGLES
#define LK_DRAW_LINES	   GL_LINES


namespace LkEngine {

	// Forward declaration
	class RendererAPI;
	class Entity;

	struct RenderPass
	{

	};

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Clear();
		static void SetDrawMode(int mode);
		static void Draw(Entity& entity);
		static void Draw(VertexBuffer& va, const Shader& shader);
		static void Draw(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawLines(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader);
		//static void Draw(VertexArray& va, const Shader& shader);
		//static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		//static void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		//static void DrawIndexed(VertexArray& va, unsigned int count);
		static void DrawIndexed(VertexBuffer& vb, unsigned int count);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
		static void DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);

		static s_ptr<Renderer2D> GetRenderer2D();// { return s_Renderer2D; }

	public:
		static int DrawMode;
		static glm::vec4 BackgroundColor;
	private:
		static s_ptr<RendererAPI> m_RendererAPI;
	};

}
