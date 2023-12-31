#include "LKpch.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    // Basic UV coordinates
    constexpr glm::vec2 TextureCoords[] = { 
        { 0.0f, 0.0f }, // bottom left 
        { 0.0f, 1.0f }, // top left
        { 1.0f, 1.0f }, // top right
        { 1.0f, 0.0f }  // bottom right
    };

    Renderer2D::Renderer2D(const Renderer2DSpecification& specification)
        : m_Specification(specification)
        , m_MaxVertices(specification.MaxQuads * 4)
        , m_MaxIndices(specification.MaxQuads * 6)
        , m_MaxLineVertices(specification.MaxLines * 2)
        , m_MaxLineIndices(specification.MaxLines * 6)
    {
        m_CameraBuffer = {};
        m_CameraUniformBuffer = {};
    }

    Renderer2D::~Renderer2D()
    {
    }

    void Renderer2D::Init()
    {
        if (m_Initialized == true)
            throw std::runtime_error("Renderer2D already initialized");
        LOG_DEBUG("Initializing 2D Renderer");

        // Quad
        m_QuadVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(QuadVertex));
        m_QuadVertexBuffer->SetLayout({
            { "a_Pos",          ShaderDataType::Float3  },
            { "a_Color",        ShaderDataType::Float4  },
            { "a_TexCoord",     ShaderDataType::Float2  },
            { "a_TexIndex",     ShaderDataType::Int,    },
            { "a_TilingFactor", ShaderDataType::Float,  },
            { "a_EntityID",     ShaderDataType::Int     },
        });
        m_QuadVertexBufferBase = new QuadVertex[m_MaxVertices];

        uint32_t* quadIndices = new uint32_t[m_MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < m_MaxIndices; i += 6)
        {
            // First triangle, 0->1->2
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            // Second triangle, 2->3->0
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        s_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_MaxIndices);
        m_QuadVertexBuffer->SetIndexBuffer(quadIB);

        m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
        m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

        delete[] quadIndices;

        // Line
        m_LineVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(LineVertex));
        VertexBufferLayout lineVertexBufLayout{ };
        m_LineVertexBuffer->SetLayout({
            { "a_Pos",       ShaderDataType::Float3, },
            { "a_Color",     ShaderDataType::Float4, },
            { "a_EntityID",  ShaderDataType::Int,  }
        });
        m_LineVertexBufferBase = new LineVertex[m_MaxVertices];

        auto* textureLibrary = TextureLibrary::Get();
        m_WhiteTexture = textureLibrary->GetWhiteTexture();
        m_TextureSlots[0] = m_WhiteTexture;

        auto textures2D = textureLibrary->GetTextures2D();
        // First texture is the white texture in the texture library also, so begin iterating at i=1
        for (int i = 1; i < textures2D.size(); i++)
        {
            auto& textureEntry = textures2D[i];
            m_TextureSlots[i] = textureEntry.second;
        }

        // Shader setup
        m_QuadShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");
        m_LineShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");

        m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);

		uint32_t* lineIndices = new uint32_t[m_MaxLineIndices];
        for (uint32_t i = 0; i < m_MaxLineIndices; i++)
			lineIndices[i] = i;
		delete[] lineIndices;
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const Camera& camera)
    {
        m_CameraBuffer.ViewProjection = camera.GetViewProjection();
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind();
        }

        StartBatch();
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind();
        }

        StartBatch();
    }

    void Renderer2D::EndScene()
    {
        Flush();
    }

    void Renderer2D::StartBatch()
    {
        m_QuadIndexCount = 0;
        m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

        m_LineIndexCount = 0;
        m_LineVertexBufferPtr = m_LineVertexBufferBase;
    }

    void Renderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void Renderer2D::Flush()
    {
        if (m_QuadIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

            m_QuadShader->Bind();
            m_QuadShader->SetUniformMat4f("u_ViewProj", Scene::GetActiveScene()->GetCamera()->GetViewProjection());

            // Bind textures
            for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
            {
                if (m_TextureSlots[i])
                {
                    m_TextureSlots[i]->Bind(i);
                    m_QuadShader->SetUniform1i("u_Textures[" + std::to_string(i) + "]", i);
                }
            }

            Renderer::SubmitIndexed(*m_QuadVertexBuffer, m_QuadIndexCount);
            //RenderCommand::DrawIndexed(*m_QuadVertexBuffer, m_QuadIndexCount);

            m_QuadShader->Unbind();

            m_Stats.DrawCalls++;
        }


        if (m_LineIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_LineVertexBufferPtr - (uint8_t*)m_LineVertexBufferBase);
            m_LineVertexBuffer->SetData(m_LineVertexBufferBase, dataSize);

            // TODO: Texture binding here, same as with quads
            m_LineShader->Bind();
            m_LineShader->SetUniformMat4f("u_ViewProj", Scene::GetActiveScene()->GetCamera()->GetViewProjection());

            auto lineElements = m_LineVertexBuffer->GetLayout().GetElements();

            for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
            {
                if (m_TextureSlots[i])
                {
                    m_TextureSlots[i]->Bind(i);
                    m_LineShader->SetUniform4f("a_Color", m_LineVertexBufferPtr->Color);
                }
            }

            //RenderCommand::SetLineWidth(m_LineWidth);
            RenderCommand::DrawLines(*m_LineVertexBuffer, m_LineIndexCount);

            m_LineShader->Unbind();

            m_Stats.DrawCalls++;
        }

        m_TextureSlotIndex = 0;
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {                                                                                                     
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation);                                   
    } 

    void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
        //DrawQuad(pos, size, m_WhiteTexture, color, rotation, entityID);
    }
                                                                                                          
    // Draw without texture - use color
    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
#if 0
        float textureIndex = 0; 
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

        if (m_QuadIndexCount >= m_MaxIndices)
        {
            NextBatch();
        }

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
            m_QuadVertexBufferPtr->TexCoord = TextureCoords[i];
            m_QuadVertexBufferPtr->Color = color;
            m_QuadVertexBufferPtr->TexIndex = textureIndex;
            m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
            m_QuadVertexBufferPtr->EntityID = entityID;
            m_QuadVertexBufferPtr++;
        }
        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;
#endif
        constexpr float textureIndex = 0;
        constexpr float tilingFactor = 1.0f;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[0];
        m_QuadVertexBufferPtr->Color = color;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[0];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[1];
        m_QuadVertexBufferPtr->Color = color;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[1];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[2];
        m_QuadVertexBufferPtr->Color = color;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[2];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[3];
        m_QuadVertexBufferPtr->Color = color;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[3];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;

    }

    void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID)
    {
        DrawLine({ p0.x, p0.y, 0.0f }, { p1.x, p1.y, 0.0f }, color, entityID);
    }

    void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID)
    {
        m_LineVertexBufferPtr->Position = p0;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;

        m_LineVertexBufferPtr->Position = p1;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;

        m_LineIndexCount += 2;

        m_Stats.LineCount++;
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture2D, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, texture2D, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID)
    {
    #if 0
        if (texture == nullptr)
            throw std::runtime_error("Passed texture to DrawQuad was NULLPTR");
        float textureIndex = 0.0f;
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;
        glm::vec4 tintColor = texture->GetTintColor();
    #endif
        //DrawQuad(pos, size, texture, texture->GetTintColor(), rotation, entityID);
        DrawQuad(pos, size, texture, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
    {
        if (texture == nullptr)
            throw std::runtime_error("Passed texture to DrawQuad was NULLPTR");

        float textureIndex = 0.0f;
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i] != nullptr && m_TextureSlots[i]->GetName() == texture->GetName())
            {
                textureIndex = (float)i;
                break;
            }
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, 0.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[0];
        m_QuadVertexBufferPtr->Color = tintColor;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[0];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[1];
        m_QuadVertexBufferPtr->Color = tintColor;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[1];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[2];
        m_QuadVertexBufferPtr->Color = tintColor;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[2];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[3];
        m_QuadVertexBufferPtr->Color = tintColor;
        m_QuadVertexBufferPtr->TexCoord = TextureCoords[3];
        m_QuadVertexBufferPtr->TexIndex = textureIndex;
        m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
        m_QuadVertexBufferPtr++;

        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
    }

    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& src, uint64_t entityID)
    {
    }

    void Renderer2D::DrawEntity(Entity& entity)
    {
	    auto& tc = entity.GetComponent<TransformComponent>();
	    auto& sc = entity.GetComponent<SpriteComponent>();

        DrawQuad(tc.GetTransform(), sc.Color, entity.UUID());
    }

    float Renderer2D::GetLineWidth()
    {
        return m_LineWidth;
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

    int Renderer2D::GetBoundTextureSlotsCount() const
    {
        int count = 0;
        for (const auto& tex : m_TextureSlots)
        {
            if (tex != nullptr)
                count++;
        }
        return count;
    }

}