#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	constexpr int MaxTexturesArrays = 10;
    constexpr int MaxTexturesPerTextureArray = 32;

	struct RendererData
	{
        RendererCapabilities m_RendererCapabilities;

		Ref<OpenGLTextureArray> TextureArrays[MaxTexturesArrays];
	};

	static RendererData* Data = nullptr;
	static int TextureArrayCount = 0;

    void OpenGLRenderer::Init()
    {
		Data = new RendererData();

		m_RenderContext = Window::Get().GetRenderContext().As<OpenGLContext>();

		// Texture Arrays
		constexpr TextureArrayDimension TextureArrayDimensions[] = { 
			Dimension_512x512, 
			Dimension_1024x1024, 
			Dimension_2048x2048 
		};

		TextureArraySpecification textureArraySpec;
		for (int i = 0; i < LK_ARRAYSIZE(TextureArrayDimensions); i++)
		{
			textureArraySpec.Format = ImageFormat::RGBA;
			textureArraySpec.TextureSlot = TextureArrayCount;
			textureArraySpec.Dimension = TextureArrayDimensions[i];
			textureArraySpec.DebugName = "TextureArray-" + Utils::TextureArrayDimensionToString(TextureArrayDimensions[i]);
			Data->TextureArrays[TextureArrayCount++] = Ref<OpenGLTextureArray>::Create(textureArraySpec);
			LK_CORE_DEBUG_TAG("OpenGLRenderer", "Created texture array {} called \"{}\"", i, textureArraySpec.DebugName);
		}

        auto textures2D = Renderer::GetTextureLibrary()->GetTextures2D();
        for (int i = 0; i < textures2D.size(); i++)
        {
			Ref<OpenGLImage2D> img = textures2D[i].second->GetImage().As<OpenGLImage2D>();
			switch (img->GetWidth())
			{
				case 512:  GetTextureArray(0)->AddTextureToArray(textures2D[i].second); break;
				case 1024: GetTextureArray(1)->AddTextureToArray(textures2D[i].second); break;
				case 2048: GetTextureArray(2)->AddTextureToArray(textures2D[i].second); break;
			}
        }

		OpenGLRenderer2DSpecification renderer2DSpec;
		renderer2DSpec.TextureArraysUsed = TextureArrayCount;
		m_Renderer2D = Ref<OpenGLRenderer2D>::Create(renderer2DSpec);

		// Add texture array references to OpenGLRenderer2D as well
		for (int i = 0; i < OpenGLRenderer2D::MaxTextureArrays; i++)
		{
			if (Data->TextureArrays[i])
			{
				m_Renderer2D->m_TextureArrays[i] = Data->TextureArrays[i];
				BindTextureArray(i);
			}
		}
		m_Renderer2D->Init();

		m_RenderContext->SetDepthEnabled(true);

		SetPrimitiveTopology(RenderTopology::Triangles);

		// Setup debugging stuff
		SetupTexturesAndShaders();
        {
            GenerateCubeVaoAndVbo(CubeVAO, CubeVBO);
            GeneratePlaneVaoAndVbo(PlaneVAO, PlaneVBO);
            GenerateScreenQuadVaoAndVbo(QuadVAO, QuadVBO);
            CubeTexture_ = LoadTexture("Assets/Textures/container.jpg");
            FloorTexture_ = LoadTexture("Assets/Textures/metal.png");
        }
    }

    void OpenGLRenderer::Shutdown()
    {
		m_RenderPass->Terminate();
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

	void OpenGLRenderer::SetPrimitiveTopology(const RenderTopology& mode)
	{
		switch (mode)
		{
			case RenderTopology::Lines:
				m_Topology = GL_LINES;
				m_Renderer2D->m_Topology = GL_LINES;
				break;
			case RenderTopology::Triangles:
				m_Topology = GL_TRIANGLES;
				m_Renderer2D->m_Topology = GL_TRIANGLES;
				break;
		}
	}

	void OpenGLRenderer::SetDepthFunction(const DepthFunction& depthFunc)
	{
		m_RenderContext->SetDepthFunction(depthFunc);
	}

	void OpenGLRenderer::Draw(VertexBuffer& vb, const Shader& shader)
	{
		shader.Bind();
		glDrawElements(m_Topology, vb.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		glDrawElements(m_Topology, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitMesh(Ref<Mesh>& mesh, Ref<Shader>& shader, const glm::mat4& transform)
	{
		Renderer::Submit([&] 
		{
			MeshSource& source = *mesh->GetMeshSource();
			VertexBuffer& vb = *source.GetVertexBuffer();
			IndexBuffer& ib = *vb.GetIndexBuffer();

			shader->Bind();

			vb.Bind();
			glDrawElements(m_Topology, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
			//glDrawElements(GLUtils::PrimitiveTopologyToOpenGL(m_Topology), ib.GetCount(), GL_UNSIGNED_INT, ib.GetLocalData().Data);
		});
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
		glDrawElements(m_Topology, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawIndexed(uint64_t indexCount)
	{
		glDrawElements(m_Topology, indexCount, GL_UNSIGNED_INT, nullptr);
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
		std::deque<RendererID> boundTextureArrays = pipeline->GetBoundTextureArrays();
		auto& framebuffer = Renderer::GetViewportFramebuffer();
		Renderer::Submit([this, pipeline, framebuffer, shader, vertexBuffer, transform, indexCount, boundTextureArrays]() mutable
		{
			framebuffer->Bind();
			shader->Bind();

			int i = 1;
			for (RendererID& textureArray : boundTextureArrays)
			{
				shader->Set("u_TextureArray" + std::to_string(i++), textureArray);
			}
			
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

	Ref<OpenGLTextureArray> OpenGLRenderer::GetTextureArray(int idx)
	{
		return Data->TextureArrays[idx];
	}

	void OpenGLRenderer::BindTextureArray(int idx)
	{
		TextureArray& textureArray = *Data->TextureArrays[idx];
		textureArray.Bind();
	}

	void OpenGLRenderer::BindTextureArray(const TextureArrayDimension& dimension)
	{
		OpenGLTextureArray& textureArray = *GetTextureArrayWithDimension(dimension);
		textureArray.Bind();
	}

	Ref<OpenGLTextureArray> OpenGLRenderer::GetTextureArrayWithDimension(const TextureArrayDimension& dimension)
	{
		for (auto& textureArray : Data->TextureArrays)
		{
			if (textureArray->m_Specification.Dimension == dimension)
				return textureArray;
		}
		LK_CORE_ASSERT(false, "Couldn't find TextureArray with dimension {}...", (int)dimension);
	}

}
