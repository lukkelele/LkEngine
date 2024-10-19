#pragma once

#include "LkEngine/Renderer/Renderer2DAPI.h"

#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureArray.h"


namespace LkEngine {

    class LEntity;

    struct OpenGLRenderer2DSpecification : public Renderer2DSpecification
    {
        uint8_t TextureArraysUsed = 1;
    };

	class LOpenGLRenderer2D : public IRenderer2DAPI, public LObject
	{
    public:
        LOpenGLRenderer2D(const OpenGLRenderer2DSpecification& InSpecification = OpenGLRenderer2DSpecification());
        ~LOpenGLRenderer2D() = default;

        virtual void Init() override;
        virtual void Shutdown() override;

        virtual void BeginScene(const LSceneCamera& camera) override;
        virtual void BeginScene(const LSceneCamera& camera, const glm::mat4& transform) override;
        virtual void BeginScene(const glm::mat4& viewProjectionMatrix) override;
        virtual void EndScene() override;
        virtual void Flush() override;

        virtual void DrawImage(const TObjectPtr<LImage>& Image) override;

        // DrawQuad
        virtual void DrawQuad(const glm::mat4& transform, 
                              const glm::vec4& color, 
                              const uint64_t entityID = 0) override;

        virtual void DrawQuad(const glm::vec2& pos,
                              const glm::vec2& size, 
                              const glm::vec4& color, 
                              const uint64_t entityID = 0) override;

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              const glm::vec4& color, 
                              const uint64_t entityID = 0) override;

        virtual void DrawQuad(const glm::vec2& pos, 
                              const glm::vec2& size, 
                              const glm::vec4& color, 
                              const float rotation, 
                              uint64_t entityID = 0) override;

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              const glm::vec4& color, 
                              const float rotation, 
                              const uint64_t entityID = 0) override; 

        virtual void DrawQuad(const glm::vec2& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture2D, 
                              const float rotation, 
                              const uint64_t entityID = 0) override; 

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture, 
                              const float rotation, 
                              const uint64_t entityID = 0) override; 

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture, 
                              const glm::vec4& tintColor, 
                              float rotation, 
                              uint64_t entityID = 0) override; 
        // ~DrawQuad

        /// FIXME
        // DrawRotatedQuad
        virtual void DrawRotatedQuad(const glm::vec2& pos, 
                                     const glm::vec2& size, 
                                     const glm::vec4& color, 
                                     const float rotation, 
                                     uint64_t entityID = 0) override;

        virtual void DrawRotatedQuad(const glm::vec3& pos, 
                                     const glm::vec2& size, 
                                     const glm::vec4& color, 
                                     const float rotation, 
                                     uint64_t entityID = 0) override;
        // ~DrawRotatedQuad

        // DrawLine
        virtual void DrawLine(const glm::vec2& p0, 
                              const glm::vec2& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) override;

        virtual void DrawLine(const glm::vec3& p0, 
                              const glm::vec3& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) override;
        // ~DrawLine

        TObjectPtr<LShader> GetQuadShader() override
        { 
            return m_QuadShader; 
        }

        TObjectPtr<LShader> GetLineShader() override 
        { 
            return m_LineShader; 
        }

        const Renderer2DSpecification& GetSpecification() const override { return m_Specification; }

        /// FIXME
        virtual float GetLineWidth() override;
        virtual void SetLineWidth(float width) override;

        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture) override;
        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture, int slot) override;

        void ResetStats() override;
        Statistics GetStats() override;

    private:
        virtual void StartBatch() override;
        virtual void NextBatch() override;

        virtual void AddQuadBuffer() override;
        virtual void AddLineBuffer() override;

        virtual QuadVertex*& GetWriteableQuadBuffer() override;
        virtual LineVertex*& GetWriteableLineBuffer() override;

        void AddTextureArray(const TObjectPtr<OpenGLTextureArray>& textureArray);

    public:
        static constexpr int MaxTextureSlots = 32;
        static constexpr int MaxTextureArrays = 10;
    private:
        int m_Topology;

        OpenGLRenderer2DSpecification m_Specification;
        RenderCommandQueue m_RenderQueue;

        const uint32_t m_MaxVertices = 0;
        const uint32_t m_MaxIndices = 0;
        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

		TObjectPtr<LRenderCommandBuffer> m_RenderCommandBuffer;

        /* Quad. */
        uint32_t m_QuadIndexCount = 0;
		TObjectPtr<LRenderPass> m_QuadPass;
		TObjectPtr<LMaterial> m_QuadMaterial;
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;
        TObjectPtr<LShader> m_QuadShader;
		TObjectPtr<LIndexBuffer> m_QuadIndexBuffer;
        TObjectPtr<LVertexBuffer> m_QuadVertexBuffer;
        glm::vec4 m_QuadVertexPositions[4] = {};

        /* Line.*/
        float m_LineWidth = 3.0f;
        uint32_t m_LineIndexCount = 0;
        TObjectPtr<LVertexBuffer> m_LineVertexBuffer;

        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;
        TObjectPtr<LIndexBuffer> m_LineIndexBuffer = nullptr;
        TObjectPtr<LShader> m_LineShader = nullptr;
		TObjectPtr<LMaterial> m_LineMaterial;

        TObjectPtr<LShader> m_TextureShader = nullptr;
        TObjectPtr<LTexture2D> m_WhiteTexture = nullptr;

        /* Texture Slots. */
        std::array<TObjectPtr<LTexture2D>, MaxTextureSlots> m_TextureSlots;

        /* Texture Arrays. */
        std::array<TObjectPtr<OpenGLTextureArray>, MaxTextureArrays> m_TextureArrays;

        struct CameraData
        {
            glm::mat4 ViewProjection{};
        };
        CameraData m_CameraBuffer{};

        TObjectPtr<LOpenGLUniformBuffer> m_CameraUniformBuffer;

        Statistics m_Stats;

        friend class LEditorLayer;
        friend class LOpenGLRenderer;
        friend class OpenGLRenderer; /// REMOVE

        LCLASS(LOpenGLRenderer2D)
	};


}