#include "LKpch.h"
#include "VulkanRenderer2D.h"

#include "VulkanImage.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Scene.h"



namespace LkEngine {

    static bool Initialized = false;

    // Basic UV coordinates
    constexpr glm::vec2 TextureCoords[] = { 
        { 0.0f, 0.0f }, // Bottom left 
        { 0.0f, 1.0f }, // Top left
        { 1.0f, 1.0f }, // Top right
        { 1.0f, 0.0f }  // Bottom right
    };

    VulkanRenderer2D::VulkanRenderer2D(const Renderer2DSpecification& specification)
        : m_Specification(specification)
        , m_MaxQuadVertices(specification.MaxQuads * 4)
        , m_MaxQuadIndices(specification.MaxQuads * 6)
        , m_MaxLineVertices(specification.MaxLines * 2)
        , m_MaxLineIndices(specification.MaxLines * 6)
    {
        m_CameraBuffer = {};
        m_CameraUniformBuffer = {};
    }

    VulkanRenderer2D::~VulkanRenderer2D()
    {
    }

    void VulkanRenderer2D::Init()
    {
        LK_CORE_DEBUG_TAG("VulkanRenderer2D", "Initializing");
        const uint32_t framesInFlight = Renderer::GetFramesInFlight();

        // Shaders
        m_QuadShader = Renderer::GetShaderLibrary()->Get("VulkanRenderer2D_Quad");
        m_LineShader = Renderer::GetShaderLibrary()->Get("VulkanRenderer2D_Line");

		FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { ImageFormat::RGBA32F, ImageFormat::Depth };
		framebufferSpec.Samples = 1;
		framebufferSpec.ClearColorOnLoad = false;
		framebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		framebufferSpec.DebugName = "VulkanRenderer2D Framebuffer";
        framebufferSpec.Width = Window::Get()->GetViewportWidth();
        framebufferSpec.Height = Window::Get()->GetViewportHeight();

		Ref<Framebuffer> framebuffer = Framebuffer::Create(framebufferSpec);
        // Quads
        {
            // Pipeline
            PipelineSpecification pipelineSpecification;
            pipelineSpecification.DebugName = "VulkanRenderer2D-Quad";
            pipelineSpecification.Shader = Renderer::GetShaderLibrary()->Get("VulkanRenderer2D_Quad");
            LK_CORE_VERIFY(pipelineSpecification.Shader != nullptr, "Shader is nullptr");
            pipelineSpecification.TargetFramebuffer = framebuffer;
            pipelineSpecification.BackfaceCulling = false;
            pipelineSpecification.Layout = {
                { "a_Pos",          ShaderDataType::Float3  },
                { "a_Color",        ShaderDataType::Float4  },
                { "a_TexCoord",     ShaderDataType::Float2  },
                { "a_TexIndex",     ShaderDataType::Int,    },
                { "a_TilingFactor", ShaderDataType::Float,  },
                { "a_EntityID",     ShaderDataType::Int     },
            };

            RenderPassSpecification quadSpec;
            quadSpec.DebugName = "VulkanRenderer2D-Quad";
            quadSpec.Pipeline = Pipeline::Create(pipelineSpecification);
            m_QuadPass = RenderPass::Create(quadSpec);
            m_QuadPass->SetInput("Camera", m_CameraUniformBuffer);
            LK_CORE_VERIFY(m_QuadPass->Validate());
            m_QuadPass->Bake();
            
            m_QuadVertexBuffer = VertexBuffer::Create(m_MaxQuadVertices * sizeof(QuadVertex));
            m_QuadVertexBuffer->SetLayout({
                { "a_Pos",          ShaderDataType::Float3  },
                { "a_Color",        ShaderDataType::Float4  },
                { "a_TexCoord",     ShaderDataType::Float2  },
                { "a_TexIndex",     ShaderDataType::Int,    },
                { "a_TilingFactor", ShaderDataType::Float,  },
                { "a_EntityID",     ShaderDataType::Int     },
            });
            m_QuadVertexBufferBase = new QuadVertex[m_MaxQuadVertices];

            m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

            uint32_t* quadIndices = new uint32_t[m_MaxQuadIndices];

            uint32_t offset = 0;
            for (uint32_t i = 0; i < m_MaxQuadIndices; i += 6)
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

            Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_MaxQuadIndices);
            m_QuadVertexBuffer->SetIndexBuffer(quadIB);
            delete[] quadIndices;
        }

        // Line
        m_LineVertexBuffer = VertexBuffer::Create(m_MaxQuadVertices * sizeof(LineVertex));
        VertexBufferLayout lineVertexBufLayout{ };
        m_LineVertexBuffer->SetLayout({
            { "a_Pos",       ShaderDataType::Float3, },
            { "a_Color",     ShaderDataType::Float4, },
            { "a_EntityID",  ShaderDataType::Int,  }
        });
        m_LineVertexBufferBase = new LineVertex[m_MaxQuadVertices];

        uint32_t* lineIndices = new uint32_t[m_MaxLineIndices];
        for (uint32_t i = 0; i < m_MaxLineIndices; i++)
		    lineIndices[i] = i;
		delete[] lineIndices;

        // Textures
        m_WhiteTexture = TextureLibrary::Get()->GetWhiteTexture();

        // First retrieved texture is always the white one from TextureLibrary
        auto textures2D = TextureLibrary::Get()->GetTextures2D();
        for (int i = 0; i < textures2D.size(); i++)
            m_TextureSlots[i] = textures2D[i].second;

        m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData));
        LK_CORE_TRACE_TAG("VulkanRenderer2D", "Created m_CameraUniformBuffer");

        m_RenderCommandBuffer = RenderCommandBuffer::Create(0, "VulkanRenderer2D-RenderCommandBuffer");

        Initialized = true;
    }

    void VulkanRenderer2D::Shutdown()
    {
    }

    void VulkanRenderer2D::BeginScene(const Camera& camera)
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

    void VulkanRenderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind(i);
        }

        StartBatch();
    }

    void VulkanRenderer2D::StartBatch()
    {
        m_QuadIndexCount = 0;
        m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

        m_LineIndexCount = 0;
        m_LineVertexBufferPtr = m_LineVertexBufferBase;
    }

    void VulkanRenderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void VulkanRenderer2D::EndScene()
    {
        Flush();
    }

    void VulkanRenderer2D::Flush()
    {
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();
        uint32_t dataSize = 0;
        //----------------------------------------------------------
        // Quads
        //----------------------------------------------------------
        if (m_QuadIndexCount)
        {
            dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

            m_QuadShader->Bind();
            m_QuadShader->Set("u_ViewProj", Scene::GetActiveScene()->GetCamera()->GetViewProjection());

            // Bind textures
            for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
            {
                if (m_TextureSlots[i])
                {
                    m_TextureSlots[i]->Bind(i);
                    m_QuadShader->Set("u_TextureArray[" + std::to_string(i) + "]", i);
                }
            }

			Renderer::BeginRenderPass(m_RenderCommandBuffer, m_QuadPass);
			Renderer::RenderGeometry(m_RenderCommandBuffer, m_QuadPass->GetPipeline(), m_QuadShader, m_QuadVertexBuffer, m_QuadIndexBuffer, glm::mat4(1.0f), m_QuadIndexCount);
			Renderer::EndRenderPass(m_RenderCommandBuffer);

            m_QuadShader->Unbind();

            m_Stats.DrawCalls++;
        }

        //----------------------------------------------------------
        // Lines
        //----------------------------------------------------------
        // disable for now
#if 0
        if (m_LineIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_LineVertexBufferPtr - (uint8_t*)m_LineVertexBufferBase);
            m_LineVertexBuffer->SetData(m_LineVertexBufferBase, dataSize);

            // TODO: Texture binding here, same as with quads
            m_LineShader->Bind();
            //m_LineShader->SetUniformMat4f("u_ViewProj", Scene::GetActiveScene()->GetCamera()->GetViewProjection());
            m_LineShader->Set("u_ViewProj", Scene::GetActiveScene()->GetCamera()->GetViewProjection());

            auto lineElements = m_LineVertexBuffer->GetLayout().GetElements();

            for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
            {
                if (m_TextureSlots[i])
                {
                    m_TextureSlots[i]->Bind(i);
                    //m_LineShader->SetUniform4f("a_Color", m_LineVertexBufferPtr->Color);
                    m_LineShader->Set("a_Color", m_LineVertexBufferPtr->Color);
                }
            }
            //RenderCommand::SetLineWidth(m_LineWidth);
            RenderCommand::DrawLines(*m_LineVertexBuffer, m_LineIndexCount);

            m_LineShader->Unbind();

            m_Stats.DrawCalls++;
        }
#endif

        m_TextureSlotIndex = 0;
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
    }

    void VulkanRenderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {                                                                                                     
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation);                                   
    } 

    void VulkanRenderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }
                                                                                                          
    void VulkanRenderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
        float textureIndex = 0; 
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

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
    }

    void VulkanRenderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID)
    {
        DrawLine({ p0.x, p0.y, 0.0f }, { p1.x, p1.y, 0.0f }, color, entityID);
    }

    void VulkanRenderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID)
    {
		auto& bufferPtr = GetWriteableLineBuffer();
		bufferPtr->Position = p0;
		bufferPtr->Color = color;
		bufferPtr++;

		bufferPtr->Position = p1;
		bufferPtr->Color = color;
		bufferPtr++;


        m_LineIndexCount += 2;

        m_Stats.LineCount++;
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture2D, float rotation, uint64_t entityID)
    {
        //if (texture2D == nullptr) return;
        DrawQuad({ pos.x, pos.y, 0.0f }, size, texture2D, rotation, entityID);
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID)
    {
        DrawQuad(pos, size, texture, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation, entityID);
    }

    void VulkanRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
    {
        if (texture == nullptr)
            throw std::runtime_error("Passed texture to DrawQuad was nullptr");

        if (m_QuadIndexCount >= m_MaxQuadIndices)
            NextBatch();

        float textureIndex = 0.0f;
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

        for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i] != nullptr && m_TextureSlots[i]->GetName() == texture->GetName())
            {
                textureIndex = (float)i;
                break;
            }
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, pos.z })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
            m_QuadVertexBufferPtr->TexCoord = TextureCoords[i];
            m_QuadVertexBufferPtr->Color = tintColor;
            m_QuadVertexBufferPtr->TexIndex = textureIndex;
            m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
            m_QuadVertexBufferPtr->EntityID = entityID;
            m_QuadVertexBufferPtr++;
        }

        m_QuadIndexCount += 6;
        m_Stats.QuadCount++;
    }

    float VulkanRenderer2D::GetLineWidth()
    {
        return m_LineWidth;
    }

    void VulkanRenderer2D::SetLineWidth(float width)
    {
    }

    void VulkanRenderer2D::ResetStats()
    {
    }

    Renderer2DAPI::Statistics VulkanRenderer2D::GetStats()
    {
        return m_Stats;
    }

    void VulkanRenderer2D::AddTextureToSlot(Ref<Texture> texture)
    {
        for (int i = 1; i < MaxTextureSlots; i++)
        {
            if (m_TextureSlots[i] == nullptr)
            {
                m_TextureSlots[i] = texture;
                LK_CORE_DEBUG_TAG("VulkanRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), i);
                break;
            }
        }
    }

    void VulkanRenderer2D::AddTextureToSlot(Ref<Texture> texture, int slot)
    {
        if (m_TextureSlots[slot] == nullptr)
        {
            m_TextureSlots[slot] = texture;
            LK_CORE_DEBUG_TAG("VulkanRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), slot);
        }
    }

	void VulkanRenderer2D::AddQuadBuffer()
	{
	}

	void VulkanRenderer2D::AddLineBuffer()
	{
	}

	Renderer2DAPI::QuadVertex*& VulkanRenderer2D::GetWriteableQuadBuffer()
	{
		return m_QuadVertexBufferPtr;
	}

	Renderer2DAPI::LineVertex*& VulkanRenderer2D::GetWriteableLineBuffer()
	{
        return m_LineVertexBufferPtr;
	}


}
