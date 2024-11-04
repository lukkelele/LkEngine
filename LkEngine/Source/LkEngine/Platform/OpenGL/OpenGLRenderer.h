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

    class OpenGLRenderer : public LRendererAPI
    {
    public:
        OpenGLRenderer();

        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        virtual void Clear() override;

        virtual void DrawIndexed(const uint64_t IndexCount) override;

        void Draw(LVertexBuffer& vb, const LShader& shader);

        virtual void Draw(const LVertexBuffer& vb,
                          const LIndexBuffer& ib,
                          const LShader& shader); // override;

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
                                    TObjectPtr<LShader>& shader, 
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

        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;

        void SubmitQuad(const glm::vec2& pos, 
                        const glm::vec2& size, 
                        TObjectPtr<LTexture> texture, 
                        const float rotation, 
                        uint64_t entityID = 0) override;

        void SubmitQuad(const glm::vec3& pos, 
                        const glm::vec2& size, 
                        TObjectPtr<LTexture> texture, 
                        const float rotation, 
                        uint64_t entityID = 0) override;

        void SubmitQuad(const glm::vec2& pos, 
                        const glm::vec2& size, 
                        TObjectPtr<LTexture> texture, 
                        const glm::vec4& tintColor, 
                        const float rotation, 
                        uint64_t entityID) override;

        void SubmitQuad(const glm::vec3& pos, 
                        const glm::vec2& size, 
                        TObjectPtr<LTexture> texture, 
                        const glm::vec4& tintColor, 
                        const float rotation, 
                        uint64_t entityID) override;

        void SubmitLine(const glm::vec2& pos, 
                        const glm::vec2& size, 
                        const glm::vec4& color, 
                        uint64_t entityID = 0) override;

        void SetPrimitiveTopology(const ERenderTopology& InRenderTopology) override;
        void SetDepthFunction(const EDepthFunction& InDepthFunction) override;

        RendererCapabilities& GetCapabilities() override;

        void BindTextureArray(int idx);
        void BindTextureArray(const ETextureArrayDimension& InTextureArrayDimension);

        TObjectPtr<OpenGLTextureArray> GetTextureArray(const int Index);

    private:
        TObjectPtr<OpenGLTextureArray> GetTextureArrayWithDimension(const ETextureArrayDimension& TextureArrayDimension);

    private:
        uint8_t m_Topology = GL_TRIANGLES;

        TObjectPtr<LOpenGLRenderer2D> Renderer2D;
        TObjectPtr<LOpenGLContext> RenderContext;

        TObjectPtr<OpenGLRenderPass> m_GeometricPass;
        TObjectPtr<OpenGLRenderPass> m_RenderPass2D;

        LCLASS(OpenGLRenderer)
    };

    namespace LOpenGL {

        inline static int PrimitiveTopologyToOpenGL(const PrimitiveTopology& topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Triangles:  return GL_TRIANGLES;
                case PrimitiveTopology::Lines:      return GL_LINES;
                case PrimitiveTopology::Points:     return GL_POINTS;
            }
            LK_CORE_ASSERT(false, "Unknown topology!");
            return -1;
        }

    }

}