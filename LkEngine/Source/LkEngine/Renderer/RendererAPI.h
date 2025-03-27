/**
 * @file
 * @brief Renderer API.
 */
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
#include "Texture.h"
#include "UniformBufferSet.h"
#include "ArrayTexture.h"
#include "Mesh.h"

#include "RendererCapabilities.h"
#include "Renderer2DAPI.h"

#include "LkEngine/Scene/Components.h"

namespace LkEngine {

	/**
	 * @defgroup Renderer
	 * @{
	 */

	/**
	 * @enum ERenderTopology
	 * Type of topology.
	 */
	enum class ERenderTopology
	{ 
		Lines, 
		Triangles 
	};

    /**
     * @class LRendererAPI
	 * 
	 * Interface for renderer APIs.
	 *
	 * @ingroup Renderer
	 * @todo Might remove deriving from LObject here since the renderer API
	 *       should only be of static use anyways.
     */
    class LRendererAPI : public LObject
    {
	public:
		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Clear() = 0;

		virtual void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer,
									 TObjectPtr<LRenderPass> InRenderPass, 
									 bool InExplicitClear = false) = 0;
		virtual void EndRenderPass(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer) = 0;

		virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer, 
									TObjectPtr<LPipeline> InPipeline, 
									TObjectPtr<LMaterial> InMaterial, 
									TObjectPtr<LVertexBuffer> InVertexBuffer, 
									TObjectPtr<LIndexBuffer> InIndexBuffer, 
									const glm::mat4& InTransform, 
									const uint32_t IndexCount = 0) = 0;

        virtual void RenderMesh(TObjectPtr<LMesh>& InMesh, TObjectPtr<LShader>& InShader, const glm::mat4& InTransform) = 0;
        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
                                    TObjectPtr<LPipeline> Pipeline, 
                                    TObjectPtr<LVertexBuffer> InVertexBuffer, 
                                    TObjectPtr<LIndexBuffer> InIndexBuffer, 
                                    const glm::mat4& Transform, 
                                    uint32_t IndexCount) = 0;


		virtual void Draw(const LVertexBuffer& VertexBuffer, const LIndexBuffer& IndexBuffer, const LShader& Shader) = 0;
		virtual void Draw(LVertexBuffer& VertexBuffer, const LShader& Shader) = 0;

		virtual void SetPrimitiveTopology(const ERenderTopology InRenderTopology) = 0;
		virtual void SetDepthFunction(const EDepthFunction InDepthFunction) = 0;
		virtual void SetDepthEnabled(const bool Enabled) = 0;

		virtual FRendererCapabilities& GetCapabilities() = 0;

		FORCEINLINE static ERendererAPI Get() { return RendererAPI; }
		static void SetAPI(ERendererAPI InRendererApiType);

		FORCEINLINE static std::string GetName()
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

		static TObjectPtr<LRendererAPI> Create();

	protected:
		inline static ERendererAPI RendererAPI = ERendererAPI::OpenGL;

		friend class LRenderer;
		friend class LEditor; /// REMOVE

	private:
		LCLASS(LRendererAPI);
    };

	/** @} */

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

