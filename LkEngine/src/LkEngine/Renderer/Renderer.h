#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/IndexBuffer.h"
#include "LkEngine/Renderer/UniformBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Scene/Components.h"
#include <glad/glad.h>

#define LK_DRAWMODE_TRIANGLES  GL_TRIANGLES
#define LK_DRAWMODE_LINES	   GL_LINES

namespace LkEngine {

	// Forward declaration
	class Entity;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static s_ptr<Renderer> Create() { return std::make_shared<Renderer>(); }

		static void Clear();
		static void BeginFrame();
		static void EndFrame();
		static void SetDrawMode(int mode);
		static void SubmitLines(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader);
		static void SubmitIndexed(VertexBuffer& vb, unsigned int count);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
        static void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);
		static void SubmitSprite(TransformComponent& tc, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID = 0);
		static s_ptr<Renderer2D> GetRenderer2D() { return m_RendererAPI->GetRenderer2D(); }

	public:
		inline static int s_DrawMode = GL_TRIANGLES;
		inline static glm::vec4 s_BackgroundColor = { 0.50f, 0.80f, 0.35f, 1.0f };
		//inline static uint64_t BoundTextureID = 0;
		inline static bool s_TexturesEnabled = true;
	private:
		inline static s_ptr<RendererAPI> m_RendererAPI = nullptr;
	};

}
