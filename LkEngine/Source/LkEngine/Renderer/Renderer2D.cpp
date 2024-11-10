#include "LKpch.h"
#include "Renderer2D.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Scene/SceneCamera.h"
#include "LkEngine/Platform/OpenGL/OpenGLRenderer2D.h" // @todo: REMOVE


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
	}

	void LRenderer2D::Initialize()
    {
		/* Shaders. */
		QuadShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Quad");
		LineShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Line");

        /* Quads. */
        {
            LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "Creating quad framebuffer");
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

            QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; 
            QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
            QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
            QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

            // Quad Pipeline
            FPipelineSpecification QuadPipelineSpec{};
            QuadPipelineSpec.TargetFramebuffer = nullptr;
            QuadPipelineSpec.DebugName = "Renderer2D-QuadPipeline";
            QuadPipelineSpec.Shader = QuadShader;

            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad pipeline");
            RenderPassSpecification QuadPassSpec;
            QuadPassSpec.DebugName = "Renderer2D-QuadPass";
            QuadPassSpec.Pipeline = LPipeline::Create(QuadPipelineSpec);
            //QuadMaterial = LMaterial::Create(QuadShader, "QuadMaterial");

        /* TODO: Need some way to invoke this for OpenGL in abstract way. */
        #if 0
            // Use correct amount of texture array uniforms
            LK_CORE_DEBUG_TAG("Renderer2D", "Creating quad renderpass");
            for (uint8_t ArrayIndex = 0; ArrayIndex < Specification.TextureArraysUsed; ArrayIndex++)
            {
                //OpenGLPipeline->BindTextureArray(ArrayIndex);
            }
        #endif
            QuadPass = LRenderPass::Create(QuadPassSpec);
            
            QuadVertexBuffer = LVertexBuffer::Create(MaxVertices * sizeof(FQuadVertex));
            QuadVertexBuffer->SetLayout({
                { "a_Position",       ShaderDataType::Float3  },
                { "a_Color",          ShaderDataType::Float4  },
                { "a_Texcoord",       ShaderDataType::Float2  },
                { "a_TexIndex",       ShaderDataType::Float,  },
                { "a_TexArray",       ShaderDataType::Float,  },
                { "a_TilingFactor",   ShaderDataType::Float,  },
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
            LK_CORE_DEBUG_TAG("OpenGLRenderer2D", "");
            LineVertexBuffer = LVertexBuffer::Create(MaxVertices * sizeof(FLineVertex));
            VertexBufferLayout lineVertexBufLayout{ };
            LineVertexBuffer->SetLayout({
                { "a_Pos",       ShaderDataType::Float3, },
                { "a_Color",     ShaderDataType::Float4, },
                { "a_EntityID",  ShaderDataType::Int,  }
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

        /* TODO: Need some way to invoke this for OpenGL in abstract way. */
        #if 0
        for (uint32_t i = 0; i < TextureArrays.size(); i++)
        {
            if (TextureArrays[i])
            {
                TextureArrays[i]->Bind();
            }
        }
        #endif

        bInitialized = true;
    }

    void LRenderer2D::Shutdown() 
    {
    }

    void LRenderer2D::BeginScene(const LSceneCamera& Camera) 
    {
		LK_UNUSED(Camera);
        //Renderer2D->BeginScene(Camera);
    }

    void LRenderer2D::BeginScene(const LSceneCamera& Camera, const glm::mat4& Transform) 
    {
		LK_UNUSED(Camera);
		LK_UNUSED(Transform);
        //Renderer2D->BeginScene(Camera, Transform);
    }

    void LRenderer2D::EndScene() 
    {
        //Renderer2D->EndScene();
    }

    void LRenderer2D::Flush() 
    {
        //Renderer2D->Flush();
	}

	void LRenderer2D::DrawImage(const TObjectPtr<LImage> Image)
	{
		LK_UNUSED(Image);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer2D::DrawQuad(const glm::mat4& Transform, const glm::vec4& Color)
	{
		LK_UNUSED(Transform);
		LK_UNUSED(Color);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color)
	{
		DrawQuad({ Position.x, Position.y, 0.0f }, Size, Color);
	}

	void LRenderer2D::DrawQuad(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color)
	{
		const float TextureIndex = 0.0f; // White Texture
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

		DrawStatistics.QuadCount++;
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture)
	{
		LK_UNUSED(Position); LK_UNUSED(Size); LK_UNUSED(Texture);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer2D::DrawQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture, const glm::vec4& TintColor)
	{
		LK_UNUSED(Position); LK_UNUSED(Size); LK_UNUSED(Texture); LK_UNUSED(TintColor);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

    float LRenderer2D::GetLineWidth() 
    {
        return 0.0f;
    }

    void LRenderer2D::SetLineWidth(const float InWidth) 
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED("SetLineWidth failed, function not implemented, width: {}", InWidth);
	}

	FQuadVertex*& LRenderer2D::GetWriteableQuadBuffer()
	{
		return QuadVertexBufferPtr;
	}

	FLineVertex*& LRenderer2D::GetWriteableLineBuffer()
	{
		return LineVertexBufferPtr;
	}

}
