#include "LKpch.h"
#include "OpenGLRenderer2D.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"
//#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "OpenGLUniformBuffer.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLSwapChain.h"
#include "OpenGLRenderPass.h"


namespace LkEngine {

    static bool Initialized = false;
    static int Uniform_TextureArray_Quad_Index = 0;
    static int Uniform_TextureArray_Quad_ActiveUnit = GL_TEXTURE0;

    OpenGLRenderer2D::OpenGLRenderer2D(const Renderer2DSpecification& specification) 
        : m_Specification(specification)
        , m_MaxVertices(specification.MaxQuads * 4)
        , m_MaxIndices(specification.MaxQuads * 6)
        , m_MaxLineVertices(specification.MaxLines * 2)
        , m_MaxLineIndices(specification.MaxLines * 6)
    {
        m_Renderer2DAPI = this;

        m_CameraBuffer = {};
        m_CameraUniformBuffer = {};

        m_QuadIndexCount = 0;
        m_LineIndexCount = 0;

    }

    OpenGLRenderer2D::~OpenGLRenderer2D()
    {
    }

    void OpenGLRenderer2D::Init()
    {
        LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Initializing");
        const uint32_t framesInFlight = Renderer::GetFramesInFlight();

        // Shaders
        m_QuadShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");
        m_LineShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");

        // Target Framebuffer
        {
		    FramebufferSpecification framebufferSpec;
		    framebufferSpec.Attachments = { ImageFormat::RGB, ImageFormat::DEPTH24STENCIL8 };
		    framebufferSpec.Samples = 1;
		    framebufferSpec.ClearColorOnLoad = false;
		    framebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		    framebufferSpec.DebugName = "OpenGLRenderer2D Framebuffer";
            framebufferSpec.Width = Window::Get().GetWidth();
            framebufferSpec.Height = Window::Get().GetHeight();
		    m_TargetFramebuffer = Framebuffer::Create(framebufferSpec);
            LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Color attachment 0: {}", m_TargetFramebuffer->GetColorAttachmentRendererID(0));
        }

        // Debug OpenGL VAO's and VBO's
        {
            GenerateCubeVaoAndVbo(CubeVAO, CubeVBO);
            GeneratePlaneVaoAndVbo(PlaneVAO, PlaneVBO);
            GenerateScreenQuadVaoAndVbo(QuadVAO, QuadVBO);
            CubeTexture = LoadTexture("assets/textures/container.jpg");
            FloorTexture = LoadTexture("assets/textures/metal.png");
        }

        // Quads
        {
            // Vertex Positions
            m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

            // Pipeline
            
            // VertexBufferLayout
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

            Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_MaxIndices);
            m_QuadVertexBuffer->SetIndexBuffer(quadIB);
            m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
            LK_CORE_VERIFY(m_QuadVertexBuffer, "m_QuadVertexBuffer is nullptr");
            LK_CORE_VERIFY(m_QuadVertexBufferPtr, "m_QuadVertexBufferPtr is nullptr");

            delete[] quadIndices;
        }

        // Lines
        {
            m_LineVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(LineVertex));
            VertexBufferLayout lineVertexBufLayout{ };
            m_LineVertexBuffer->SetLayout({
                { "a_Pos",       ShaderDataType::Float3, },
                { "a_Color",     ShaderDataType::Float4, },
                { "a_EntityID",  ShaderDataType::Int,  }
            });
            m_LineVertexBufferBase = new LineVertex[m_MaxVertices];

            uint32_t* lineIndices = new uint32_t[m_MaxLineIndices];
            for (uint32_t i = 0; i < m_MaxLineIndices; i++)
		        lineIndices[i] = i;
		    delete[] lineIndices;
        }

        // Textures
        {
            auto textures2D = TextureLibrary::Get()->GetTextures2D();
            for (int i = 0; i < textures2D.size(); i++)
                m_TextureSlots[i] = textures2D[i].second;
            m_WhiteTexture = TextureLibrary::Get()->GetWhiteTexture2D();

            m_CameraBuffer.ViewProjection = glm::mat4(1.0f);
            m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData));
            LK_CORE_VERIFY(m_CameraUniformBuffer != nullptr, "CameraUniformBuffer is nullptr!");

            m_RenderCommandBuffer = RenderCommandBuffer::Create(0, "OpenGLRenderer2D-RenderCommandBuffer");
        }

        Initialized = true;
        LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Initialization done");
    }

    void OpenGLRenderer2D::Shutdown()
    {
    }

    void OpenGLRenderer2D::BeginScene(const SceneCamera& camera)
    {
        m_CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind();
        }

        StartBatch();
    }

    void OpenGLRenderer2D::BeginScene(const SceneCamera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind(i);
        }

        StartBatch();
    }

    void OpenGLRenderer2D::BeginScene(const glm::mat4& viewProjectionMatrix)
    {
        m_CameraBuffer.ViewProjection = viewProjectionMatrix;
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
        {
            if (m_TextureSlots[i])
                m_TextureSlots[i]->Unbind(i);
        }

        StartBatch();
    }

    void OpenGLRenderer2D::StartBatch()
    {
        m_QuadIndexCount = 0;
        m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

        m_LineIndexCount = 0;
        m_LineVertexBufferPtr = m_LineVertexBufferBase;
    }

    void OpenGLRenderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void OpenGLRenderer2D::EndScene()
    {
        Flush();
    }

    void OpenGLRenderer2D::Flush()
    {
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();
        uint32_t dataSize = 0;

        m_TargetFramebuffer->Bind();

        // Quads
        if (m_QuadIndexCount)
        {
            dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

            m_QuadShader->Bind();
            m_QuadShader->Set("u_ViewProj", m_CameraBuffer.ViewProjection);

            glActiveTexture(Uniform_TextureArray_Quad_ActiveUnit);
            m_QuadShader->Set("u_TextureArray", Uniform_TextureArray_Quad_Index);

			//Renderer::BeginRenderPass(m_RenderCommandBuffer, m_QuadPass);
			Renderer::RenderGeometry(m_RenderCommandBuffer, m_QuadPass->GetPipeline(), m_QuadShader, m_QuadVertexBuffer, m_QuadIndexBuffer, glm::mat4(1.0f), m_QuadIndexCount);
			//Renderer::EndRenderPass(m_RenderCommandBuffer);

            m_QuadShader->Unbind();
            m_Stats.DrawCalls++;
        }
        m_TargetFramebuffer->Unbind();


        // Lines
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

    void OpenGLRenderer2D::DrawImage(const Ref<Image> image)
    {
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
    }

    void OpenGLRenderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {                                                                                                     
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation);                                   
    } 

    void OpenGLRenderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }
                                                                                                          
    // Draw without texture - use color
    void OpenGLRenderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
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

    void OpenGLRenderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID)
    {
        DrawLine({ p0.x, p0.y, 0.0f }, { p1.x, p1.y, 0.0f }, color, entityID);
    }

    void OpenGLRenderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID)
    {
#if 0
        m_LineVertexBufferPtr->Position = p0;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;

        m_LineVertexBufferPtr->Position = p1;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;
#endif
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

    void OpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture2D, float rotation, uint64_t entityID)
    {
        //if (texture2D == nullptr) return;
        DrawQuad({ pos.x, pos.y, 0.0f }, size, texture2D, rotation, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, float rotation, uint64_t entityID)
    {
        DrawQuad(pos, size, texture, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
    {
        if (texture == nullptr)
            throw std::runtime_error("Passed texture to DrawQuad was nullptr");

        if (m_QuadIndexCount >= m_MaxIndices)
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

    float OpenGLRenderer2D::GetLineWidth()
    {
        return m_LineWidth;
    }

    void OpenGLRenderer2D::SetLineWidth(float width)
    {
    }

    void OpenGLRenderer2D::ResetStats()
    {
    }

    Renderer2DAPI::Statistics OpenGLRenderer2D::GetStats()
    {
        return m_Stats;
    }

    void OpenGLRenderer2D::AddTextureToSlot(Ref<Texture2D> texture)
    {
        for (int i = 1; i < MaxTextureSlots; i++)
        {
            if (m_TextureSlots[i] == nullptr)
            {
                m_TextureSlots[i] = texture;
                LK_CORE_DEBUG("OpenGLRenderer2D: Added texture \"{}\" to slot {}!", texture->GetName(), i);
                break;
            }
        }
    }

    void OpenGLRenderer2D::AddTextureToSlot(Ref<Texture2D> texture, int slot)
    {
        if (m_TextureSlots[slot] == nullptr)
        {
            m_TextureSlots[slot] = texture;
            LK_CORE_DEBUG("OpenGLRenderer2D: Added texture \"{}\" to slot {}!", texture->GetName(), slot);
        }
    }

	void OpenGLRenderer2D::AddQuadBuffer()
	{
	}

	void OpenGLRenderer2D::AddLineBuffer()
	{
	}

	Renderer2DAPI::QuadVertex*& OpenGLRenderer2D::GetWriteableQuadBuffer()
	{
		return m_QuadVertexBufferPtr;
	}

	Renderer2DAPI::LineVertex*& OpenGLRenderer2D::GetWriteableLineBuffer()
	{
        return m_LineVertexBufferPtr;
	}

}
