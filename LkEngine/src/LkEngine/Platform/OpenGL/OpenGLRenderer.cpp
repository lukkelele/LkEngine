#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"


namespace LkEngine {

	constexpr uint8_t MaxTexturesArrays = 10;

	struct RendererData
	{
        RendererCapabilities m_RendererCapabilities;

		Ref<TextureArray> TextureArrays[MaxTexturesArrays];
	};

	static RendererData* Data = nullptr;

	// TODO: Move all texture init here
    void OpenGLRenderer::Init()
    {
		Data = new RendererData();

        static int MaxTexturesPerTextureArray = 32;

        // Create textures        
        TextureSpecification textureSpec;
        textureSpec.Format = ImageFormat::RGBA32F;
        textureSpec.Width = 2048;
        textureSpec.Height = 2048;
		textureSpec.SamplerWrap = TextureWrap::Clamp;
		textureSpec.SamplerFilter = TextureFilter::Nearest;

		{
			textureSpec.Name = "container";
			textureSpec.DebugName = "container";
			textureSpec.Width = 2048;
			textureSpec.Height = 2048;
			textureSpec.Format = ImageFormat::RGBA;
			textureSpec.Path = "assets/Textures/container.jpg";
			TextureLibrary::Get()->AddTexture2D(textureSpec);

			textureSpec.Name = "bricks";
			textureSpec.DebugName = "bricks";
			textureSpec.Path = "assets/Textures/bricks_orange.jpg";
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}

        // Åle texture
		{
			textureSpec.Name = "ale1024";
			textureSpec.DebugName = "ale1024";
			textureSpec.Width = 2048;
			textureSpec.Height = 2048;
			textureSpec.Path = "assets/Textures/Misc/ale_1024x1024.png";
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}
        // Lukas texture
		{
			textureSpec.Width = 2048;
			textureSpec.Height = 2048;
			textureSpec.Name = "lukas_1024";
			textureSpec.Path = "assets/Textures/Misc/lukas_1024.jpg";
			textureSpec.DebugName = "lukas-1024x1024";
			textureSpec.SamplerWrap = TextureWrap::Repeat;
			TextureLibrary::Get()->AddTexture2D(textureSpec);
		}
        // Texture: metal
        {
		    textureSpec.Format = ImageFormat::RGBA32F;
		    textureSpec.Width = 2048;
		    textureSpec.Height = 2048;
            textureSpec.Name = "metal-ground";
            textureSpec.DebugName = "metal-ground";
            textureSpec.Path = "assets/Textures/metal.png";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary::Get()->AddTexture2D(textureSpec);
        }

        // Texture: wood
        {
		    textureSpec.Format = ImageFormat::RGBA32F;
		    textureSpec.Width = 2048;
		    textureSpec.Height = 2048;
            textureSpec.Name = "wood";
            textureSpec.DebugName = "wood";
            textureSpec.Path = "assets/Textures/wood.png";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			TextureLibrary::Get()->AddTexture2D(textureSpec);
        }

        // Skybox
        {
            TextureSpecification skyboxSpec;
            skyboxSpec.Width = 2048;
            skyboxSpec.Height = 2048;
            skyboxSpec.Name = "skybox-ice-back";
            skyboxSpec.DebugName = "skybox-ice-back";
            skyboxSpec.Path = "assets/Textures/Skybox/back.jpg";
            skyboxSpec.GenerateMips = false;
            skyboxSpec.Format = ImageFormat::RGBA32F;
            skyboxSpec.SamplerFilter = TextureFilter::Nearest;
            skyboxSpec.SamplerWrap = TextureWrap::Clamp;
			TextureLibrary::Get()->AddTexture2D(skyboxSpec);
        }


		//------------------------------------------------
		// Create texture arrays
		//------------------------------------------------
		TextureArraySpecification textureArraySpec;
		// Texture Array, 2048x2048
		{
			textureArraySpec.Dimension = TextureArrayDimension::Dimension_2048x2048;
			textureArraySpec.TextureSlot = 0;
			textureArraySpec.DebugName = "TextureArray-2048x2048";
			textureArraySpec.Format = ImageFormat::RGBA32F;
			Data->TextureArrays[0] = Ref<TextureArray>::Create(textureArraySpec);
		}

        auto textures2D = TextureLibrary::Get()->GetTextures2D();
        for (int i = 0; i < textures2D.size(); i++)
        {
			Ref<OpenGLImage2D> img = textures2D[i].second->GetImage().As<OpenGLImage2D>();
			LK_CORE_DEBUG_TAG("OpenGLRenderer", "Adding texture {} to {}x{} texture array", img->GetSpecification().Name, img->GetWidth(), img->GetHeight());
			switch (img->GetWidth())
			{
				case 2048: GetTextureArray(0)->AddTextureToArray(textures2D[i].second); break;
				//case 512:  GetTextureArray(0)->AddTextureToArray(textures2D[i].second); break;
				//case 1024: GetTextureArray(1)->AddTextureToArray(textures2D[i].second); break;
			}
        }

		Renderer2DSpecification renderer2DSpecification;
		Ref<OpenGLRenderer2D> renderer2D = Ref<OpenGLRenderer2D>::Create(renderer2DSpecification);

		// Add texture array references to OpenGLRenderer2D as well
		int addedTextureArrays = 0;
		for (int i = 0; i < 10; i++)
		{
			if (Data->TextureArrays[i])
			{
				renderer2D->m_TextureArrays[i] = Data->TextureArrays[i];
				LK_CORE_DEBUG_TAG("OpenGLRenderer", "Added texture array {}x{} to OpenGLRenderer2D, number {}", Data->TextureArrays[i]->GetWidth(), Data->TextureArrays[i]->GetHeight(), i);
				addedTextureArrays++;
			}
		}
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Added {} texture arrays to OpenGLRenderer2D", addedTextureArrays);
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
			shader->Set("u_TextureArray1", textureArray);
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
