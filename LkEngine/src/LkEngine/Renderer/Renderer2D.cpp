#include "LKpch.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RenderCommand.h"


namespace LkEngine {

    Renderer2D::Renderer2D(const Renderer2DSpecification& specification)
        : m_Specification(specification)
        , m_MaxVertices(specification.MaxQuads * 4)
        , m_MaxIndices(specification.MaxQuads * 6)
        , m_MaxLineVertices(specification.MaxLines * 2)
        , m_MaxLineIndices(specification.MaxLines * 6)
    {
        Init();
    }

    Renderer2D::~Renderer2D()
    {
    }

    void Renderer2D::Init()
    {
        m_QuadVertexArray = VertexArray::Create();

        m_QuadVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(QuadVertex));
        m_QuadVertexBuffer->SetLayout({
            { "a_Position",     ShaderDataType::Float3  },
            { "a_Color",        ShaderDataType::Float4, },
            //{ "a_TexCoord",     ShaderDataType::Float2, },
            //{ "a_TexIndex",     ShaderDataType::Float,  },
            //{ "a_TilingFactor", ShaderDataType::Float,  },
        });
        m_QuadVertexArray->AddVertexBuffer(*m_QuadVertexBuffer);
        m_QuadVertexBufferBase = new QuadVertex[m_MaxVertices];

        uint32_t* quadIndices = new uint32_t[m_MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < m_MaxIndices; i += 6)
        {
            // First triangle
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            // Second triangle
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }
        s_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_MaxIndices);
        m_QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // Line
        m_LineVertexArray = VertexArray::Create();
        m_LineVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(LineVertex));
        VertexBufferLayout lineVertexBufLayout{ };
        m_LineVertexBuffer->SetLayout({
            {  "a_Position", ShaderDataType::Float3,  },
            {  "a_Color",    ShaderDataType::Float4,  },
            {  "a_EntityID", ShaderDataType::Int,     }
            });
        m_LineVertexArray->AddVertexBuffer(*m_LineVertexBuffer);
        m_LineVertexBufferBase = new LineVertex[m_MaxVertices];

        m_WhiteTexture = std::make_shared<Texture2D>("assets/img/atte_square.png");
        m_TextureSlots[0] = m_WhiteTexture;

        m_QuadShader = Shader::Create("assets/shaders/Renderer2D_BasicQuad.shader");
        //m_QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.shader");
        m_LineShader = Shader::Create("assets/shaders/basic_test.shader");

        m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

        m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const Camera& camera)
    {
        m_CameraBuffer.ViewProjection = camera.GetViewProjection();
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        m_TextureSlotIndex = 1;
        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            m_TextureSlots[i] = nullptr;
        }

        LOG_WARN("BEGIN SCENE");
        StartBatch();
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));
        StartBatch();
        LOG_WARN("BEGIN SCENE");
    }

    void Renderer2D::EndScene()
    {
        Flush();
        LOG_DEBUG("END SCENE");
    }

    void Renderer2D::StartBatch()
    {
        m_QuadIndexCount = 0;
        m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

        m_LineVertexCount = 0;
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
            LOG_TRACE("FLUSH");
            //m_QuadVertexArray->Bind();
            m_QuadShader->Bind();

            uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            LOG_INFO("[Flush] Datasize: {}", dataSize);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

            // Bind textures
            //for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
            //{
            //    s_ptr<Texture2D> tex2D = m_TextureSlots[i];
            //    if (tex2D)
            //    {
            //        //LOG_DEBUG("Texture: {}", tex2D->GetPath());
            //        tex2D->Bind(i);
            //    }
            //}

            m_QuadShader->SetUniformMat4f("u_ViewProj", Scene::ActiveScene->GetActiveCamera()->GetViewProjection());
            r -= inc;
            if (r < 0 || r > 1.0f)
                inc *= -1;
            m_QuadShader->SetUniform4f("u_Color", r, 1.0f, 0.2f, 1.0f);

            //LOG_TRACE("QuadIndexCount: {}", m_QuadIndexCount);
            //m_QuadVertexBuffer->Bind();
            RenderCommand::DrawIndexed(*m_QuadVertexArray, m_QuadIndexCount);
            //m_QuadVertexArray->GetIndexBuffer
            //RenderCommand::Draw(*m_QuadVertexArray, *m_QuadVertexArray->GetIndexBuffer(), *m_QuadShader);
            //m_QuadVertexBuffer->Unbind();
            m_QuadShader->Unbind();

            m_Stats.DrawCalls++;
        }

        if (m_LineVertexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_LineVertexBufferPtr - (uint8_t*)m_LineVertexBufferBase);
            m_LineVertexBuffer->SetData(m_LineVertexBufferBase, dataSize);

            m_LineShader->Bind();
            RenderCommand::SetLineWidth(m_LineWidth);
            RenderCommand::DrawLines(*m_LineVertexArray, m_LineVertexCount);
            m_LineShader->Unbind();
            m_Stats.DrawCalls++;
        }
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, 0 /* TODO: editor should insert entity ID here */);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { 
            { 0.0f, 0.0f }, 
            { 1.0f, 0.0f }, 
            { 1.0f, 1.0f },
            { 0.0f, 1.0f } 
        };
        const float tilingFactor = 1.0f;

        if (m_QuadIndexCount >= m_MaxIndices)
        {
            LOG_TRACE("Next Batch");
            NextBatch();
        }

        m_QuadShader->Bind();
		m_QuadShader->SetUniformMat4f("u_TransformMatrix", transform);
        for (size_t i = 0; i < quadVertexCount; i++)
        {
            LOG_TRACE("VertexBufferPtr: {}", (uint64_t)m_QuadVertexBufferPtr);
            //LOG_WARN("m_QuadVertexBufferPtr->Position == ({}, {})", m_QuadVertexBufferPtr->Position.x, m_QuadVertexBufferPtr->Position.y );
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i]; 
            m_QuadVertexBufferPtr->Color = color;
            m_QuadVertexBufferPtr->TexCoord = textureCoords[i];
            m_QuadVertexBufferPtr->TexIndex = textureIndex;
            m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
            m_QuadVertexBufferPtr->EntityID = entityID;
            m_QuadVertexBufferPtr++;
            //LOG_ERROR("m_QuadVertexBufferPtr->Position == ({}, {})", m_QuadVertexBufferPtr->Position.x, m_QuadVertexBufferPtr->Position.y );

            //m_QuadShader->Unbind();
        }
        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;
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