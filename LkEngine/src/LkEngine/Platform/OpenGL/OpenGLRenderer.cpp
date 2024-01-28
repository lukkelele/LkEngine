#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"


namespace LkEngine {

	struct RendererData
	{
        RendererCapabilities m_RendererCapabilities;
	};

	static RendererData* Data = nullptr;

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

        // Åle texture 
        textureSpec.Name = "ale";
        textureSpec.Path = "assets/textures/misc/ale_200x200.png";
        textureSpec.SamplerWrap = TextureWrap::Clamp;
        textureSpec.SamplerFilter = TextureFilter::Nearest;
        textureSpec.DebugName = "ale";
        TextureLibrary::Get()->AddTexture2D(textureSpec);

        // Atte texture
        textureSpec.Name = "atte";
        textureSpec.Path = "assets/textures/misc/atte_200x200.png";
        textureSpec.DebugName = "atte";
        TextureLibrary::Get()->AddTexture2D(textureSpec);

        // Ground texture
        textureSpec.Name = "SuperMario-ground_block";
        textureSpec.Path = "assets/textures/SuperMario/SuperMario-ground_block.png";
        textureSpec.DebugName = "groundblock";
        TextureLibrary::Get()->AddTexture2D(textureSpec);

        textureSpec.Name = "sky-background-2d";
        textureSpec.Path = "assets/textures/sky-background-2d.png";
        textureSpec.SamplerFilter = TextureFilter::Linear;
        textureSpec.DebugName = "sky";
        TextureLibrary::Get()->AddTexture2D(textureSpec);

        textureSpec.Name = "SuperMario-gun";
        textureSpec.Path = "assets/textures/SuperMario/SuperMario-gun.png";
        textureSpec.SamplerFilter = TextureFilter::Linear;
        textureSpec.DebugName = "gun";
        TextureLibrary::Get()->AddTexture2D(textureSpec);

        GL_CALL(glGenTextures(1, &m_TextureArray_200x200));
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray_200x200));

        GL_CALL(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, MaxTexturesPerTextureArray, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

        //GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 10));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        auto textures2D = TextureLibrary::Get()->GetTextures2D();
        for (int i = 0; i < textures2D.size(); i++)
        {
			auto& texture = textures2D[i].second;
			LK_CORE_DEBUG_TAG("Texture", "Added to sampler2DArray -> {}", texture->GetName());
			GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, textures2D[i].second->GetImage()->GetBuffer().Data));
        }

		Renderer2DSpecification renderer2DSpecification;
		//renderer2DSpecification.SwapChainTarget = true;

		m_Renderer2D = Ref<OpenGLRenderer2D>::Create(renderer2DSpecification);
		m_Renderer2D->Init();
		LK_VERIFY(m_Renderer2D, "OpenGLRenderer2D (Renderer 2D API) is nullptr!");

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray_200x200);
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
		auto& c = Renderer::ClearColor;
		glClearColor(c.r, c.g, c.b, c.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& renderer2DFramebuffer = *m_Renderer2D->GetFramebuffer().As<OpenGLFramebuffer>();
		renderer2DFramebuffer.Bind();
		renderer2DFramebuffer.Clear();
		renderer2DFramebuffer.Unbind();
	}

	void OpenGLRenderer::SetDrawMode(const RendererDrawMode& mode)
	{
		switch (mode)
		{
			case RendererDrawMode::Lines:
				m_DrawMode = GL_LINES;
				break;
			case RendererDrawMode::Triangles:
				m_DrawMode = GL_TRIANGLES;
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
		Ref<OpenGLPipeline> pipeline = _pipeline.As<OpenGLPipeline>();
		//Ref<OpenGLShader> shader = _shader.As<OpenGLShader>();
		Ref<OpenGLShader> shader = pipeline->GetShader();
		Ref<OpenGLVertexBuffer> vertexBuffer = _vertexBuffer.As<OpenGLVertexBuffer>();
		Ref<OpenGLRenderCommandBuffer> renderCommandBuffer = _renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
		Ref<OpenGLFramebuffer> framebuffer = pipeline->GetSpecification().TargetFramebuffer.As<OpenGLFramebuffer>();

		//LK_CORE_DEBUG_TAG("OpenGLRenderer", "RenderGeometry: Framebuffer ID={}", framebuffer->GetRendererID());
		Renderer::Submit([&]
		{
			framebuffer->Bind();
			shader->Bind();
			vertexBuffer->Bind();
			DrawIndexed(indexCount);
			framebuffer->Unbind();
		});
	}

	void OpenGLRenderer::RenderGeometry(Ref<RenderCommandBuffer> _renderCommandBuffer, Ref<Pipeline> _pipeline, Ref<Shader> _shader, Ref<VertexBuffer> _vertexBuffer, Ref<IndexBuffer> _indexBuffer, const glm::mat4& transform, uint32_t indexCount /* == 0*/)
	{
		Ref<OpenGLPipeline> pipeline = _pipeline.As<OpenGLPipeline>();
		Ref<OpenGLShader> shader = _shader.As<OpenGLShader>();
		Ref<OpenGLVertexBuffer> vertexBuffer = _vertexBuffer.As<OpenGLVertexBuffer>();
		Ref<OpenGLRenderCommandBuffer> renderCommandBuffer = _renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
		Ref<OpenGLFramebuffer> framebuffer = pipeline->GetSpecification().TargetFramebuffer.As<OpenGLFramebuffer>();

		Renderer::Submit([this, renderCommandBuffer, framebuffer, shader, vertexBuffer, indexCount]() 
		{
			framebuffer->Bind();
			shader->Bind();
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

}
