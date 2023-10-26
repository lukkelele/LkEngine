#pragma once

#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/UniformBuffer.h"


namespace LkEngine {

    struct Renderer2DSpecification
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxLines = 1000;
        bool SwapChainTarget = false;
    };

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
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
        void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID);
        void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);
        void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        void DrawSprite(const glm::mat4& transform, SpriteComponent& src, int entityID);

        float GetLineWidth();
        void SetLineWidth(float width);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };
        void ResetStats();
        Statistics GetStats();

    private:
        void StartBatch();
        void NextBatch();

    private:
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

        glm::vec4 m_QuadVertexPositions[4];
        s_ptr<VertexArray> m_QuadVertexArray;
        s_ptr<VertexBuffer> m_QuadVertexBuffer;
        s_ptr<Shader> m_QuadShader;

        s_ptr<VertexArray> m_LineVertexArray;
        s_ptr<VertexBuffer> m_LineVertexBuffer;
        s_ptr<Shader> m_LineShader;

        uint32_t m_QuadIndexCount = 0;
        QuadVertex* m_QuadVertexBufferBase = nullptr;
        QuadVertex* m_QuadVertexBufferPtr = nullptr;

        uint32_t m_LineVertexCount = 0;
        LineVertex* m_LineVertexBufferBase = nullptr;
        LineVertex* m_LineVertexBufferPtr = nullptr;

        float m_LineWidth = 2.0f;

        s_ptr<Texture2D> m_WhiteTexture = nullptr;
        std::array<s_ptr<Texture2D>, MaxTextureSlots> m_TextureSlots;
        uint32_t m_TextureSlotIndex = 1; // 0 = white texture

        Renderer2D::Statistics m_Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData m_CameraBuffer;
        s_ptr<UniformBuffer> m_CameraUniformBuffer;

    };

}