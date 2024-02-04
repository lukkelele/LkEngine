#include "LKpch.h"
#include "OpenGLRenderer2D.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "OpenGLRenderer.h"
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

    OpenGLRenderer2D::OpenGLRenderer2D(const OpenGLRenderer2DSpecification& specification) 
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
        {
            m_QuadShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");
            m_LineShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");
        }

        // Quad
        {
		    FramebufferSpecification quadFramebufferSpec;
		    quadFramebufferSpec.Attachments = { ImageFormat::RGBA32F, ImageFormat::DEPTH24STENCIL8 };
		    quadFramebufferSpec.Samples = 1;
		    quadFramebufferSpec.ClearColorOnLoad = false;
		    quadFramebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		    quadFramebufferSpec.DebugName = "OpenGLRenderer2D_Framebuffer";
            quadFramebufferSpec.Width = Window::Get().GetWidth();
            quadFramebufferSpec.Height = Window::Get().GetHeight();
		    //m_QuadFramebuffer = Framebuffer::Create(quadFramebufferSpec);

            m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

            // Pipeline
            PipelineSpecification quadPipelineSpec;
            //quadPipelineSpec.TargetFramebuffer = m_QuadFramebuffer;
            quadPipelineSpec.DebugName = "Renderer2D-QuadPipeline";
            quadPipelineSpec.Shader = m_QuadShader;

            RenderPassSpecification quadPassSpec;
            quadPassSpec.DebugName = "Renderer2D-QuadPass";
            quadPassSpec.Pipeline = Pipeline::Create(quadPipelineSpec);
            Ref<OpenGLPipeline> openglPipeline = quadPassSpec.Pipeline.As<OpenGLPipeline>();

            // Set up quad shader to use the correct amount of texture array uniforms 
            for (uint8_t textureArray = 0; textureArray < m_Specification.TextureArraysUsed; textureArray++)
                openglPipeline->BindTextureArray(textureArray);
            m_QuadPass = RenderPass::Create(quadPassSpec);
            
            m_QuadVertexBuffer = VertexBuffer::Create(m_MaxVertices * sizeof(QuadVertex));
            m_QuadVertexBuffer->SetLayout({
                { "a_Position",       ShaderDataType::Float3  },
                { "a_Color",          ShaderDataType::Float4  },
                { "a_Texcoord",       ShaderDataType::Float2  },
                { "a_TexIndex",       ShaderDataType::Float,  },
                { "a_TexArray",       ShaderDataType::Float,  },
                { "a_TilingFactor",   ShaderDataType::Float,  },
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

        m_WhiteTexture = TextureLibrary::Get()->GetWhiteTexture2D();

        m_CameraBuffer.ViewProjection = glm::mat4(1.0f);
        m_CameraUniformBuffer = Ref<OpenGLUniformBuffer>::Create(sizeof(CameraData));
        m_CameraUniformBuffer->SetBinding(m_QuadShader, "UB_Camera", 0); // Binding is set default to 0 in Renderer_Quad.shader

        m_RenderCommandBuffer = RenderCommandBuffer::Create(0, "OpenGLRenderer2D-RenderCommandBuffer");

        for (uint32_t i = 0; i < m_TextureArrays.size(); i++)
        {
            if (m_TextureArrays[i])
            {
                m_TextureArrays[i]->Bind();
            }
        }


        Initialized = true;
    }

    void OpenGLRenderer2D::BeginScene(const SceneCamera& camera)
    {
        m_CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        StartBatch();
    }

    void OpenGLRenderer2D::BeginScene(const SceneCamera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        StartBatch();
    }

    void OpenGLRenderer2D::BeginScene(const glm::mat4& viewProjectionMatrix)
    {
        m_CameraBuffer.ViewProjection = viewProjectionMatrix;
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

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

        // Quads
        if (m_QuadIndexCount)
        {
            dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

			Renderer::RenderGeometry(m_RenderCommandBuffer, m_QuadPass->GetPipeline(), m_QuadShader, m_QuadVertexBuffer, m_QuadIndexBuffer, m_CameraBuffer.ViewProjection, m_QuadIndexCount);

            m_Stats.DrawCalls++;
        }
    }

    void OpenGLRenderer2D::Shutdown()
    {
        for (auto& textureArray : m_TextureArrays)
        {
            textureArray->~OpenGLTextureArray();
        }

        delete m_QuadVertexBufferBase;
        delete m_QuadVertexBufferPtr;

        delete m_LineVertexBufferBase;
        delete m_LineVertexBufferPtr;
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
                                                                                                          
    void OpenGLRenderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
    {
        LK_ASSERT(false);
        const float textureIndex = 0; 
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
            m_QuadVertexBufferPtr->Texcoord = TextureCoords[i];
            m_QuadVertexBufferPtr->Color = color;
            m_QuadVertexBufferPtr->TexIndex = textureIndex; // White texture
            m_QuadVertexBufferPtr->TexArray = 0;            // White texture
            m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
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
        m_LineVertexBufferPtr->Position = p0;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;

        m_LineVertexBufferPtr->Position = p1;
        m_LineVertexBufferPtr->Color = color;
        m_LineVertexBufferPtr++;

        m_LineIndexCount += 2;
        m_Stats.LineCount++;
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture2D, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, texture2D, rotation, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, float rotation, uint64_t entityID)
    {
        DrawQuad(pos, size, texture, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation, entityID);
    }

    void OpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
    {
        LK_CORE_ASSERT(texture, "Passed texture to DrawQuad was nullptr");

        if (m_QuadIndexCount >= m_MaxIndices)
            NextBatch();

        float textureIndex = 0.0f;
        float textureArrayIndex = 0.0f;
        const float tilingFactor = 1.0f;
        constexpr size_t quadVertexCount = 4;

        for (uint32_t i = 0; i < m_TextureArrays.size(); i++)
        {
            if (m_TextureArrays[i] && m_TextureArrays[i]->HasTexture(texture))
            {
                textureIndex = m_TextureArrays[i]->GetIndexOfTexture(texture);
                textureArrayIndex = (float)i;
                break;
            }
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), { pos.x, pos.y, pos.z })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
            m_QuadVertexBufferPtr->Color = tintColor;
            m_QuadVertexBufferPtr->Texcoord = TextureCoords[i];
            m_QuadVertexBufferPtr->TexIndex = textureIndex;
            m_QuadVertexBufferPtr->TexArray = textureArrayIndex;
            m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
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
                LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), i);
                break;
            }
        }
    }

    void OpenGLRenderer2D::AddTextureToSlot(Ref<Texture2D> texture, int slot)
    {
        if (m_TextureSlots[slot] == nullptr)
        {
            m_TextureSlots[slot] = texture;
            LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), slot);
        }
    }

	void OpenGLRenderer2D::AddQuadBuffer()
	{
	}

	void OpenGLRenderer2D::AddLineBuffer()
	{
	}

	QuadVertex*& OpenGLRenderer2D::GetWriteableQuadBuffer()
	{
		return m_QuadVertexBufferPtr;
	}

	LineVertex*& OpenGLRenderer2D::GetWriteableLineBuffer()
	{
        return m_LineVertexBufferPtr;
	}

    void OpenGLRenderer2D::AddTextureArray(const Ref<OpenGLTextureArray>& textureArray)
    {
        for (int i = 0; i < MaxTextureArrays; i++)
        {
            if (m_TextureArrays[i] == nullptr)
                m_TextureArrays[i] = textureArray;
        }
    }

}
