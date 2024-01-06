#pragma once

#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	enum class RendererAPIType
	{
		None,
		OpenGL,
		Vulkan
	};

	enum class RendererDrawMode 
	{ 
		Lines, 
		Triangles 
	};

    class RendererAPI
    {
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		static s_ptr<RendererAPI> Create();
		static s_ptr<Renderer2D>& GetRenderer2D() { return m_Renderer2D; }
		static void SetAPI(RendererAPIType api);
		static RendererAPIType Current() { return m_CurrentRendererAPI; }

		virtual void Clear() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void SetDrawMode(const RendererDrawMode& mode) = 0;
		virtual void Draw(VertexBuffer& vb, const Shader& shader) = 0;
		virtual void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) = 0;
		virtual void SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) = 0;
		virtual void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0) = 0;
		virtual void SubmitIndexed(unsigned int indexCount) = 0;

	protected:
		inline static RendererAPIType m_CurrentRendererAPI = RendererAPIType::OpenGL;
		inline static s_ptr<Renderer2D> m_Renderer2D = nullptr;

		friend class Renderer;
    };

}
