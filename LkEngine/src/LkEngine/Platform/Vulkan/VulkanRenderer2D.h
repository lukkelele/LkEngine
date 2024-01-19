#pragma once

#include "LkEngine/Renderer/Renderer2DAPI.h"

#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanMaterial.h"


namespace LkEngine {

    class Entity;

	class VulkanRenderer2D : public Renderer2DAPI
	{
    public:
        VulkanRenderer2D(const Renderer2DSpecification& specification = Renderer2DSpecification());
        ~VulkanRenderer2D() override;

        void Init() override;
        void Shutdown() override;
        void BeginScene(const Camera& camera) override;
        void BeginScene(const Camera& camera, const glm::mat4& transform) override;
        void EndScene() override;
        void Flush() override;

        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture2D, float rotation, uint64_t entityID = 0) override; 
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) override; 
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID = 0) override; 
        void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) override;
        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0) override;
        void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0) override;

        Ref<Shader> GetQuadShader() override { return m_QuadShader; }
        Ref<Shader> GetLineShader() override { return m_LineShader; }

        float GetLineWidth() override;
        void SetLineWidth(float width) override;

        void AddTextureToSlot(Ref<Texture> texture) override;
        void AddTextureToSlot(Ref<Texture> texture, int slot) override;

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
        static const uint32_t MaxTextureSlots = 32;
    private:
        Renderer2DSpecification m_Specification;
        RenderCommandQueue m_RenderQueue;

		Ref<RenderCommandBuffer> m_RenderCommandBuffer;

        // Quad
        const uint32_t m_MaxQuadVertices;
        const uint32_t m_MaxQuadIndices;

        uint32_t m_QuadIndexCount = 0;
        glm::vec4 m_QuadVertexPositions[4];
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;
		Ref<RenderPass> m_QuadPass;
        Ref<VertexBuffer> m_QuadVertexBuffer;
		Ref<IndexBuffer> m_QuadIndexBuffer;
        Ref<Shader> m_QuadShader;
		Ref<Material> m_QuadMaterial;


        // Line
        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

        float m_LineWidth = 3.0f;
        uint32_t m_LineIndexCount = 0;
        Ref<VertexBuffer> m_LineVertexBuffer;
		Ref<RenderPass> m_LinePass;
		Ref<RenderPass> m_LineOnTopPass;
        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;
        Ref<IndexBuffer> m_LineIndexBuffer = nullptr;
        Ref<Shader> m_LineShader = nullptr;
		Ref<Material> m_LineMaterial;

        Ref<Texture> m_WhiteTexture = nullptr;
        uint32_t m_TextureSlotIndex = 1; 
        Ref<Shader> m_TextureShader = nullptr;
        std::array<Ref<Texture>, MaxTextureSlots> m_TextureSlots; // FIXME

        Ref<Framebuffer> m_TargetFramebuffer = nullptr;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData m_CameraBuffer;
        Ref<UniformBuffer> m_CameraUniformBuffer;

        Statistics m_Stats;

        friend class Renderer2D;
        friend class Editor;
	};

}