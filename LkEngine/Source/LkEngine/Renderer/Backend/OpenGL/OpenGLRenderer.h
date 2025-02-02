#pragma once

#include "LkEngine/Renderer/RendererAPI.h"

#include "OpenGLPipeline.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLContext.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLRenderPass.h"
#include "OpenGLRenderCommandBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLArrayTexture.h"
#include "OpenGLImGuiLayer.h"


namespace LkEngine {

    class LOpenGLRenderer : public LRendererAPI
    {
    public:
        LOpenGLRenderer();
		~LOpenGLRenderer() = default;

        virtual void Initialize() override;
        virtual void Destroy() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void Clear() override;

        virtual void RenderMesh(TObjectPtr<LMesh>& InMesh, TObjectPtr<LShader>& InShader, const glm::mat4& InTransform) override;
        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
                                    TObjectPtr<LPipeline> Pipeline, 
                                    TObjectPtr<LVertexBuffer> InVertexBuffer, 
                                    TObjectPtr<LIndexBuffer> InIndexBuffer, 
                                    const glm::mat4& Transform, 
                                    uint32_t IndexCount) override;

        virtual void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
                                     TObjectPtr<LRenderPass> renderPass, 
                                     bool bExplicitClear = false) override;
        virtual void EndRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer) override;

        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, 
                                    TObjectPtr<LPipeline> Pipeline, 
                                    TObjectPtr<LMaterial> Material, 
                                    TObjectPtr<LVertexBuffer> InVertexBuffer, 
                                    TObjectPtr<LIndexBuffer> InIndexBuffer, 
                                    const glm::mat4& Transform, 
                                    const uint32_t IndexCount) override;

        virtual void Draw(LVertexBuffer& InVertexBuffer, const LShader& InShader) override;
        virtual void Draw(const LVertexBuffer& VertexBuffer, const LIndexBuffer& IndexBuffer, const LShader& Shader);

        virtual void SetPrimitiveTopology(const ERenderTopology InRenderTopology) override;
        virtual void SetDepthFunction(const EDepthFunction InDepthFunction) override;
		virtual void SetDepthEnabled(const bool Enabled) override;

        virtual FRendererCapabilities& GetCapabilities() override;

	#if 0
        virtual void BindArrayTexture(const uint8_t Index) override;
        virtual void BindArrayTexture(const EArrayTextureDimension Dimension) override;
	#else
        void BindArrayTexture(const uint8_t Index);
        void BindArrayTexture(const EArrayTextureDimension Dimension);
        TObjectPtr<LOpenGLArrayTexture> GetArrayTexture(const int Index);
	#endif

    private:
        TObjectPtr<LOpenGLArrayTexture> GetArrayTextureWithDimension(const EArrayTextureDimension Dimension);

    private:
        int TopologyGL = GL_TRIANGLES;
		static_assert(std::is_same_v<decltype(TopologyGL), decltype(GL_TRIANGLES)>);

		TObjectPtr<LOpenGLContext> RenderContext{};

        TObjectPtr<LOpenGLRenderPass> GeometricPass{};
		TObjectPtr<LOpenGLRenderPass> RenderPass2D{};

        LCLASS(LOpenGLRenderer)
    };

}
