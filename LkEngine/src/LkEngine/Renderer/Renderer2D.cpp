#include "LKpch.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/RenderCommand.h"


namespace LkEngine {

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor-only
        int EntityID;
    };

    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;

        int EntityID; /* For editor */
    };

    struct Renderer2DData
    {
        static const uint32_t MaxQuads = 20000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32;

        s_ptr<VertexArray> QuadVertexArray;
        s_ptr<VertexBuffer> QuadVertexBuffer;
        s_ptr<Shader> QuadShader;
        glm::vec4 QuadVertexPositions[4];

        s_ptr<VertexArray> LineVertexArray;
        s_ptr<VertexBuffer> LineVertexBuffer;
        s_ptr<Shader> LineShader;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        uint32_t LineVertexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;

        float LineWidth = 2.0f;

        std::array<s_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        Renderer2D::Statistics Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        s_ptr<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }
        s_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        s_Data.LineVertexArray = VertexArray::Create();
        s_Data.LineVertexBuffer = VertexBuffer::Create((const void*)s_Data.MaxVertices, s_Data.MaxVertices);
        VertexBufferLayout lineVertexBufLayout{ };
        s_Data.LineVertexBuffer->SetLayout({
            {  "a_Position", ShaderDataType::Float3 ,  },
            {  "a_Color",    ShaderDataType::Float4 ,  },
            {  "a_EntityID", ShaderDataType::Int ,     }
            });
        s_Data.LineVertexArray->AddVertexBuffer(*s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::StartBatch()
    {
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.LineVertexCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
    }

    void Renderer2D::NextBatch()
    {
    }

    void Renderer2D::Flush()
    {
        {
            if (s_Data.QuadIndexCount)
            {
                uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
                s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

                // Bind textures
                for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
                    s_Data.TextureSlots[i]->Bind(i);

                s_Data.QuadShader->Bind();
                RenderCommand::DrawIndexed(*s_Data.QuadVertexArray, s_Data.QuadIndexCount);
                s_Data.Stats.DrawCalls++;
            }

            if (s_Data.LineVertexCount)
            {
                uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
                s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

                s_Data.LineShader->Bind();
                RenderCommand::SetLineWidth(s_Data.LineWidth);
                RenderCommand::DrawLines(*s_Data.LineVertexArray, s_Data.LineVertexCount);
                s_Data.Stats.DrawCalls++;
            }
        }
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
    {

    }

    void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& src, int entityID)
    {
    }

    float Renderer2D::GetLineWidth()
    {
        return 0.0f;
    }

    void Renderer2D::SetLineWidth(float width)
    {
    }

    void Renderer2D::ResetStats()
    {
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return Statistics();
    }

}