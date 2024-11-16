#pragma once

#include "LkEngine/Renderer/RendererAPI.h"

#include "OpenGLRenderer2D.h"

#include "OpenGLPipeline.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLContext.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLRenderPass.h"
#include "OpenGLRenderCommandBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureArray.h"
#include "OpenGLImGuiLayer.h"


namespace LkEngine {

    class LOpenGLRenderer : public LRendererAPI
    {
    public:
        LOpenGLRenderer();
		~LOpenGLRenderer() = default;

        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void Clear() override;

        virtual void DrawIndexed(const uint64_t IndexCount) override;

        void Draw(LVertexBuffer& VertexBuffer, const LShader& Shader);

        virtual void Draw(const LVertexBuffer& VertexBuffer, const LIndexBuffer& IndexBuffer, const LShader& Shader); // override;

        virtual void SubmitImage(const TObjectPtr<LImage> image) override;
        virtual void SubmitImage(const TObjectPtr<LImage2D> image) override;

        virtual void SubmitMesh(TObjectPtr<LMesh>& InMesh, 
                                TObjectPtr<LShader>& InShader, 
                                const glm::mat4& InTransform) override;

        virtual void BeginRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
                                     TObjectPtr<LRenderPass> renderPass, 
                                     bool bExplicitClear = false) override;

        virtual void EndRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer) override;

        // RenderGeometry
        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
                                    TObjectPtr<LPipeline> pipeline, 
                                    TObjectPtr<LVertexBuffer> vertexBuffer, 
                                    TObjectPtr<LIndexBuffer> indexBuffer, 
                                    const glm::mat4& transform, 
                                    uint32_t indexCount) override;

        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
                                    TObjectPtr<LPipeline> pipeline, 
                                    TObjectPtr<LShader>& Shader, 
                                    TObjectPtr<LVertexBuffer>& vertexBuffer, 
                                    TObjectPtr<LIndexBuffer>& indexBuffer, 
                                    const glm::mat4& transform, 
                                    const uint32_t indexCount) override;

        virtual void RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
                                    TObjectPtr<LPipeline> pipeline, 
                                    TObjectPtr<LMaterial> material, 
                                    TObjectPtr<LVertexBuffer> vertexBuffer, 
                                    TObjectPtr<LIndexBuffer> indexBuffer, 
                                    const glm::mat4& transform, 
                                    const uint32_t indexCount) override;
        // ~RenderGeometry

		void SubmitIndexed(unsigned int count) override;

        void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& color, uint64_t EntityID = 0) override;
        void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& color, uint64_t EntityID = 0) override;
        void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& color, float Rotation, uint64_t EntityID = 0) override;
        void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& color, float Rotation, uint64_t EntityID = 0) override;

        void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, 
                        const float Rotation, uint64_t EntityID = 0) override;

        void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, 
                        const float Rotation, uint64_t EntityID = 0) override;

        void SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, 
                        const glm::vec4& TintColor, const float Rotation, uint64_t EntityID) override;

        void SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture, 
                        const glm::vec4& TintColor, const float Rotation, uint64_t EntityID) override;

        void SubmitLine(const glm::vec2& Pos, const glm::vec2& Size, 
                        const glm::vec4& color, uint64_t EntityID = 0) override;

        virtual void SetPrimitiveTopology(const ERenderTopology InRenderTopology) override;
        virtual void SetDepthFunction(const EDepthFunction InDepthFunction) override;

        RendererCapabilities& GetCapabilities() override;

        virtual void BindTextureArray(const uint8_t Index) override;
        virtual void BindTextureArray(const ETextureArrayDimension& InTextureArrayDim) override;

        TObjectPtr<LOpenGLTextureArray> GetTextureArray(const int Index);

    private:
        TObjectPtr<LOpenGLTextureArray> GetTextureArrayWithDimension(const ETextureArrayDimension TextureArrayDimension);

    private:
		static_assert(GL_TRIANGLES == sizeof(uint32_t));
        uint32_t m_Topology = GL_TRIANGLES;

        TObjectPtr<LOpenGLRenderer2D> Renderer2D{};
		TObjectPtr<LOpenGLContext> RenderContext{};

        TObjectPtr<LOpenGLRenderPass> GeometricPass{};
		TObjectPtr<LOpenGLRenderPass> RenderPass2D{};

        LCLASS(LOpenGLRenderer)
    };

}
