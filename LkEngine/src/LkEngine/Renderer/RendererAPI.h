#pragma once

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	enum class RendererAPIType
	{
		None,
		OpenGL,
		Vulkan
	};

    class RendererAPI
    {
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		static s_ptr<RendererAPI> Create();

		virtual void Clear() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Draw(VertexBuffer& vb, const Shader& shader) = 0;
		virtual void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) = 0;
		virtual void DrawIndexed(unsigned int indexCount) = 0;
		virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0) = 0;

		static RendererAPIType Current() { return m_CurrentRendererAPI; }
		static void SetAPI(RendererAPIType api);

	protected:
		inline static RendererAPIType m_CurrentRendererAPI = RendererAPIType::OpenGL;
		static s_ptr<Renderer2D> m_Renderer2D;

		friend class Renderer;
    };

}
