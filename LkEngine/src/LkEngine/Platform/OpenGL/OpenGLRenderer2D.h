#pragma once

#include "LkEngine/Renderer/Renderer2DAPI.h"

#include "OpenGLFramebuffer.h"

#include <stb_image/stb_image.h>


namespace LkEngine {

    class Entity;

	class OpenGLRenderer2D : public Renderer2DAPI
	{
    public:
        OpenGLRenderer2D(const Renderer2DSpecification& specification = Renderer2DSpecification());
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

        //Ref<OpenGLFramebuffer> GetFramebuffer() { return m_QuadFramebuffer; }

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

    public:
        static constexpr uint32_t MaxTextureSlots = 32;
    private:
        Renderer2DSpecification m_Specification;
        RenderCommandQueue m_RenderQueue;

        RendererID m_TextureArray;

        const uint32_t m_MaxVertices;
        const uint32_t m_MaxIndices;

        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

		Ref<RenderCommandBuffer> m_RenderCommandBuffer;

        // Quad
        uint32_t m_QuadIndexCount = 0;
		Ref<RenderPass> m_QuadPass;
		Ref<Material> m_QuadMaterial;
        Ref<Texture2D> m_QuadTextureArray = nullptr;
        Ref<Texture2D> m_LineTextureArray = nullptr;
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;
        Ref<Shader> m_QuadShader;
		Ref<IndexBuffer> m_QuadIndexBuffer;
        Ref<VertexBuffer> m_QuadVertexBuffer;
        glm::vec4 m_QuadVertexPositions[4];
        //Ref<Framebuffer> m_QuadFramebuffer;

        // Line
        float m_LineWidth = 3.0f;
        uint32_t m_LineIndexCount = 0;
        Ref<VertexBuffer> m_LineVertexBuffer;

        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;
        Ref<IndexBuffer> m_LineIndexBuffer = nullptr;
        Ref<Shader> m_LineShader = nullptr;
		Ref<Material> m_LineMaterial;

        uint32_t m_TextureSlotIndex = 1; // 0 --> White Texture
        Ref<Shader> m_TextureShader = nullptr;
        Ref<Texture2D> m_WhiteTexture = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> m_TextureSlots;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        } m_CameraBuffer;

        Ref<UniformBuffer> m_CameraUniformBuffer;

        Statistics m_Stats;

        friend class EditorLayer;
        friend class OpenGLRenderer;
	};


}