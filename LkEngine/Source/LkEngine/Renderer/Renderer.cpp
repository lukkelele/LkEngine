#include "LKpch.h"
#include "Renderer.h"

//#include "RendererAPI.h"
#include "Texture.h"
#include "RenderContext.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	struct ShaderDependencies
	{
		std::vector<TObjectPtr<LPipeline>> Pipelines{};
		std::vector<TObjectPtr<LMaterial>> Materials{};
	};

	static std::unordered_map<size_t, ShaderDependencies> s_ShaderDependencies;

	void LRendererAPI::SetAPI(ERendererAPI InRendererAPI)
	{
		LRendererAPI::RendererAPI = InRendererAPI;
	}

	struct FRendererData
	{
		TObjectPtr<LShaderLibrary> m_ShaderLibrary;
		TObjectPtr<LTextureLibrary> TextureLibrary;
		TObjectPtr<LMaterialLibrary> MaterialLibrary;

		TObjectPtr<LTexture2D> WhiteTexture;
		TObjectPtr<LTexture2D> BlackTexture;

		LEditorLayer* Editor = nullptr;
	};

	static FRendererData* RendererData = nullptr;
	constexpr static uint32_t RenderCommandQueueCount = 2;
	static RenderCommandQueue* CommandQueue[RenderCommandQueueCount];
	static std::atomic<uint32_t> RenderCommandQueueSubmissionIndex = 0;
	static RenderCommandQueue ResourceFreeQueue[3];

	LRenderer::LRenderer()
	{
		s_Instance = this;
	}

	void LRenderer::Initialize()
	{
		RendererData = new FRendererData();

		PrimitiveTopology = ERenderTopology::Triangles;
		CommandQueue[0] = new RenderCommandQueue();
		CommandQueue[1] = new RenderCommandQueue();

		LK_CORE_DEBUG_TAG("Renderer", "Creating shader library");
		RendererData->m_ShaderLibrary = TObjectPtr<LShaderLibrary>::Create();

		if (TObjectPtr<LShaderLibrary> ShaderLibrary = GetShaderLibrary())
		{
			LK_CORE_DEBUG_TAG("Renderer", "Loading shaders");
			ShaderLibrary->Load("Renderer2D_Quad",    "Assets/Shaders/OpenGL/Renderer2D_Quad.shader");
			ShaderLibrary->Load("Renderer2D_Line",    "Assets/Shaders/OpenGL/Renderer2D_Line.shader");
			ShaderLibrary->Load("Renderer2D_Debug",   "Assets/Shaders/OpenGL/Renderer2D_Debug.shader");
			ShaderLibrary->Load("Renderer2D_Screen",  "Assets/Shaders/OpenGL/Renderer2D_Screen.shader");
			ShaderLibrary->Load("Renderer_Debug",     "Assets/Shaders/OpenGL/Renderer_Debug.shader");
			ShaderLibrary->Load("Renderer_Skybox",    "Assets/Shaders/OpenGL/Renderer_Skybox.shader");
			ShaderLibrary->Load("Renderer_Model",     "Assets/Shaders/OpenGL/Renderer_Model.shader");
		}

		LK_CORE_DEBUG_TAG("Renderer", "Creating texture library");
		RendererData->TextureLibrary = TObjectPtr<LTextureLibrary>(&LTextureLibrary::Get());
		RendererData->WhiteTexture = RendererData->TextureLibrary->GetWhiteTexture();
		

		LK_CORE_DEBUG_TAG("Renderer", "Creating material library");
		RendererData->MaterialLibrary = TObjectPtr<LMaterialLibrary>::Create();

		RendererAPI = LRendererAPI::Create();
		RendererAPI->Init();
	}

	void LRenderer::Clear()
	{
		RendererAPI->Clear();
	}

	void LRenderer::BeginFrame()
	{
        LRenderer::SwapQueues();
		RendererAPI->BeginFrame();
	}

	void LRenderer::EndFrame()
	{
		CommandQueue[GetRenderQueueIndex()]->Execute();
		RendererAPI->EndFrame();
	}

	void LRenderer::Shutdown()
	{
		RendererAPI->Shutdown();
	}

	void LRenderer::SetPrimitiveTopology(const ERenderTopology& InRenderTopology)
	{
		PrimitiveTopology = InRenderTopology;
		RendererAPI->SetPrimitiveTopology(InRenderTopology);
	}

	void LRenderer::SwapQueues()
	{
		RenderCommandQueueSubmissionIndex = (RenderCommandQueueSubmissionIndex + 1) % RenderCommandQueueCount;
	}

	RenderCommandQueue& LRenderer::GetRenderCommandQueue()
	{
		return *CommandQueue[RenderCommandQueueSubmissionIndex];
	}

	uint32_t LRenderer::GetRenderQueueIndex()
	{
		return (RenderCommandQueueSubmissionIndex + 1) % RenderCommandQueueCount;
	}

	uint32_t LRenderer::GetRenderQueueSubmissionIndex()
	{
		return RenderCommandQueueSubmissionIndex;
	}

	TObjectPtr<LFramebuffer>& LRenderer::GetViewportFramebuffer()
	{
		/// FIXME
		// Editor is enabled, thus the render target is a texture that the Editor is responsible for
		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			return Editor->m_ViewportFramebuffer;
		}
		else
		{
			// TODO:
			LK_CORE_ASSERT(false, "Not implemented yet!");
		}
	}

	TObjectPtr<LShaderLibrary> LRenderer::GetShaderLibrary()
	{
		LK_CORE_ASSERT(RendererData->m_ShaderLibrary, "ShaderLibrary is nullptr!");
		return RendererData->m_ShaderLibrary;
	}

	void LRenderer::SubmitMesh(TObjectPtr<Mesh>& mesh, 
							   TObjectPtr<LShader>& shader, 
							   const glm::mat4& transform)
	{
		RendererAPI->SubmitMesh(mesh, shader, transform);
	}

	void LRenderer::SubmitImage(const TObjectPtr<LImage> image)
	{
		RendererAPI->SubmitImage(image);
	}

	void LRenderer::SubmitImage(const TObjectPtr<LImage2D> image)
	{
		RendererAPI->SubmitImage(image);
	}

	void LRenderer::SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID)
	{
		RendererAPI->SubmitLine(p0, p1, color, entityID);
	}

	void LRenderer::SubmitLines(const LVertexBuffer& vb, const LIndexBuffer& ib, const LShader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();

		ERenderTopology InitialTopology = GetPrimitiveTopology();
		SetPrimitiveTopology(ERenderTopology::Lines);
		RendererAPI->Draw(vb, ib, shader);

		/* Reset initial topology */
		SetPrimitiveTopology(InitialTopology);
	}

	void LRenderer::SubmitIndexed(LVertexBuffer& VertexBuffer, const uint32_t Count)
	{
        VertexBuffer.Bind();
		RendererAPI->SubmitIndexed(Count);
	}

	void LRenderer::SubmitQuad(const glm::vec2& pos, 
							   const glm::vec2& size, 
							   const glm::vec4& color, 
							   const uint64_t entityID)
	{
		RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void LRenderer::SubmitQuad(const glm::vec3& pos, 
							   const glm::vec2& size, 
							   const glm::vec4& color, 
							   uint64_t entityID)
	{
		RendererAPI->SubmitQuad(pos, size, color, entityID);
	}

	void LRenderer::SubmitQuad(const glm::vec2& pos, 
							   const glm::vec2& size, 
							   TObjectPtr<LTexture> texture, 
							   uint64_t entityID)
	{
		RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, texture, entityID);
	}

	void LRenderer::SubmitQuad(const glm::vec3& pos, 
							   const glm::vec2& size, 
							   TObjectPtr<LTexture> texture, 
							   uint64_t entityID)
	{
		RendererAPI->SubmitQuad(pos, size, texture, 0.0f, entityID);
	}

	void LRenderer::SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 const glm::vec4 color, 
								 uint64_t entityID)
    {
        RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, 
								size, 
								color, 
								tc.Rotation2D, 
								entityID);
    }

	void LRenderer::SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 TObjectPtr<LTexture> texture, 
								 uint64_t entityID)
    {
        RendererAPI->SubmitQuad(tc.Translation, size, texture, tc.Rotation2D, entityID);
    }

	void LRenderer::SubmitSprite(LTransformComponent& tc, 
								 const glm::vec2& size, 
								 TObjectPtr<LTexture> texture, 
								 const glm::vec4& color, 
								 uint64_t entityID)
    {
        RendererAPI->SubmitQuad(tc.Translation, size, texture, color, tc.Rotation2D, entityID);
    }

	// REMOVE
	void LRenderer::BeginScene(const LSceneCamera& InSceneCamera)
	{
		//Renderer2DAPI->BeginScene(InSceneCamera);
	}

	// REMOVE
	void LRenderer::BeginScene(const glm::mat4& viewProjectionMatrix)
	{
		//Renderer2DAPI->BeginScene(viewProjectionMatrix);
	}

	RendererCapabilities& LRenderer::GetCapabilities()
	{
		return RendererAPI->GetCapabilities();
	}

	uint32_t LRenderer::GetCurrentFrameIndex()
	{
		return LApplication::Get()->GetCurrentFrameIndex();
	}

	uint32_t LRenderer::RT_GetCurrentFrameIndex()
	{
		// Swapchain owns the RenderThread frame index
		return LApplication::Get()->GetWindow().GetSwapChain()->GetCurrentBufferIndex();
	}

	void LRenderer::BeginRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, TObjectPtr<LRenderPass> renderPass)
	{
		RendererAPI->BeginRenderPass(renderCommandBuffer, renderPass, false);
	}

	void LRenderer::EndRenderPass(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer)
	{
		RendererAPI->EndRenderPass(renderCommandBuffer);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LVertexBuffer> vertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& transform, 
								   uint32_t IndexCount /*= 0*/)
	{
		//RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, vertexBuffer, indexBuffer, transform, IndexCount);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LShader> shader, 
								   TObjectPtr<LVertexBuffer> vertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& transform, 
								   const uint32_t indexCount /*= 0*/)
	{
		//RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, shader, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> renderCommandBuffer, 
								   TObjectPtr<LPipeline> pipeline, 
								   TObjectPtr<LMaterial> material, 
								   TObjectPtr<LVertexBuffer> vertexBuffer, 
								   TObjectPtr<LIndexBuffer> indexBuffer, 
								   const glm::mat4& Transform, 
								   const uint32_t IndexCount /*= 0*/)
	{
		//RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, material, vertexBuffer, indexBuffer, Transform, IndexCount);
	}
	
	TObjectPtr<LTexture2D> LRenderer::GetWhiteTexture()
	{
		return RendererData->WhiteTexture;
	}

	TObjectPtr<LTextureCube> LRenderer::GetWhiteTextureCube()
	{
		LK_CORE_ASSERT(false, "Not implemented!");
		return nullptr;
	}

	void LRenderer::DrawMesh(Ref<Mesh>& mesh, const TObjectPtr<LShader> shader)
	{
		//RendererAPI->Draw(*mesh->GetMeshSource()->GetVertexBuffer(), *shader);
	}

	void LRenderer::RegisterShaderDependency(TObjectPtr<LShader> shader, TObjectPtr<LMaterial> material)
	{
		s_ShaderDependencies[shader->GetHash()].Materials.push_back(material);
	}

	void LRenderer::LoadTextures()
	{
		LTextureLibrary& TextureLibrary = LTextureLibrary::Get();

		// Textures: 512x512
		{
			TextureSpecification textureSpec;
			// Grass
			textureSpec.Width = 512;
		    textureSpec.Height = 512;
            textureSpec.Path = "Assets/Textures/grass.png";
            textureSpec.Name = "grass-512x512";
            textureSpec.DebugName = "grass-512x512";
            textureSpec.GenerateMips = true;
			textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			TextureLibrary.AddTexture(textureSpec);

			// Ice Skybox 
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.Name = "skybox-ice-back-512x512";
            textureSpec.DebugName = "skybox-ice-back-512x512";
            textureSpec.GenerateMips = false;
			textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary.AddTexture(textureSpec);
		}
		// Textures: 1024x1024
		{
			TextureSpecification textureSpec;
			// Brickwall
			textureSpec.Width = 1024;
		    textureSpec.Height = 1024;
            textureSpec.Path = "Assets/Textures/brickwall.jpg";
            textureSpec.Name = "brickwall";
            textureSpec.DebugName = "brickwall";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			TextureLibrary.AddTexture(textureSpec);
		}

		// Textures: 2048x2048 
		{
			TextureSpecification textureSpec;
			// Wood container
			textureSpec.Format = ImageFormat::RGBA32F;
			textureSpec.Width = 2048;
			textureSpec.Height = 2048;
			textureSpec.Path = "Assets/Textures/container.jpg";
			textureSpec.Name = "wood-container";
			textureSpec.DebugName = "wood-container";
			textureSpec.SamplerWrap = TextureWrap::Clamp;
			textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary.AddTexture(textureSpec);

			// Wood container 2
			textureSpec.Path = "Assets/Textures/container2.png";
			textureSpec.Name = "wood-container2";
			textureSpec.DebugName = "wood-container2";
			TextureLibrary.AddTexture(textureSpec);

			// Bricks
			textureSpec.Path = "Assets/Textures/bricks_orange.jpg";
			textureSpec.Name = "bricks";
			textureSpec.DebugName = "bricks";
			TextureLibrary.AddTexture(textureSpec);

			// Åle texture
			textureSpec.Path = "Assets/Textures/Misc/ale_1024x1024.png";
			textureSpec.Name = "ale1024";
			textureSpec.DebugName = "ale1024";
			TextureLibrary.AddTexture(textureSpec);

			// Lukas texture
			textureSpec.Path = "Assets/Textures/Misc/lukas_1024.jpg";
			textureSpec.Name = "lukas_1024";
			textureSpec.DebugName = "lukas-1024x1024";
			textureSpec.SamplerWrap = TextureWrap::Repeat;
			TextureLibrary.AddTexture(textureSpec);

			// Metal
            textureSpec.Path = "Assets/Textures/metal.png";
            textureSpec.Name = "metal-ground";
            textureSpec.DebugName = "metal-ground";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary.AddTexture(textureSpec);

			// Wood
            textureSpec.Name = "wood";
            textureSpec.DebugName = "wood";
            textureSpec.Path = "Assets/Textures/wood.png";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			TextureLibrary.AddTexture(textureSpec);

			// Skybox
            textureSpec.Name = "skybox-ice-back";
            textureSpec.DebugName = "skybox-ice-back";
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.GenerateMips = false;
            textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
			TextureLibrary.AddTexture(textureSpec);
        }
	}

	void LRenderer::SetDepthFunction(const EDepthFunction& InDepthFunction)
	{
		RendererAPI->SetDepthFunction(InDepthFunction);
	}


}
