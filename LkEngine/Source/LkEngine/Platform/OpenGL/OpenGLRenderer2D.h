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

	class OpenGLRenderer2D : public Renderer2DAPI
	{
    public:
        OpenGLRenderer2D(const OpenGLRenderer2DSpecification& specification = OpenGLRenderer2DSpecification());
        ~OpenGLRenderer2D();

        void Init() override;
        void Shutdown() override;
        void BeginScene(const SceneCamera& camera) override;
        void BeginScene(const SceneCamera& camera, const glm::mat4& transform) override;
        void BeginScene(const glm::mat4& viewProjectionMatrix) override;
        void EndScene() override;
        void Flush() override;

        void DrawImage(const Ref<Image> image) override;

        void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override; 
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture2D, float rotation, uint64_t entityID = 0) override; 
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, float rotation, uint64_t entityID = 0) override; 
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID = 0) override; 

        void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;

        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0) override;

        Ref<Shader> GetQuadShader() override { return m_QuadShader; }
        Ref<Shader> GetLineShader() override { return m_LineShader; }

        const Renderer2DSpecification& GetSpecification() const override { return m_Specification; }

        float GetLineWidth() override;
        void SetLineWidth(float width) override;

        void AddTextureToSlot(Ref<Texture2D> texture) override;
        void AddTextureToSlot(Ref<Texture2D> texture, int slot) override;

        void ResetStats() override;
        Statistics GetStats() override;

    private:
        void StartBatch() override;
        void NextBatch() override;

        void AddQuadBuffer() override;
        void AddLineBuffer() override;
        QuadVertex*& GetWriteableQuadBuffer() override;
        LineVertex*& GetWriteableLineBuffer() override;

        void AddTextureArray(const Ref<OpenGLTextureArray>& textureArray);

    public:
        static constexpr int MaxTextureSlots = 32;
        static constexpr int MaxTextureArrays = 10;
    private:
        int m_Topology;

        OpenGLRenderer2DSpecification m_Specification;
        RenderCommandQueue m_RenderQueue;

        const uint32_t m_MaxVertices;
        const uint32_t m_MaxIndices;

        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

		Ref<RenderCommandBuffer> m_RenderCommandBuffer;

        // Quad
        uint32_t m_QuadIndexCount = 0;
		Ref<RenderPass> m_QuadPass;
		Ref<Material> m_QuadMaterial;
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;
        Ref<Shader> m_QuadShader;
		Ref<IndexBuffer> m_QuadIndexBuffer;
        Ref<VertexBuffer> m_QuadVertexBuffer;
        glm::vec4 m_QuadVertexPositions[4] = {};

        // Line
        float m_LineWidth = 3.0f;
        uint32_t m_LineIndexCount = 0;
        Ref<VertexBuffer> m_LineVertexBuffer;

        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;
        Ref<IndexBuffer> m_LineIndexBuffer = nullptr;
        Ref<Shader> m_LineShader = nullptr;
		Ref<Material> m_LineMaterial;

        Ref<Shader> m_TextureShader = nullptr;
        Ref<Texture2D> m_WhiteTexture = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> m_TextureSlots;
        std::array<Ref<OpenGLTextureArray>, MaxTextureArrays> m_TextureArrays;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        } m_CameraBuffer{};

        Ref<OpenGLUniformBuffer> m_CameraUniformBuffer;

        Statistics m_Stats;

        friend class EditorLayer;
        friend class OpenGLRenderer;
	};


}