#pragma once

#include "Framebuffer.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "RenderCommandBuffer.h"
#include "RenderCommandQueue.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderContext.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "TextureLibrary.h"
#include "MaterialLibrary.h"
#include "UniformBufferSet.h"
#include "TextureArray.h"
#include "Mesh.h"

#include "RendererCapabilities.h"
#include "Renderer2DAPI.h"

#include "LkEngine/Scene/Components.h"


namespace LkEngine {

	enum class ERenderTopology
	{ 
		Lines, 
		Triangles 
	};
	
    class LRendererAPI : public LObject
    {
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Clear() = 0;

		virtual void SubmitImage(const TObjectPtr<LImage> Image) = 0;
		virtual void SubmitImage(const TObjectPtr<LImage2D> Image) = 0;

		virtual void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer,
									 TObjectPtr<LRenderPass> renderPass, 
									 bool InExplicitClear = false) = 0;

		virtual void EndRenderPass(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer) = 0;

		// RenderGeometry
		virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer, 
									TObjectPtr<LPipeline> InPipeline, 
									TObjectPtr<LVertexBuffer> InVertexBuffer, 
									TObjectPtr<LIndexBuffer> InIndexBuffer, 
									const glm::mat4& InTransform, 
									uint32_t IndexCount = 0) = 0;

		virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer, 
									TObjectPtr<LPipeline> InPipeline, 
									TObjectPtr<LShader>& shader, 
									TObjectPtr<LVertexBuffer>& InVertexBuffer, 
									TObjectPtr<LIndexBuffer>& InIndexBuffer, 
									const glm::mat4& InTransform, 
									const uint32_t IndexCount = 0) = 0;

		virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer, 
									TObjectPtr<LPipeline> InPipeline, 
									TObjectPtr<LMaterial> InMaterial, 
									TObjectPtr<LVertexBuffer> InVertexBuffer, 
									TObjectPtr<LIndexBuffer> InIndexBuffer, 
									const glm::mat4& InTransform, 
									const uint32_t IndexCount = 0) = 0;
		// ~RenderGeometry

		// Draw
		virtual void Draw(const LVertexBuffer& vb, const LIndexBuffer& ib, const LShader& shader) = 0;
		virtual void Draw(LVertexBuffer& vb, const LShader& shader) = 0;
		// ~Draw

        virtual void DrawIndexed(uint64_t IndexCount) = 0;

		virtual void SubmitMesh(TObjectPtr<LMesh>& InMesh, 
								TObjectPtr<LShader>& InShader, 
								const glm::mat4& InTransform) = 0;

		virtual void SubmitIndexed(unsigned int IndexCount) = 0;

		// SubmitQuad
		virtual void SubmitQuad(const glm::vec2& pos, 
								const glm::vec2& size, 
								const glm::vec4& color, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec3& pos, 
								const glm::vec2& size, 
								const glm::vec4& color, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec2& pos, 
								const glm::vec2& size, 
								const glm::vec4& color, 
								const float rotation, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec3& pos, 
								const glm::vec2& size, 
								const glm::vec4& color, 
								const float rotation, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec2& pos, 
								const glm::vec2& size, 
								TObjectPtr<LTexture> texture, 
								float rotation, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec3& pos, 
								const glm::vec2& size, 
								TObjectPtr<LTexture> texture, 
								const float rotation, 
								uint64_t entityID = 0) = 0;

		virtual void SubmitQuad(const glm::vec2& pos, 
								const glm::vec2& size, 
								TObjectPtr<LTexture> texture, 
								const glm::vec4& tintColor, 
								const float rotation, 
								uint64_t entityID) = 0;

		virtual void SubmitQuad(const glm::vec3& pos, 
								const glm::vec2& size, 
								TObjectPtr<LTexture> texture, 
								const glm::vec4& tintColor, 
								const float rotation, 
								uint64_t entityID) = 0;
		// ~SubmitQuad

		virtual void SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, uint64_t entityID = 0) = 0;

		virtual void SetPrimitiveTopology(const ERenderTopology& InRenderTopology) = 0;
		virtual void SetDepthFunction(const EDepthFunction& InDepthFunction) = 0;

		virtual RendererCapabilities& GetCapabilities() = 0;

		static TObjectPtr<LRendererAPI> Create();

		/** Get current RendererAPI. */
		FORCEINLINE static ERendererAPI Current() { return RendererAPI; }

		static void SetAPI(ERendererAPI InRendererApiType);

		FORCEINLINE static LString GetName()
		{
			switch (RendererAPI)
			{
				case ERendererAPI::OpenGL: return "OpenGL";
				case ERendererAPI::Vulkan: return "Vulkan";
				default: break; 
			}

			LK_CORE_ASSERT(false, "Invalid render API");
			return {};
		}

	protected:
		inline static ERendererAPI RendererAPI = ERendererAPI::OpenGL;

		friend class LEditor;
		friend class LRenderer;

	private:
		LCLASS(LRendererAPI);
    };

	namespace Enum
	{
		FORCEINLINE static constexpr const char* ToString(const ERenderTopology RenderTopology)
		{
			switch (RenderTopology)
			{
				case ERenderTopology::Lines:     return "Lines";
				case ERenderTopology::Triangles: return "Triangles";
			}

			LK_CORE_ASSERT(false);
			return nullptr;
		}
	}

}

