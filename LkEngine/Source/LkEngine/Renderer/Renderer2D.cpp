#include "LKpch.h"
#include "Renderer2D.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine
{
	LRenderer2D::LRenderer2D(const FRenderer2DSpecification InSpecification)
		: Specification(InSpecification)
        , MaxVertices(InSpecification.MaxQuads * 4)
        , MaxIndices(InSpecification.MaxQuads * 6)
        , MaxLineVertices(InSpecification.MaxLines * 2)
        , MaxLineIndices(InSpecification.MaxLines * 6)
	{
		LCLASS_REGISTER();
		LK_CORE_ASSERT((MaxVertices > 0) && (MaxIndices > 0) && (MaxLineVertices > 0) && (MaxLineIndices > 0));
	}

	void LRenderer2D::Initialize()
    {
		/* Shaders. */
		QuadShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Quad");
		LineShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Line");

        /* Quads. */
        {
            QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

			/* Create the quad framebuffer. */
            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad framebuffer");
            FFramebufferSpecification QuadFramebufferSpec{};
		    QuadFramebufferSpec.Attachments = {
                EImageFormat::RGBA32F, 
                EImageFormat::DEPTH24STENCIL8 
            };
		    QuadFramebufferSpec.Samples = 1;
		    QuadFramebufferSpec.ClearColorOnLoad = false;
		    QuadFramebufferSpec.ClearColor = { 0.1f, 0.5f, 0.5f, 1.0f };
		    QuadFramebufferSpec.DebugName = "Renderer2D_Framebuffer";
            QuadFramebufferSpec.Width = LWindow::Get().GetWidth();
            QuadFramebufferSpec.Height = LWindow::Get().GetHeight();
		    TObjectPtr<LFramebuffer> QuadFramebuffer = LFramebuffer::Create(QuadFramebufferSpec);

			/* Create the quad pipeline. */
            FPipelineSpecification QuadPipelineSpec{};
            QuadPipelineSpec.TargetFramebuffer = nullptr;
            QuadPipelineSpec.DebugName = "Renderer2D-QuadPipeline";
            QuadPipelineSpec.Shader = QuadShader;

            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad pipeline");
            FRenderPassSpecification QuadPassSpec;
            QuadPassSpec.DebugName = "Renderer2D-RenderPass-Quad";
            QuadPassSpec.Pipeline = LPipeline::Create(QuadPipelineSpec);
            //QuadMaterial = LMaterial::Create(QuadShader, "QuadMaterial");

            /* Use correct amount of texture array uniforms. */
            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad renderpass");
            for (uint8_t ArrayIndex = 0; ArrayIndex < Specification.TextureArraysCount; ArrayIndex++)
            {
                QuadPassSpec.Pipeline->BindTextureArray(ArrayIndex);
            }
            QuadPass = LRenderPass::Create(QuadPassSpec);
            
            QuadVertexBuffer = LVertexBuffer::Create(MaxVertices * sizeof(FQuadVertex));
            QuadVertexBuffer->SetLayout({
                { "a_Position",     EShaderDataType::Float3 },
                { "a_Color",        EShaderDataType::Float4 },
                { "a_TexCoord",     EShaderDataType::Float2 },
                { "a_TexIndex",     EShaderDataType::Float, },
                { "a_TexArray",     EShaderDataType::Float, },
                { "a_TilingFactor", EShaderDataType::Float, },
            });

            QuadVertexBufferBase = new FQuadVertex[MaxVertices];
            uint32_t* QuadIndices = new uint32_t[MaxIndices];
            uint32_t Offset = 0;

            for (uint32_t i = 0; i < MaxIndices; i += 6)
            {
				/* First triangle, 0->1->2 */
                QuadIndices[i + 0] = Offset + 0;
                QuadIndices[i + 1] = Offset + 1;
                QuadIndices[i + 2] = Offset + 2;

                /* Second triangle, 2->3->0 */
                QuadIndices[i + 3] = Offset + 2;
                QuadIndices[i + 4] = Offset + 3;
                QuadIndices[i + 5] = Offset + 0;

                Offset += 4;
            }

            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad indexbuffer");
            TObjectPtr<LIndexBuffer> quadIB = LIndexBuffer::Create(QuadIndices, MaxIndices);
            QuadVertexBuffer->SetIndexBuffer(quadIB);
            QuadVertexBufferPtr = QuadVertexBufferBase;

            delete[] QuadIndices;
        }

        /* Lines. */
        {
            LK_CORE_DEBUG_TAG("Renderer2D", "");
            LineVertexBuffer = LVertexBuffer::Create(MaxVertices * sizeof(FLineVertex));
            FVertexBufferLayout LineVertexBufferLayout{ };
            LineVertexBuffer->SetLayout({
                { "a_Pos",       EShaderDataType::Float3, },
                { "a_Color",     EShaderDataType::Float4, },
                { "a_EntityID",  EShaderDataType::Int,    }  /// TODO: FIX THIS
            });
            LineVertexBufferBase = new FLineVertex[MaxVertices];

            uint32_t* LineIndices = new uint32_t[MaxLineIndices];
            for (uint32_t i = 0; i < MaxLineIndices; i++)
            {
		        LineIndices[i] = i;
            }

		    delete[] LineIndices;
        }

        WhiteTexture = LTextureLibrary::Get().GetWhiteTexture();

        CameraBuffer.ViewProjection = glm::mat4(1.0f);
        CameraUniformBuffer = LUniformBuffer::Create(sizeof(FCameraData));
        CameraUniformBuffer->SetBinding(QuadShader, "UB_Camera", 0); // Binding is set default to 0 in Renderer_Quad.shader

        RenderCommandBuffer = LRenderCommandBuffer::Create(0, "Renderer2D_RenderCommandBuffer");

        for (uint32_t i = 0; i < TextureArrays.size(); i++)
        {
            if (TextureArrays[i])
            {
                TextureArrays[i]->Bind();
            }
        }

        bInitialized = true;
    }

    void LRenderer2D::Shutdown() 
    {
    }

    void LRenderer2D::BeginScene(const LSceneCamera& Camera) 
    {
        CameraBuffer.ViewProjection = Camera.GetViewProjectionMatrix();
        CameraUniformBuffer->SetData(&CameraBuffer, sizeof(FCameraData));

        StartBatch();
    }

    void LRenderer2D::BeginScene(const LSceneCamera& Camera, const glm::mat4& Transform) 
    {
        CameraBuffer.ViewProjection = Camera.GetProjectionMatrix() * glm::inverse(Transform);
        CameraUniformBuffer->SetData(&CameraBuffer, sizeof(FCameraData));

        StartBatch();
    }

    void LRenderer2D::EndScene() 
    {
		Flush();
    }

	void LRenderer2D::StartBatch()
	{
        QuadIndexCount = 0;
        QuadVertexBufferPtr = QuadVertexBufferBase;

        LineIndexCount = 0;
        LineVertexBufferPtr = LineVertexBufferBase;
	}

	void LRenderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

    void LRenderer2D::Flush() 
    {
		LK_PROFILE_FUNC();
        const uint32_t FrameIndex = LRenderer::GetCurrentFrameIndex();
        uint32_t DataSize = 0;

        /* Quads. */
        if (QuadIndexCount)
        {
            DataSize = static_cast<uint32_t>((uint8_t*)QuadVertexBufferPtr - (uint8_t*)QuadVertexBufferBase);
            QuadVertexBuffer->SetData(QuadVertexBufferBase, DataSize);

			LRenderer::RenderGeometry(RenderCommandBuffer, 
                                      QuadPass->GetPipeline(), 
                                      QuadShader, 
                                      QuadVertexBuffer, 
                                      QuadIndexBuffer, 
                                      CameraBuffer.ViewProjection, 
                                      QuadIndexCount);

            Statistics.DrawCalls++;
        }
	}


	void LRenderer2D::DrawImage(const TObjectPtr<LImage> Image)
	{
		LK_UNUSED(Image);
	}

	void LRenderer2D::DrawQuad(const glm::mat4& Transform, const glm::vec4& Color)
	{
		LK_UNUSED(Transform); LK_UNUSED(Color);
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color)
	{
		DrawQuad({ Position.x, Position.y, 0.0f }, Size, Color);
	}

	void LRenderer2D::DrawQuad(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color)
	{
		LK_PROFILE_FUNC();

		const float TextureIndex = 0.0f; /* White Texture. */
		const float TilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position) 
			* glm::scale(glm::mat4(1.0f), { Size.x, Size.y, 1.0f });

		FQuadVertex*& BufferPtr = GetWriteableQuadBuffer();
		BufferPtr->Position = transform * QuadVertexPositions[0];
		BufferPtr->Color = Color;
		BufferPtr->TexCoord = { 0.0f, 0.0f };
		BufferPtr->TexIndex = TextureIndex;
		BufferPtr->TilingFactor = TilingFactor;
		BufferPtr++;

		BufferPtr->Position = transform * QuadVertexPositions[1];
		BufferPtr->Color = Color;
		BufferPtr->TexCoord = { 1.0f, 0.0f };
		BufferPtr->TexIndex = TextureIndex;
		BufferPtr->TilingFactor = TilingFactor;
		BufferPtr++;

		BufferPtr->Position = transform * QuadVertexPositions[2];
		BufferPtr->Color = Color;
		BufferPtr->TexCoord = { 1.0f, 1.0f };
		BufferPtr->TexIndex = TextureIndex;
		BufferPtr->TilingFactor = TilingFactor;
		BufferPtr++;

		BufferPtr->Position = transform * QuadVertexPositions[3];
		BufferPtr->Color = Color;
		BufferPtr->TexCoord = { 0.0f, 1.0f };
		BufferPtr->TexIndex = TextureIndex;
		BufferPtr->TilingFactor = TilingFactor;
		BufferPtr++;

		QuadIndexCount += 6;

		Statistics.QuadCount++;
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture)
	{
		LK_UNUSED(Position); LK_UNUSED(Size); LK_UNUSED(Texture);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, 
                               TObjectPtr<LTexture> Texture, const glm::vec4& TintColor)
	{
    #if 0
		LK_UNUSED(Position); LK_UNUSED(Size); LK_UNUSED(Texture); LK_UNUSED(TintColor);
		LK_MARK_FUNC_NOT_IMPLEMENTED();

        const float TextureIndex = 0; 
        const float TilingFactor = 1.0f;
        constexpr size_t QuadVertexCount = 4;

        for (size_t i = 0; i < QuadVertexCount; i++)
        {
            QuadVertexBufferPtr->Position = Transform * QuadVertexPositions[i];
            QuadVertexBufferPtr->TexCoord = TextureCoords[i];
            QuadVertexBufferPtr->Color = Color;
            QuadVertexBufferPtr->TexIndex = TextureIndex; // White texture
            QuadVertexBufferPtr->TexArray = 0;            // White texture
            QuadVertexBufferPtr->TilingFactor = TilingFactor;
            QuadVertexBufferPtr++;
        }

        m_QuadIndexCount += 6;
        m_Stats.QuadCount++;
    #endif

        DrawQuad({ Position.x, Position.y, 0.0f }, Size, Texture, TintColor);
    }

	void LRenderer2D::DrawQuad(const glm::vec3& Position, const glm::vec2& Size, 
                               TObjectPtr<LTexture> Texture, const glm::vec4& TintColor)
	{
		LK_PROFILE_FUNC();

        if (QuadIndexCount >= MaxIndices)
        {
            NextBatch();
        }

        static constexpr std::size_t QuadVertexCount = 4;
        static constexpr float TilingFactor = 1.0f;

        float TextureIndex = 0;
        float TextureArrayIndex = 0;

        for (uint32_t i = 0; i < TextureArrays.size(); i++)
        {
            if (TextureArrays[i] && TextureArrays[i]->HasTexture(Texture))
            {
                TextureIndex = TextureArrays[i]->GetIndexOfTexture(Texture);
                TextureArrayIndex = static_cast<uint32_t>(i);
                break;
            }
        }

        const glm::mat4 Transform = glm::translate(glm::mat4(1.0f), { Position.x, Position.y, Position.z})
            * glm::scale(glm::mat4(1.0f), { Size.x, Size.y, 1.0f });

        for (std::size_t i = 0; i < QuadVertexCount; i++)
        {
            QuadVertexBufferPtr->Position = Transform * QuadVertexPositions[i];
            QuadVertexBufferPtr->Color = TintColor;
            QuadVertexBufferPtr->TexCoord = TextureCoords[i];
            QuadVertexBufferPtr->TexIndex = TextureIndex;
            QuadVertexBufferPtr->TexArray = TextureArrayIndex;
            QuadVertexBufferPtr->TilingFactor = TilingFactor;
            QuadVertexBufferPtr++;
        }

        QuadIndexCount += 6;
        Statistics.QuadCount++;
	}

    float LRenderer2D::GetLineWidth() 
    {
        return 2.0f;
    }

    void LRenderer2D::SetLineWidth(const float InWidth) 
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED("SetLineWidth failed, function not implemented, width: {}", InWidth);
	}

}
