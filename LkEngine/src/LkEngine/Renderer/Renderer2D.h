#pragma once

#include "LkEngine/Renderer/UniformBuffer.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/RenderCommandQueue.h"
#include "LkEngine/Renderer/Color.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Editor/EditorCamera.h"


namespace LkEngine {

    // Forward declaration
    class Entity;

    struct Renderer2DSpecification
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxLines = 1000;
        bool SwapChainTarget = false;
    };

    // TODO:
    // - GetTexture(index: int) / GetTexture(name: string)
    //
    class Renderer2D
    {
    public:
        Renderer2D(const Renderer2DSpecification& specification = Renderer2DSpecification());
        ~Renderer2D();

        void Init();
        void Shutdown();
        void BeginScene(const Camera& camera);
        void BeginScene(const Camera& camera, const glm::mat4& transform);
        void EndScene();
        void Flush();

        void DrawEntity(Entity& entity);

        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0); // Wrapper
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0); // Wrapper
        void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0);
        void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);

        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture2D, float rotation, uint64_t entityID = 0); // Wrapper
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0); // Wrapper

        void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0);
        void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0);

        void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void DrawRect(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0);

        void DrawSprite(const glm::mat4& transform, SpriteComponent& sc, uint64_t entityID);

        float GetLineWidth();
        void SetLineWidth(float width);

        int GetTextureSlotsCount() const { return m_TextureSlots.size(); }
        int GetBoundTextureSlotsCount() const;
        s_ptr<Texture> GetBoundTexture(int slot) const { return m_TextureSlots[slot]; }

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t QuadVertexArray_RendererID = 0;
            uint32_t QuadVertexBuffer_RendererID = 0;
            uint32_t LineCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };
        void ResetStats();
        Statistics GetStats();

    private:
        void StartBatch();
        void NextBatch();

    private:
        bool m_Initialized = false;
        RenderCommandQueue m_RenderQueue;
        Renderer2DSpecification m_Specification;

        struct LineVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            int EntityID; // For editor
        };
    
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float TilingFactor;
            int EntityID; // For editor 
        };

        static const uint32_t MaxTextureSlots = 32;

        const uint32_t m_MaxVertices;
        const uint32_t m_MaxIndices;

        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

        // Quad
        uint32_t m_QuadIndexCount = 0;
        glm::vec4 m_QuadVertexPositions[4];
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;
        s_ptr<Shader> m_QuadShader;
        s_ptr<VertexBuffer> m_QuadVertexBuffer;

        // Line
        uint32_t m_LineIndexCount = 0;
        s_ptr<Shader> m_LineShader;
        s_ptr<VertexBuffer> m_LineVertexBuffer;
        float m_LineWidth = 3.0f;

        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;

        uint32_t m_TextureSlotIndex = 1; // 0 = white texture
        s_ptr<Shader> m_TextureShader = nullptr;
        s_ptr<Texture2D> m_WhiteTexture = nullptr;
        std::array<s_ptr<Texture>, MaxTextureSlots> m_TextureSlots;

        Renderer2D::Statistics m_Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData m_CameraBuffer;
        s_ptr<UniformBuffer> m_CameraUniformBuffer;

        friend class EditorLayer;
    };

}