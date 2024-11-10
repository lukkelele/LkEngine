#include "LKpch.h"
#include "Renderer.h"

#include "Texture.h"
#include "RenderContext.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	struct FShaderDependencies
	{
		std::vector<TObjectPtr<LPipeline>> Pipelines{};
		std::vector<TObjectPtr<LMaterial>> Materials{};
	};

	static std::unordered_map<std::size_t, FShaderDependencies> ShaderDependencies;

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
		LCLASS_REGISTER(LRenderer);
		Instance = this;
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

		LK_CORE_DEBUG_TAG("Renderer", "Creating Texture library");
		RendererData->TextureLibrary = TObjectPtr<LTextureLibrary>(&LTextureLibrary::Get());
		RendererData->TextureLibrary->Initialize();
		RendererData->WhiteTexture = RendererData->TextureLibrary->GetWhiteTexture();

		LK_CORE_DEBUG_TAG("Renderer", "Creating material library");
		RendererData->MaterialLibrary = TObjectPtr<LMaterialLibrary>::Create();

		RendererAPI = LRendererAPI::Create();
		RendererAPI->Initialize();
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

	TObjectPtr<LFramebuffer> LRenderer::GetViewportFramebuffer()
	{
		/* TODO: Should not do it like this for the editor, skip the LRenderer here entirely... */
		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			return Editor->ViewportFramebuffer;
		}

		LK_CORE_ASSERT(false, "Not implemented yet!");
		return nullptr;
	}

	TObjectPtr<LShaderLibrary> LRenderer::GetShaderLibrary()
	{
		LK_CORE_ASSERT(RendererData->m_ShaderLibrary, "ShaderLibrary is nullptr");
		return RendererData->m_ShaderLibrary;
	}

	void LRenderer::SubmitMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform)
	{
		RendererAPI->SubmitMesh(Mesh, Shader, Transform);
	}

	void LRenderer::SubmitImage(const TObjectPtr<LImage> image)
	{
		RendererAPI->SubmitImage(image);
	}

	void LRenderer::SubmitImage(const TObjectPtr<LImage2D> image)
	{
		RendererAPI->SubmitImage(image);
	}

	void LRenderer::SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& Color, uint32_t EntityID)
	{
		RendererAPI->SubmitLine(p0, p1, Color, EntityID);
	}

	void LRenderer::SubmitLines(const LVertexBuffer& VertexBuffer, const LIndexBuffer& ib, const LShader& shader) 
	{
		shader.Bind();
		VertexBuffer.Bind();
		ib.Bind();

		ERenderTopology InitialTopology = GetPrimitiveTopology();
		SetPrimitiveTopology(ERenderTopology::Lines);
		RendererAPI->Draw(VertexBuffer, ib, shader);

		/* Reset initial topology */
		SetPrimitiveTopology(InitialTopology);
	}

	void LRenderer::SubmitIndexed(LVertexBuffer& VertexBuffer, const uint32_t Count)
	{
        VertexBuffer.Bind();
		RendererAPI->SubmitIndexed(Count);
	}

	void LRenderer::SubmitQuad(const glm::vec2& Position, const glm::vec2& Size, 
							   const glm::vec4& Color, const uint64_t EntityID)
	{
		RendererAPI->SubmitQuad({ Position.x, Position.y, 0.0f }, Size, Color, EntityID);
	}

	void LRenderer::SubmitQuad(const glm::vec3& Position, const glm::vec2& Size, 
							   const glm::vec4& Color, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(Position, Size, Color, EntityID);
	}

	void LRenderer::SubmitQuad(const glm::vec2& Position, const glm::vec2& Size, 
							   TObjectPtr<LTexture> Texture, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad({ Position.x, Position.y, 0.0f }, Size, Texture, EntityID);
	}

	void LRenderer::SubmitQuad(const glm::vec3& Position, const glm::vec2& Size, 
							   TObjectPtr<LTexture> Texture, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(Position, Size, Texture, 0.0f, EntityID);
	}

	void LRenderer::SubmitSprite(LTransformComponent& TransformComponent, const glm::vec2& Size, 
								 const glm::vec4 Color, uint64_t EntityID)
    {
        RendererAPI->SubmitQuad(
			{ TransformComponent.Translation.x, TransformComponent.Translation.y }, 
			Size, 
			Color, 
			TransformComponent.Rotation2D, 
			EntityID
		);
    }

	void LRenderer::SubmitSprite(LTransformComponent& TransformComponent, 
								 const glm::vec2& Size, 
								 TObjectPtr<LTexture> Texture, 
								 uint64_t EntityID)
    {
        RendererAPI->SubmitQuad(TransformComponent.Translation, Size, Texture, TransformComponent.Rotation2D, EntityID);
    }

	void LRenderer::SubmitSprite(LTransformComponent& TransformComponent, const glm::vec2& Size, 
								 TObjectPtr<LTexture> Texture, const glm::vec4& Color, uint64_t EntityID)
    {
        RendererAPI->SubmitQuad(TransformComponent.Translation, Size, Texture, Color, TransformComponent.Rotation2D, EntityID);
    }

	void LRenderer::BeginScene(const LSceneCamera& InSceneCamera)
	{
		//LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer::BeginScene(const glm::mat4& viewProjectionMatrix)
	{
		//LK_MARK_FUNC_NOT_IMPLEMENTED();
	}

	void LRenderer::EndScene()
	{
		//LK_MARK_FUNC_NOT_IMPLEMENTED();
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
		/* FIXME: Swapchain owns the RenderThread frame index. */
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
		LK_MARK_FUNC_NOT_IMPLEMENTED();
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
		LK_MARK_FUNC_NOT_IMPLEMENTED();
	}
	
	TObjectPtr<LTexture2D> LRenderer::GetWhiteTexture()
	{
		LK_VERIFY(RendererData->WhiteTexture, "White texture is nullptr");
		return RendererData->WhiteTexture;
	}

	TObjectPtr<LTextureCube> LRenderer::GetWhiteTextureCube()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return nullptr;
	}

	void LRenderer::DrawMesh(TObjectPtr<LMesh>& Mesh, const TObjectPtr<LShader> Shader)
	{
		//RendererAPI->Draw(*mesh->GetMeshSource()->GetVertexBuffer(), *shader);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LK_UNUSED(Mesh && Shader);
	}

	void LRenderer::RegisterShaderDependency(TObjectPtr<LShader> Shader, TObjectPtr<LMaterial> material)
	{
		ShaderDependencies[Shader->GetHash()].Materials.push_back(material);
	}

	/* TODO: Make this dynamic instead of using hardcoded entries. */
	void LRenderer::LoadTextures()
	{
		LTextureLibrary& TextureLibrary = LTextureLibrary::Get();

		/* Textures: 512x512 */
		{
			FTextureSpecification Specification{};

			/* Grass. */
			Specification.Width = 512;
		    Specification.Height = 512;
            Specification.Path = "Assets/Textures/grass.png";
            Specification.Name = "grass-512x512";
            Specification.DebugName = "grass-512x512";
            Specification.GenerateMips = true;
			Specification.Format = EImageFormat::RGBA32F;
            Specification.SamplerWrap = ETextureWrap::Repeat;
            Specification.SamplerFilter = ETextureFilter::Linear;
			TextureLibrary.AddTexture(Specification);

			/* Ice Skybox. */
            Specification.Path = "Assets/Textures/Skybox/back.jpg";
            Specification.Name = "skybox-ice-back-512x512";
            Specification.DebugName = "skybox-ice-back-512x512";
            Specification.GenerateMips = false;
			Specification.Format = EImageFormat::RGBA32F;
            Specification.SamplerWrap = ETextureWrap::Clamp;
            Specification.SamplerFilter = ETextureFilter::Nearest;
			TextureLibrary.AddTexture(Specification);
		}

		/* Textures: 1024x1024 */
		{
			FTextureSpecification Specification{};

			/* Brickwall. */
			Specification.Width = 1024;
		    Specification.Height = 1024;
            Specification.Path = "Assets/Textures/brickwall.jpg";
            Specification.Name = "brickwall";
            Specification.DebugName = "brickwall";
            Specification.GenerateMips = true;
            Specification.SamplerWrap = ETextureWrap::Repeat;
            Specification.SamplerFilter = ETextureFilter::Linear;
			TextureLibrary.AddTexture(Specification);
		}

		/* Textures: 2048x2048 */
		{
			FTextureSpecification Specification{};

			/* Wood Container. */
			Specification.Format = EImageFormat::RGBA32F;
			Specification.Width = 2048;
			Specification.Height = 2048;
			Specification.Path = "Assets/Textures/container.jpg";
			Specification.Name = "wood-container";
			Specification.DebugName = "wood-container";
			Specification.SamplerWrap = ETextureWrap::Clamp;
			Specification.SamplerFilter = ETextureFilter::Nearest;
			TextureLibrary.AddTexture(Specification);

			/* Wood Container 2. */
			Specification.Path = "Assets/Textures/container2.png";
			Specification.Name = "wood-container2";
			Specification.DebugName = "wood-container2";
			TextureLibrary.AddTexture(Specification);

			/* Bricks. */
			Specification.Path = "Assets/Textures/brickorange.jpg";
			Specification.Name = "bricks";
			Specification.DebugName = "bricks";
			TextureLibrary.AddTexture(Specification);

			/* Åle Texture. */
			Specification.Path = "Assets/Textures/Misc/ale_1024x1024.png";
			Specification.Name = "ale1024";
			Specification.DebugName = "ale1024";
			TextureLibrary.AddTexture(Specification);

			/* Lukas Texture. */
			Specification.Path = "Assets/Textures/Misc/luka1024.jpg";
			Specification.Name = "luka1024";
			Specification.DebugName = "lukas-1024x1024";
			Specification.SamplerWrap = ETextureWrap::Repeat;
			TextureLibrary.AddTexture(Specification);

			/* Metal. */
            Specification.Path = "Assets/Textures/metal.png";
            Specification.Name = "metal-ground";
            Specification.DebugName = "metal-ground";
            Specification.GenerateMips = true;
            Specification.SamplerWrap = ETextureWrap::Repeat;
            Specification.SamplerFilter = ETextureFilter::Nearest;
			TextureLibrary.AddTexture(Specification);

			/* Wood. */
            Specification.Name = "wood";
            Specification.DebugName = "wood";
            Specification.Path = "Assets/Textures/wood.png";
            Specification.GenerateMips = true;
            Specification.SamplerWrap = ETextureWrap::Repeat;
            Specification.SamplerFilter = ETextureFilter::Linear;
			TextureLibrary.AddTexture(Specification);

			/* Ice Skybox. */
            Specification.Name = "skybox-ice-back";
            Specification.DebugName = "skybox-ice-back";
            Specification.Path = "Assets/Textures/Skybox/back.jpg";
            Specification.GenerateMips = false;
            Specification.Format = EImageFormat::RGBA32F;
            Specification.SamplerFilter = ETextureFilter::Nearest;
            Specification.SamplerWrap = ETextureWrap::Clamp;
			TextureLibrary.AddTexture(Specification);
        }
	}

	void LRenderer::SetDepthFunction(const EDepthFunction& InDepthFunction)
	{
		RendererAPI->SetDepthFunction(InDepthFunction);
	}


}
