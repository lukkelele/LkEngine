#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"


namespace LkEngine {

	struct RendererData
	{
        RendererCapabilities m_RendererCapabilities;

		Ref<TextureArray> TextureArrays[10];
	};

	static RendererData* Data = nullptr;
	static RendererID TEXTURE_ARRAY;

	// TODO: Move all texture init here
    void OpenGLRenderer::Init()
    {
		Data = new RendererData();

        int width = 200; 
        int height = 200;
        static int MaxTexturesPerTextureArray = 32;

        // Create textures        
        TextureSpecification textureSpec;
        textureSpec.Format = ImageFormat::RGBA32F;
        textureSpec.Width = 200;
        textureSpec.Height = 200;
		textureSpec.SamplerWrap = TextureWrap::Clamp;
		textureSpec.SamplerFilter = TextureFilter::Nearest;

        // Åle texture 
		{
			textureSpec.Name = "ale";
			textureSpec.DebugName = "ale";
			textureSpec.Path = "assets/Textures/misc/ale_500x375.png";
			//textureSpec.Path = "asset/Textures/misc/ale_200x200.png";
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}
        // Atte texture
		{
			textureSpec.Name = "atte";
			textureSpec.Path = "assets/Textures/misc/atte_200x200.png";
			textureSpec.DebugName = "atte";
			textureSpec.SamplerWrap = TextureWrap::Repeat;
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}
        // Ground texture
		{
			textureSpec.Name = "SuperMario-ground_block";
			textureSpec.Path = "assets/Textures/SuperMario/SuperMario-ground_block.png";
			textureSpec.DebugName = "groundblock";
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}
        // Nerd Emoji
		{
			textureSpec.Name = "NerdEmoji";
			textureSpec.DebugName = "NerdEmoji";
			textureSpec.Path = "assets/Textures/misc/nerdEmoji.jpg";
			textureSpec.Format = ImageFormat::RGBA32F;
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}

#if 0
		GL_CALL(glGenTextures(1, &TEXTURE_ARRAY));
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, TEXTURE_ARRAY));

        GL_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, MaxTexturesPerTextureArray, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

        //GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 10));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
#endif

		//------------------------------------------------
		// Create texture arrays
		//------------------------------------------------
		TextureArraySpecification textureArraySpec;
		// Texture Array 0, 200x200
		{
			textureArraySpec.TextureSlot = 0;
			textureArraySpec.Dimension = Dimension_200x200;
			Data->TextureArrays[0] = Ref<TextureArray>::Create(textureArraySpec);
		}
		// Texture Array 1, 400x400
		{
			textureArraySpec.Dimension = TextureArrayDimension::Dimension_400x400;
			textureArraySpec.TextureSlot = 1;
			Data->TextureArrays[1] = Ref<TextureArray>::Create(textureArraySpec);
		}
		// Texture Array 2, 800x800
		{
			textureArraySpec.Dimension = TextureArrayDimension::Dimension_800x800;
			textureArraySpec.TextureSlot = 2;
			Data->TextureArrays[2] = Ref<TextureArray>::Create(textureArraySpec);
		}
		// Texture Array 3, 1024x1024
		{
			textureArraySpec.Dimension = TextureArrayDimension::Dimension_1024x1024;
			textureArraySpec.TextureSlot = 3;
			Data->TextureArrays[3] = Ref<TextureArray>::Create(textureArraySpec);
		}

        auto textures2D = TextureLibrary::Get()->GetTextures2D();
        for (int i = 0; i < textures2D.size(); i++)
        {
			// Check the size of texture and add it to its texture array
			//auto& texture = textures2D[i].second;
			Ref<OpenGLImage2D> img = textures2D[i].second->GetImage().As<OpenGLImage2D>();

			LK_CORE_DEBUG_TAG("OpenGLRenderer", "Adding texture to {}x{} texture array: {}", img->GetWidth(), img->GetHeight(), img->GetSpecification().Name);
			switch (img->GetWidth())
			{
				case 200:  // 200x200
					BindTextureArray(Dimension_200x200);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 200, 200, 1, GL_RGBA, GL_UNSIGNED_BYTE, img->GetBuffer().Data);
					break;
				case 400:  // 400x400
					BindTextureArray(Dimension_400x400);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 400, 400, 1, GL_RGBA, GL_UNSIGNED_BYTE, img->GetBuffer().Data);
					break;
				case 800:  // 800x800
					BindTextureArray(Dimension_800x800);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 800, 800, 1, GL_RGBA, GL_UNSIGNED_BYTE, img->GetBuffer().Data);
					break;
				case 1024: // 1024x1024
					BindTextureArray(Dimension_1024x1024);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE, img->GetBuffer().Data);
					break;
			}
        }

		Renderer2DSpecification renderer2DSpecification;
		Ref<OpenGLRenderer2D> renderer2D = Ref<OpenGLRenderer2D>::Create(renderer2DSpecification);

		// Add texture array references to OpenGLRenderer2D as well
		for (int i = 0; i < 10; i++)
		{
			if (Data->TextureArrays[i])
			{
				renderer2D->m_TextureArrays[i++] = Data->TextureArrays[i];
				LK_CORE_DEBUG_TAG("OpenGLRenderer", "Added texture array to OpenGLRenderer2D, number {}", i);
			}
		}
		m_Renderer2D = renderer2D;
		m_Renderer2D->Init();

		// Setup debugging stuff
		SetupTexturesAndShaders();

		GraphicsContext::Get()->SetDepthEnabled(true);

		SetDrawMode(RendererDrawMode::Triangles);
    }

    void OpenGLRenderer::Shutdown()
    {
    }

    void OpenGLRenderer::BeginFrame()
    {
		Clear();
    }

    void OpenGLRenderer::EndFrame()
    {
    }

	void OpenGLRenderer::Clear()
	{
		Framebuffer::TargetSwapChain();
		glClearColor(Renderer::ClearColor.r, Renderer::ClearColor.g, Renderer::ClearColor.b, Renderer::ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& viewportFramebuffer = *Renderer::GetViewportFramebuffer();
		viewportFramebuffer.Bind();
		viewportFramebuffer.Clear();
		viewportFramebuffer.Unbind();
	}

	void OpenGLRenderer::SetDrawMode(const RendererDrawMode& mode)
	{
		switch (mode)
		{
			case RendererDrawMode::Lines:
				m_DrawMode = GL_LINES;
				m_Renderer2D->m_DrawMode = GL_LINES;
				break;
			case RendererDrawMode::Triangles:
				m_DrawMode = GL_TRIANGLES;
				m_Renderer2D->m_DrawMode = GL_TRIANGLES;
				break;
		}
	}

	void OpenGLRenderer::Draw(VertexBuffer& vb, const Shader& shader)
	{
		glDrawElements(m_DrawMode, vb.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		glDrawElements(m_DrawMode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitImage(const Ref<Image> image)
	{
		m_Renderer2D->DrawImage(image);
	}

	void OpenGLRenderer::SubmitImage(const Ref<Image2D> image)
	{
		m_Renderer2D->DrawImage(image);
	}

	void OpenGLRenderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawLine(p1, p2, color, entityID);
	}

	void OpenGLRenderer::SubmitIndexed(unsigned int indexCount)
	{
		glDrawElements(m_DrawMode, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawIndexed(uint64_t indexCount)
	{
		glDrawElements(m_DrawMode, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
		//Renderer2D::DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, tintColor, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, texture, tintColor, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, texture, rotation, entityID);
	}

	RendererCapabilities& OpenGLRenderer::GetCapabilities()
	{
		return Data->m_RendererCapabilities;
	}

	void OpenGLRenderer::RenderGeometry(Ref<RenderCommandBuffer> _renderCommandBuffer, Ref<Pipeline> _pipeline, Ref<VertexBuffer> _vertexBuffer, Ref<IndexBuffer> _indexBuffer, const glm::mat4& transform, uint32_t indexCount /* == 0*/)
	{
		Ref<OpenGLPipeline> pipeline = _pipeline.As<OpenGLPipeline>();
		Ref<OpenGLVertexBuffer> vertexBuffer = _vertexBuffer.As<OpenGLVertexBuffer>();
		Ref<OpenGLShader> shader = pipeline->GetShader();

		Renderer::Submit([_renderCommandBuffer, pipeline, vertexBuffer, _indexBuffer, transform, indexCount]() mutable
		{
		});
	}

	void OpenGLRenderer::RenderGeometry(Ref<RenderCommandBuffer> _renderCommandBuffer, Ref<Pipeline> _pipeline, Ref<Material> _material, Ref<VertexBuffer> _vertexBuffer, Ref<IndexBuffer> _indexBuffer, const glm::mat4& transform, uint32_t indexCount /* == 0*/)
	{
		LK_CORE_ASSERT(false, "Not implemented");
	}

	// TODO: It is required to set the framebuffer in this submission call weirdly enough, even if its set per default in RenderCommandQueue
	//       Weird bug
	void OpenGLRenderer::RenderGeometry(Ref<RenderCommandBuffer> _renderCommandBuffer, Ref<Pipeline> pipelineRef, Ref<Shader>& shader, Ref<VertexBuffer>& vertexBuffer, Ref<IndexBuffer>& _indexBuffer, const glm::mat4& transform, uint32_t indexCount /* == 0*/)
	{
		Ref<OpenGLPipeline> pipeline = pipelineRef.As<OpenGLPipeline>();
		uint32_t textureArray = pipeline->GetBoundTextureArray();
		auto& framebuffer = Renderer::GetViewportFramebuffer();
		Renderer::Submit([this, pipeline, framebuffer, shader, vertexBuffer, transform, indexCount, textureArray]() mutable
		{
			framebuffer->Bind();
			//framebuffer->BindTexture(0); // TODO: bind texture array idx here ?
			shader->Bind();
			shader->Set("u_TextureArray", textureArray);
            shader->Set("u_ViewProj", transform);
			
			vertexBuffer->Bind();
			DrawIndexed(indexCount);

			framebuffer->Unbind();
		});
	}

	void OpenGLRenderer::BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear)
	{
		auto commandBuffer = renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
		auto pass = renderPass.As<OpenGLRenderPass>();
		auto pipeline = pass->GetPipeline().As<OpenGLPipeline>();
		auto shader = pipeline->GetShader().As<OpenGLShader>();
	}

	void OpenGLRenderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
		auto commandBuffer = renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
	}

	Ref<TextureArray> OpenGLRenderer::GetTextureArray(int idx)
	{
		return Data->TextureArrays[idx];
	}

	void OpenGLRenderer::BindTextureArray(int idx)
	{
		TextureArray& textureArray = *Data->TextureArrays[idx];
		glActiveTexture(GL_TEXTURE0 + textureArray.GetTextureSlot());
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray.GetRendererID());
	}

	void OpenGLRenderer::BindTextureArray(const TextureArrayDimension& dimension)
	{
		Ref<TextureArray> textureArray = GetTextureArrayWithDimension(dimension);
		textureArray->Bind();
	}

	void OpenGLRenderer::UnbindTextureArray(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	Ref<TextureArray> OpenGLRenderer::GetTextureArrayWithDimension(const TextureArrayDimension& dimension)
	{
		for (auto& textureArray : Data->TextureArrays)
		{
			if (textureArray->m_Specification.Dimension == dimension)
				return textureArray;
		}
		LK_CORE_ASSERT(false, "Couldn't find TextureArray with dimension {}...", (int)dimension);
	}

}
