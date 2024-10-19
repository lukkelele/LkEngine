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

    LOpenGLRenderer2D::LOpenGLRenderer2D(const OpenGLRenderer2DSpecification& InSpecification) 
        : m_Specification(InSpecification)
        , m_MaxVertices(InSpecification.MaxQuads * 4)
        , m_MaxIndices(InSpecification.MaxQuads * 6)
        , m_MaxLineVertices(InSpecification.MaxLines * 2)
        , m_MaxLineIndices(InSpecification.MaxLines * 6)
    {
        Renderer2DAPI = this;

        m_CameraBuffer = {};
        m_CameraUniformBuffer = {};

        m_QuadIndexCount = 0;
        m_LineIndexCount = 0;
    }

    void LOpenGLRenderer2D::Init()
    {
        LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Initializing");
        const uint32_t framesInFlight = LRenderer::GetFramesInFlight();

        // Shaders
        {
            m_QuadShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Quad");
            m_LineShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Line");
        }

        // Quad
        {
            FFramebufferSpecification QuadFramebufferSpec{};
		    QuadFramebufferSpec.Attachments = {
                EImageFormat::RGBA32F, 
                EImageFormat::DEPTH24STENCIL8 
            };
		    QuadFramebufferSpec.Samples = 1;
		    QuadFramebufferSpec.ClearColorOnLoad = false;
		    QuadFramebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		    QuadFramebufferSpec.DebugName = "OpenGLRenderer2D_Framebuffer";
            QuadFramebufferSpec.Width = LWindow::Get().GetWidth();
            QuadFramebufferSpec.Height = LWindow::Get().GetHeight();
		    TObjectPtr<LFramebuffer> QuadFramebuffer = LFramebuffer::Create(QuadFramebufferSpec);

            m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

            // Quad Pipeline
            FPipelineSpecification QuadPipelineSpec{};
            QuadPipelineSpec.TargetFramebuffer = nullptr;
            QuadPipelineSpec.DebugName = "Renderer2D-QuadPipeline";
            QuadPipelineSpec.Shader = m_QuadShader;

            RenderPassSpecification QuadPassSpec;
            QuadPassSpec.DebugName = "Renderer2D-QuadPass";
            QuadPassSpec.Pipeline = LPipeline::Create(QuadPipelineSpec);
            TObjectPtr<LOpenGLPipeline> OpenGLPipeline = QuadPassSpec.Pipeline.As<LOpenGLPipeline>();
            m_QuadMaterial = LMaterial::Create(m_QuadShader, "QuadMaterial");

            // Use correct amount of texture array uniforms
            for (uint8_t ArrayIndex = 0; ArrayIndex < m_Specification.TextureArraysUsed; ArrayIndex++)
            {
                OpenGLPipeline->BindTextureArray(ArrayIndex);
            }
            m_QuadPass = LRenderPass::Create(QuadPassSpec);
            
            m_QuadVertexBuffer = LVertexBuffer::Create(m_MaxVertices * sizeof(QuadVertex));
            m_QuadVertexBuffer->SetLayout({
                { "a_Position",       ShaderDataType::Float3  },
                { "a_Color",          ShaderDataType::Float4  },
                { "a_Texcoord",       ShaderDataType::Float2  },
                { "a_TexIndex",       ShaderDataType::Float,  },
                { "a_TexArray",       ShaderDataType::Float,  },
                { "a_TilingFactor",   ShaderDataType::Float,  },
            });

            m_QuadVertexBufferBase = new QuadVertex[m_MaxVertices];
            uint32_t* QuadIndices = new uint32_t[m_MaxIndices];
            uint32_t Offset = 0;

            for (uint32_t i = 0; i < m_MaxIndices; i += 6)
            {
                // First triangle, 0->1->2
                QuadIndices[i + 0] = Offset + 0;
                QuadIndices[i + 1] = Offset + 1;
                QuadIndices[i + 2] = Offset + 2;

                // Second triangle, 2->3->0
                QuadIndices[i + 3] = Offset + 2;
                QuadIndices[i + 4] = Offset + 3;
                QuadIndices[i + 5] = Offset + 0;

                Offset += 4;
            }

            TObjectPtr<LIndexBuffer> quadIB = LIndexBuffer::Create(QuadIndices, m_MaxIndices);
            m_QuadVertexBuffer->SetIndexBuffer(quadIB);
            m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

            delete[] QuadIndices;
        }

        // Lines
        {
            m_LineVertexBuffer = LVertexBuffer::Create(m_MaxVertices * sizeof(LineVertex));
            VertexBufferLayout lineVertexBufLayout{ };
            m_LineVertexBuffer->SetLayout({
                { "a_Pos",       ShaderDataType::Float3, },
                { "a_Color",     ShaderDataType::Float4, },
                { "a_EntityID",  ShaderDataType::Int,  }
            });
            m_LineVertexBufferBase = new LineVertex[m_MaxVertices];

            uint32_t* LineIndices = new uint32_t[m_MaxLineIndices];
            for (uint32_t i = 0; i < m_MaxLineIndices; i++)
            {
		        LineIndices[i] = i;
            }
		    delete[] LineIndices;
        }

        m_WhiteTexture = LTextureLibrary::Get().GetWhiteTexture();

        m_CameraBuffer.ViewProjection = glm::mat4(1.0f);
        m_CameraUniformBuffer = TObjectPtr<LOpenGLUniformBuffer>::Create(sizeof(CameraData));
        m_CameraUniformBuffer->SetBinding(m_QuadShader, "UB_Camera", 0); // Binding is set default to 0 in Renderer_Quad.shader

        m_RenderCommandBuffer = LRenderCommandBuffer::Create(0, "OpenGLRenderer2D-RenderCommandBuffer");

        for (uint32_t i = 0; i < m_TextureArrays.size(); i++)
        {
            if (m_TextureArrays[i])
            {
                m_TextureArrays[i]->Bind();
            }
        }

        Initialized = true;
    }

    void LOpenGLRenderer2D::BeginScene(const LSceneCamera& camera)
    {
        m_CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        StartBatch();
    }

    void LOpenGLRenderer2D::BeginScene(const LSceneCamera& camera, const glm::mat4& transform)
    {
        m_CameraBuffer.ViewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        StartBatch();
    }

    void LOpenGLRenderer2D::BeginScene(const glm::mat4& viewProjectionMatrix)
    {
        m_CameraBuffer.ViewProjection = viewProjectionMatrix;
        m_CameraUniformBuffer->SetData(&m_CameraBuffer, sizeof(CameraData));

        StartBatch();
    }

    void LOpenGLRenderer2D::StartBatch()
    {
        m_QuadIndexCount = 0;
        m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

        m_LineIndexCount = 0;
        m_LineVertexBufferPtr = m_LineVertexBufferBase;
    }

    void LOpenGLRenderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void LOpenGLRenderer2D::EndScene()
    {
        Flush();
    }

    void LOpenGLRenderer2D::Flush()
    {
		uint32_t frameIndex = LRenderer::GetCurrentFrameIndex();
        uint32_t dataSize = 0;

        // Quads
        if (m_QuadIndexCount)
        {
            //dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            dataSize = static_cast<uint32_t>((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
            m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

			LRenderer::RenderGeometry(m_RenderCommandBuffer, 
                                      m_QuadPass->GetPipeline(), 
                                      m_QuadShader, 
                                      m_QuadVertexBuffer, 
                                      m_QuadIndexBuffer, 
                                      m_CameraBuffer.ViewProjection, 
                                      m_QuadIndexCount);

            m_Stats.DrawCalls++;
        }
    }

    void LOpenGLRenderer2D::Shutdown()
    {
        for (TObjectPtr<OpenGLTextureArray>& TextureArray : m_TextureArrays)
        {
            //TextureArray->~OpenGLTextureArray();
            //TextureArray->Destroy();
        }

        delete m_QuadVertexBufferBase;
        delete m_QuadVertexBufferPtr;

        delete m_LineVertexBufferBase;
        delete m_LineVertexBufferPtr;
    }

    void LOpenGLRenderer2D::DrawImage(const TObjectPtr<LImage>& Image)
    {
        LK_UNUSED(Image);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
    }

    void LOpenGLRenderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {                                                                                                     
        DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation);                                   
    } 

    void LOpenGLRenderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        DrawQuad(transform, color, entityID);
    }
                                                                                                          
    void LOpenGLRenderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID)
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

    void LOpenGLRenderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID)
    {
        DrawLine({ p0.x, p0.y, 0.0f }, { p1.x, p1.y, 0.0f }, color, entityID);
    }

    void LOpenGLRenderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID)
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

    void LOpenGLRenderer2D::DrawQuad(const glm::vec2& pos, 
                                     const glm::vec2& size, 
                                     TObjectPtr<LTexture2D> texture2D, 
                                     const float rotation, 
                                     const uint64_t entityID)
    {
        DrawQuad({ pos.x, pos.y, 0.0f }, size, texture2D, rotation, entityID);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec3& pos, 
                                     const glm::vec2& size, 
                                     TObjectPtr<LTexture2D> texture, 
                                     const float rotation, 
                                     const uint64_t entityID)
    {
        DrawQuad(pos, size, texture, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation, entityID);
    }

    void LOpenGLRenderer2D::DrawQuad(const glm::vec3& pos, 
                                     const glm::vec2& size, 
                                     TObjectPtr<LTexture2D> texture, 
                                     const glm::vec4& tintColor, 
                                     const float rotation, 
                                     const uint64_t entityID)
    {
        LK_CORE_ASSERT(texture, "Passed texture to DrawQuad was nullptr");
        if (m_QuadIndexCount >= m_MaxIndices)
        {
            NextBatch();
        }

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

    float LOpenGLRenderer2D::GetLineWidth()
    {
        return m_LineWidth;
    }

    void LOpenGLRenderer2D::SetLineWidth(float width)
    {
    }

    void LOpenGLRenderer2D::ResetStats()
    {
    }

    IRenderer2DAPI::Statistics LOpenGLRenderer2D::GetStats()
    {
        return m_Stats;
    }

    void LOpenGLRenderer2D::AddTextureToSlot(TObjectPtr<LTexture2D> texture)
    {
        for (int i = 1; i < MaxTextureSlots; i++)
        {
            if (!m_TextureSlots[i])
            {
                m_TextureSlots[i] = texture;
                LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), i);
                break;
            }
        }
    }

    void LOpenGLRenderer2D::AddTextureToSlot(TObjectPtr<LTexture2D> texture, int slot)
    {
        if (m_TextureSlots[slot] == nullptr)
        {
            m_TextureSlots[slot] = texture;
            LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Added texture \"{}\" to slot {}!", texture->GetName(), slot);
        }
    }

	void LOpenGLRenderer2D::AddQuadBuffer()
	{
	}

	void LOpenGLRenderer2D::AddLineBuffer()
	{
	}

	QuadVertex*& LOpenGLRenderer2D::GetWriteableQuadBuffer()
	{
		return m_QuadVertexBufferPtr;
	}

	LineVertex*& LOpenGLRenderer2D::GetWriteableLineBuffer()
	{
        return m_LineVertexBufferPtr;
	}

    void LOpenGLRenderer2D::AddTextureArray(const TObjectPtr<OpenGLTextureArray>& textureArray)
    {
        for (int i = 0; i < MaxTextureArrays; i++)
        {
            if (m_TextureArrays[i] == nullptr)
            {
                m_TextureArrays[i] = textureArray;
            }
        }
    }

}
