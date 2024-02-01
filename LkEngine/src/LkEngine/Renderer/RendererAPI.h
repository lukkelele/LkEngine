#pragma once

#include "SwapChain.h"
#include "RenderPass.h"
#include "RenderCommandBuffer.h"
#include "RenderCommandQueue.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GraphicsContext.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "TextureLibrary.h"
#include "MaterialLibrary.h"
#include "UniformBufferSet.h"
#include "Framebuffer.h"

#include "RendererCapabilities.h"

#include "LkEngine/Scene/Components.h"

#include "Renderer2DAPI.h"


namespace LkEngine {

	// TODO: Remove, use Topology instead
	enum class RendererDrawMode 
	{ 
		Lines, 
		Triangles 
	};

    class RendererAPI : public RefCounted
    {
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Clear() = 0;

		virtual void SubmitImage(const Ref<Image> image) = 0;
		virtual void SubmitImage(const Ref<Image2D> image) = 0;

		virtual void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear = false) = 0;
		virtual void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer) = 0;
		virtual void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0) = 0;
		//virtual void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader> shader, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0) = 0;
		virtual void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader>& shader, Ref<VertexBuffer>& vertexBuffer, Ref<IndexBuffer>& indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0) = 0;
		virtual void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0) = 0;

		virtual void SetDrawMode(const RendererDrawMode& mode) = 0;
		virtual void Draw(VertexBuffer& vb, const Shader& shader) = 0;
		virtual void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) = 0;
        virtual void DrawIndexed(uint64_t indexCount) = 0;

		virtual void SubmitIndexed(unsigned int indexCount) = 0;

		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) = 0;
		virtual void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) = 0;
		virtual void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) = 0;

		virtual void SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, uint64_t entityID = 0) = 0;

		static RendererAPIType Current() { return m_CurrentRendererAPI; }
		static Ref<RendererAPI> Create();
		static void SetAPI(RendererAPIType api);

		virtual Ref<Renderer2DAPI> GetRenderer2D() = 0;
		virtual Ref<Renderer2DAPI> GetRenderer2DAPI() = 0;

		virtual RendererCapabilities& GetCapabilities() = 0;
		
	protected:
		inline static RendererAPIType m_CurrentRendererAPI = RendererAPIType::OpenGL;

		friend class Renderer;
		friend class Editor;
    };

}

