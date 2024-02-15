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

    class OpenGLRenderer : public RendererAPI
    {
    public:
        void Init();
        void Shutdown();

        void BeginFrame();
        void EndFrame();
        void Clear();

        void DrawIndexed(uint64_t indexCount) override;
        void Draw(VertexBuffer& vb, const Shader& shader);
        void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader);

        void SubmitImage(const Ref<Image> image) override;
        void SubmitImage(const Ref<Image2D> image) override;

        void SubmitMesh(Ref<Mesh>& mesh, Ref<Shader>& shader, const glm::mat4& transform);

        void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear = false) override;
        void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer) override;
        void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount) override;
        void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader>& shader, Ref<VertexBuffer>& vertexBuffer, Ref<IndexBuffer>& indexBuffer, const glm::mat4& transform, uint32_t indexCount) override;
        void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount) override;

		void SubmitIndexed(unsigned int count) override;

        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) override;
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) override;
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) override;

        void SubmitLine(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;

        void SetPrimitiveTopology(const RenderTopology& mode) override;
        void SetDepthFunction(const DepthFunction& depthFunc) override;

        RendererCapabilities& GetCapabilities() override;

        Ref<Renderer2DAPI> GetRenderer2D() override { return m_Renderer2D; }
        Ref<Renderer2DAPI> GetRenderer2DAPI() override { return m_Renderer2D; }

        void BindTextureArray(int idx);
        void BindTextureArray(const TextureArrayDimension& dimension);
        Ref<OpenGLTextureArray> GetTextureArray(int idx);

    private:
        Ref<OpenGLTextureArray> GetTextureArrayWithDimension(const TextureArrayDimension& dimension);

    private:
        uint8_t m_Topology = GL_TRIANGLES;

        Ref<OpenGLRenderer2D> m_Renderer2D;
        Ref<OpenGLContext> m_RenderContext;

        Ref<OpenGLRenderPass> m_GeometricPass;
        Ref<OpenGLRenderPass> m_RenderPass2D;
    };

    namespace GLUtils {

        inline static int PrimitiveTopologyToOpenGL(const PrimitiveTopology& topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Triangles:  return GL_TRIANGLES;
                case PrimitiveTopology::Lines:      return GL_LINES;
                case PrimitiveTopology::Points:     return GL_POINTS;
            }
            LK_CORE_ASSERT(false, "Unknown topology!");
        }

    }

}