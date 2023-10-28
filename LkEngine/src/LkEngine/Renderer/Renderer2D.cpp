#include "LKpch.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    s_ptr<Renderer2D> Renderer2D::s_Instance = nullptr;

    Renderer2D::Renderer2D(const Renderer2DSpecification& specification)
        : m_Specification(specification)
        , m_MaxVertices(specification.MaxQuads * 4)
        , m_MaxIndices(specification.MaxQuads * 6)
        , m_MaxLineVertices(specification.MaxLines * 2)
        , m_MaxLineIndices(specification.MaxLines * 6)
    {
        s_Instance = std::shared_ptr<Renderer2D>(this);
    }

    Renderer2D::~Renderer2D()
    {
    }

    void Renderer2D::Init()
    {
        if (m_QuadVertexArray)
        {
            LOG_CRITICAL("Renderer2D already initialized");
            exit(1);
            return;
        }
        m_QuadVertexArray = VertexArray::Create();

        m_QuadVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(QuadVertex));
        m_QuadVertexBuffer->SetLayout({
            { "a_Position",     ShaderDataType::Float3  },
            { "a_Color",        ShaderDataType::Float4  },
            { "a_EntityID",     ShaderDataType::Float  },
        });
        m_QuadVertexArray->AddVertexBuffer(*m_QuadVertexBuffer);
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
        m_QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        m_WhiteTexture = std::make_shared<Texture2D>("assets/img/atte_square.png");
        m_TextureSlots[0] = m_WhiteTexture;

        m_QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.shader");

        m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
        m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };
        //m_QuadVertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f }; 
        //m_QuadVertexPositions[1] = { -1.0f,  1.0f, 0.0f, 1.0f };
        //m_QuadVertexPositions[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
        //m_QuadVertexPositions[3] = {  1.0f, -1.0f, 0.0f, 1.0f };

        m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);

        m_Stats.QuadVertexArray_RendererID = m_QuadVertexArray->GetID();
        m_Stats.QuadVertexBuffer_RendererID = m_QuadVertexBuffer->GetID();
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
        StartBatch();
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));
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
            // Bind textures
            /* <<< bind textures here >>> */

            m_QuadShader->Bind();
            m_QuadShader->SetUniformMat4f("u_ViewProj", Scene::ActiveScene->GetActiveCamera()->GetViewProjection());
            RenderCommand::DrawIndexed(*m_QuadVertexArray, m_QuadIndexCount);
            m_QuadShader->Unbind();

            m_Stats.DrawCalls++;
        }
    }

    void Renderer2D::DrawEntity(Entity& entity)
    {
        if (!entity.HasComponent<MeshComponent>())
	    	return;
	    auto& mesh = entity.GetComponent<MeshComponent>();
	    auto& tc = entity.GetComponent<TransformComponent>();
	    auto& sc = entity.GetComponent<SpriteComponent>();

        DrawQuad(tc.GetTransform(), sc.Color, entity.GetUUID());
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

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color, uint64_t entityID)
    {
        DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawRotatedQuad(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
        constexpr size_t quadVertexCount = 4;

        if (m_QuadIndexCount >= m_MaxIndices) 
            NextBatch();

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i]; 
            m_QuadVertexBufferPtr->Color = color;
            m_QuadVertexBufferPtr->EntityID = entityID;
            m_QuadVertexBufferPtr++;
        }
        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color, uint64_t entityID)
    {
        DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawRotatedQuad(transform, color, entityID);
    }

    void Renderer2D::DrawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
        constexpr size_t quadVertexCount = 4;

        if (m_QuadIndexCount >= m_MaxIndices) 
            NextBatch();

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i]; 
            m_QuadVertexBufferPtr->Color = color;
            m_QuadVertexBufferPtr->EntityID = entityID;
            m_QuadVertexBufferPtr++;
        }
        m_QuadIndexCount += 6;

        m_Stats.QuadCount++;
    }


    void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, uint64_t entityID)
    {
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