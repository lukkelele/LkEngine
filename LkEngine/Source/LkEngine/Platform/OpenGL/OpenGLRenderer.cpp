#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	constexpr int MaxTexturesArrays = 10;
	constexpr int MaxTexturesPerTextureArray = 32;

	struct FRendererData
	{
		RendererCapabilities m_RendererCapabilities;

		TObjectPtr<OpenGLTextureArray> TextureArrays[MaxTexturesArrays];
	};

	static FRendererData* RendererData = nullptr;
	static int TextureArrayCount = 0;


	OpenGLRenderer::OpenGLRenderer()
	{
		LCLASS_REGISTER();
	}

	void OpenGLRenderer::Initialize()
	{
		RendererData = new FRendererData();

		RenderContext = LWindow::Get().GetRenderContext().As<LOpenGLContext>();

		// Texture Arrays
		static constexpr ETextureArrayDimension TextureArrayDimensions[] = {
			Dimension_512x512,
			Dimension_1024x1024,
			Dimension_2048x2048
		};

		FTextureArraySpecification TextureArraySpec;
		for (int i = 0; i < LK_ARRAYSIZE(TextureArrayDimensions); i++)
		{
			TextureArraySpec.ImageFormat = EImageFormat::RGBA;
			TextureArraySpec.TextureSlot = TextureArrayCount;
			TextureArraySpec.TextureArrayDimension = TextureArrayDimensions[i];
			TextureArraySpec.DebugName = "TextureArray-" + Utils::TextureArrayDimensionToString(TextureArrayDimensions[i]);
			RendererData->TextureArrays[TextureArrayCount++] = TObjectPtr<OpenGLTextureArray>::Create(TextureArraySpec);
			LK_CORE_TRACE_TAG("OpenGLRenderer", "Created texture array {} called \"{}\"", i, TextureArraySpec.DebugName);
		}

		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Adding textures to TextureArray's");
		const std::vector<TTexture2DPair>& Textures2D = LAssetManager::GetTextures2D();
		for (int i = 0; i < Textures2D.size(); i++)
		{
			const TObjectPtr<LOpenGLImage2D> img = Textures2D.at(i).second->GetImage().As<LOpenGLImage2D>();
			switch (img->GetWidth())
			{
				case 512:
					GetTextureArray(0)->AddTextureToArray(Textures2D[i].second);
					break;

				case 1024:
					GetTextureArray(1)->AddTextureToArray(Textures2D[i].second);
					break;

				case 2048:
					GetTextureArray(2)->AddTextureToArray(Textures2D[i].second);
					break;

				default:
					break;
			}
		}

	#if 0 // DISABLED

		/// REMOVE
		for (int i = 0; i < TextureArrayCount; i++)
		{
			LK_CORE_WARN_TAG("OpenGLRenderer", "TextureArray {}  TypeID={}", i, RendererData->TextureArrays[i]->GetTypeID());
		}

		OpenGLRenderer2DSpecification renderer2DSpec;
		renderer2DSpec.TextureArraysUsed = TextureArrayCount;
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Creating 2D Renderer with {} texture arrays used", TextureArrayCount);
		Renderer2D = TObjectPtr<LOpenGLRenderer2D>::Create(renderer2DSpec);

		/* Add texture array references to OpenGLRenderer2D as well. */
		for (int i = 0; i < LOpenGLRenderer2D::MaxTextureArrays; i++)
		{
			if (RendererData->TextureArrays[i])
			{
                LK_CORE_DEBUG_TAG("OpenGLRenderer", "TextureArray {}  TypeID={}", i, RendererData->TextureArrays[i]->GetTypeID());
				Renderer2D->m_TextureArrays[i] = RendererData->TextureArrays[i];
				BindTextureArray(i);
			}
		}
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Initializing 2D renderer");
		Renderer2D->Init();

		RenderContext->SetDepthEnabled(true);
		/* Default to triangles */
		SetPrimitiveTopology(ERenderTopology::Triangles);

		/* Create render passes. */
		{
			/* Geometric. */
			RenderPassSpecification RenderPassSpec;
			RenderPassSpec.DebugName = "RenderPass_Geometric";
			m_GeometricPass = TObjectPtr<OpenGLRenderPass>::Create(RenderPassSpec);

			/* 2D. */
			RenderPassSpec.DebugName = "RenderPass_2D";
			m_RenderPass2D = TObjectPtr<OpenGLRenderPass>::Create(RenderPassSpec);
		}

	#if 1
		/* Setup debugging stuff. */
		SetupTexturesAndShaders();
        {
            GenerateCubeVaoAndVbo(CubeVAO, CubeVBO);
            GeneratePlaneVaoAndVbo(PlaneVAO, PlaneVBO);
            GenerateScreenQuadVaoAndVbo(QuadVAO, QuadVBO);
            CubeTexture_ = LoadTexture("Assets/Textures/container.jpg");
            FloorTexture_ = LoadTexture("Assets/Textures/metal.png");
        }
	#endif

	#endif
    }

    void OpenGLRenderer::Shutdown()
    {
		if (m_GeometricPass)
		{
			m_GeometricPass->Terminate();
			m_GeometricPass.Release();
		}
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
		LFramebuffer::TargetSwapChain();
		glClearColor(LRenderer::ClearColor.r, 
					 LRenderer::ClearColor.g, 
					 LRenderer::ClearColor.b, 
					 LRenderer::ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		LFramebuffer& ViewportFramebuffer = *LRenderer::GetViewportFramebuffer();
		ViewportFramebuffer.Bind();
		ViewportFramebuffer.Clear();
		ViewportFramebuffer.Unbind();
	}

	void OpenGLRenderer::SetPrimitiveTopology(const ERenderTopology& InRenderTopology)
	{
		// No need to call RenderContext here as the topology is used for draw calls
		switch (InRenderTopology)
		{
			case ERenderTopology::Lines:
			{
				m_Topology = GL_LINES;
				Renderer2D->m_Topology = GL_LINES;
				break;
			}

			case ERenderTopology::Triangles:
			{
				m_Topology = GL_TRIANGLES;
				Renderer2D->m_Topology = GL_TRIANGLES;
				break;
			}
		}
	}

	void OpenGLRenderer::SetDepthFunction(const EDepthFunction& InDepthFunction)
	{
		RenderContext->SetDepthFunction(InDepthFunction);
	}

	void OpenGLRenderer::Draw(LVertexBuffer& vb, const LShader& shader)
	{
		shader.Bind();
		glDrawElements(m_Topology, vb.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::Draw(const LVertexBuffer& vb, const LIndexBuffer& ib, const LShader& shader) 
	{
		glDrawElements(m_Topology, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform)
	{
		//LRenderer::Submit([&] 
		const uint8_t Topology = m_Topology;
		LRenderer::Submit([&Mesh, &Shader, &Transform, Topology] 
		{
			LMeshSource& MeshSource = *Mesh->GetMeshSource();
			LVertexBuffer& VertexBuffer = *MeshSource.GetVertexBuffer();
			LIndexBuffer& IndexBuffer = *VertexBuffer.GetIndexBuffer();

			Shader->Bind();

			VertexBuffer.Bind();
			LK_OpenGL(glDrawElements(Topology, IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));
			//LK_OpenGL(glDrawElements(m_Topology, IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));
		});
	}

	void OpenGLRenderer::SubmitImage(const TObjectPtr<LImage> image)
	{
		Renderer2D->DrawImage(image);
	}

	void OpenGLRenderer::SubmitImage(const TObjectPtr<LImage2D> image)
	{
		Renderer2D->DrawImage(image);
	}

	void OpenGLRenderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, uint64_t entityID)
	{
		Renderer2D->DrawLine(p1, p2, color, entityID);
	}

	void OpenGLRenderer::SubmitIndexed(unsigned int indexCount)
	{
		glDrawElements(m_Topology, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawIndexed(uint64_t indexCount)
	{
		glDrawElements(m_Topology, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		Renderer2D->DrawQuad(pos, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
		//Renderer2D::DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, 
									const glm::vec2& size, 
									TObjectPtr<LTexture> texture, 
									const float rotation, 
									uint64_t entityID)
	{
		Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, 
									const glm::vec2& size, 
									TObjectPtr<LTexture> texture, 
									const glm::vec4& tintColor, 
									const float rotation, 
									uint64_t entityID)
	{
		Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, tintColor, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, TObjectPtr<LTexture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID)
	{
		Renderer2D->DrawQuad(pos, size, texture, tintColor, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, TObjectPtr<LTexture> texture, float rotation, uint64_t entityID)
	{
		Renderer2D->DrawQuad(pos, size, texture, rotation, entityID);
	}

	RendererCapabilities& OpenGLRenderer::GetCapabilities()
	{
		return RendererData->m_RendererCapabilities;
	}

	void OpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> _renderCommandBuffer, 
										TObjectPtr<LPipeline> _pipeline, 
										TObjectPtr<LVertexBuffer> _vertexBuffer, 
										TObjectPtr<LIndexBuffer> _indexBuffer, 
										const glm::mat4& transform, 
										uint32_t indexCount /* == 0*/)
	{
		TObjectPtr<LOpenGLPipeline> pipeline = _pipeline.As<LOpenGLPipeline>();
		TObjectPtr<LOpenGLShader> shader = pipeline->GetShader();
		TObjectPtr<OpenGLVertexBuffer> vertexBuffer = _vertexBuffer.As<OpenGLVertexBuffer>();

	#if 0
		LRenderer::Submit([_renderCommandBuffer, pipeline, vertexBuffer, _indexBuffer, transform, indexCount]() mutable
		{
		});
	#endif
	}

	void OpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> _renderCommandBuffer, 
										TObjectPtr<LPipeline> InPipeline, 
										TObjectPtr<LMaterial> InMaterial, 
										TObjectPtr<LVertexBuffer> _vertexBuffer, 
										TObjectPtr<LIndexBuffer> 
										_indexBuffer, 
										const glm::mat4& transform, 
										uint32_t indexCount /* == 0*/)
	{
		LK_CORE_ASSERT(false, "Not implemented");
	}

	void OpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> _renderCommandBuffer, 
										TObjectPtr<LPipeline> InPipeline, 
										TObjectPtr<LShader>& shader, 
										TObjectPtr<LVertexBuffer>& InVertexBuffer, 
										TObjectPtr<LIndexBuffer>& _indexBuffer, 
										const glm::mat4& transform, 
										uint32_t indexCount /* == 0*/)
	{
		TObjectPtr<LOpenGLPipeline> pipeline = InPipeline.As<LOpenGLPipeline>();
		std::deque<LRendererID> boundTextureArrays = pipeline->GetBoundTextureArrays();
		TObjectPtr<LFramebuffer> framebuffer = LRenderer::GetViewportFramebuffer();
		LRenderer::Submit([&, pipeline, framebuffer, shader, 
						  InVertexBuffer, transform, indexCount, boundTextureArrays]() mutable
		{
			framebuffer->Bind();
			shader->Bind();

			int i = 1;
			for (LRendererID& textureArray : boundTextureArrays)
			{
				shader->Set("u_TextureArray" + std::to_string(i++), textureArray);
			}
			
			InVertexBuffer->Bind();
			DrawIndexed(indexCount);

			framebuffer->Unbind();
		});
	}

	void OpenGLRenderer::BeginRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
										 TObjectPtr<LRenderPass> renderPass, 
										 const bool ExplicitClear)
	{
#if 0
		auto commandBuffer = renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
		auto pass = renderPass.As<OpenGLRenderPass>();
		auto pipeline = pass->GetPipeline().As<LOpenGLPipeline>();
		auto shader = pipeline->GetShader().As<OpenGLShader>();
#endif
	}

	void OpenGLRenderer::EndRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer)
	{
		auto commandBuffer = renderCommandBuffer.As<OpenGLRenderCommandBuffer>();
	}

	TObjectPtr<OpenGLTextureArray> OpenGLRenderer::GetTextureArray(int idx)
	{
		return RendererData->TextureArrays[idx];
	}

	void OpenGLRenderer::BindTextureArray(int idx)
	{
		TextureArray& textureArray = *RendererData->TextureArrays[idx];
		textureArray.Bind();
	}

	void OpenGLRenderer::BindTextureArray(const ETextureArrayDimension& TextureArrayDimension)
	{
		OpenGLTextureArray& textureArray = *GetTextureArrayWithDimension(TextureArrayDimension);
		textureArray.Bind();
	}

	TObjectPtr<OpenGLTextureArray> OpenGLRenderer::GetTextureArrayWithDimension(const ETextureArrayDimension& TextureArrayDimension)
	{
		for (TObjectPtr<OpenGLTextureArray>& TextureArray : RendererData->TextureArrays)
		{
			if (TextureArray->Specification.TextureArrayDimension == TextureArrayDimension)
			{
				return TextureArray;
			}
		}

		LK_CORE_ASSERT(false, "Couldn't find TextureArray with dimension {}...", static_cast<int>(TextureArrayDimension));
	}

}
