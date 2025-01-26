#include "LKpch.h"
#include "Renderer.h"

#include "Texture.h"
#include "RenderContext.h"

#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Core/Application.h"

#if defined(LK_ENGINE_EDITOR)
#	include "LkEngine/Editor/EditorLayer.h"
#endif


namespace LkEngine {

	struct FShaderDependency
	{
		std::vector<TObjectPtr<LPipeline>> Pipelines{};
		std::vector<TObjectPtr<LMaterial>> Materials{};
	};

	static std::unordered_map<std::size_t, FShaderDependency> ShaderDependencies;

	void LRendererAPI::SetAPI(ERendererAPI InRendererAPI)
	{
		LRendererAPI::RendererAPI = InRendererAPI;
	}

	struct FRendererData
	{
		TObjectPtr<LShaderLibrary> ShaderLibrary{};

		TObjectPtr<LTexture2D> WhiteTexture{};
		TObjectPtr<LTexture2D> BlackTexture{};
	};

	static FRendererData* RendererData = nullptr;
	static constexpr uint32_t RenderCommandQueueCount = 2;
	static LRenderCommandQueue* CommandQueue[RenderCommandQueueCount];
	static std::atomic<uint32_t> RenderCommandQueueSubmissionIndex = 0;
	static LRenderCommandQueue ResourceFreeQueue[3];

	LRenderer::LRenderer()
	{
		LOBJECT_REGISTER(LRenderer);
	}

	void LRenderer::Initialize()
	{
		LK_CORE_ASSERT(RendererData == nullptr, "RendererData already exist");
		LK_CORE_DEBUG_TAG("Renderer", "Initializing");
		RendererData = new FRendererData();

		PrimitiveTopology = ERenderTopology::Triangles;
		CommandQueue[0] = new LRenderCommandQueue();
		CommandQueue[1] = new LRenderCommandQueue();

		RendererData->ShaderLibrary = TObjectPtr<LShaderLibrary>::Create();
		if (TObjectPtr<LShaderLibrary> ShaderLibrary = GetShaderLibrary(); ShaderLibrary != nullptr)
		{
			LK_CORE_DEBUG_TAG("Renderer", "Loading shaders");
			ShaderLibrary->Load("Renderer2D_Quad",   LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer2D_Quad.shader");
			ShaderLibrary->Load("Renderer2D_Line",   LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer2D_Line.shader");
			ShaderLibrary->Load("Renderer2D_Debug",  LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer2D_Debug.shader");
			ShaderLibrary->Load("Renderer2D_Screen", LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer2D_Screen.shader");
			ShaderLibrary->Load("Renderer_Debug",    LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer_Debug.shader");
			ShaderLibrary->Load("Renderer_Skybox",   LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer_Skybox.shader");
			ShaderLibrary->Load("Renderer_Model",    LFileSystem::GetAssetsDir() / "Shaders/OpenGL/Renderer_Model.shader");
			LK_CORE_DEBUG_TAG("Renderer", "Loaded {} shaders", RendererData->ShaderLibrary->GetShadersLoaded());
		}

		/* Create white texture. */
		{
			LK_CORE_DEBUG_TAG("Renderer", "Creating white texture");
			FTextureSpecification TextureSpec;
			TextureSpec.Format = EImageFormat::RGBA32F;
			TextureSpec.Width = 2048;
			TextureSpec.Height = 2048;
			TextureSpec.SamplerFilter = ETextureFilter::None;
			TextureSpec.SamplerWrap = ETextureWrap::None;
			const std::filesystem::path TexturePath = LFileSystem::GetAssetsDir() / "Textures" / "White.png";
			TextureSpec.Path = TexturePath.string();
			TextureSpec.Name = "White";
			TextureSpec.DebugName = "White";
			RendererData->WhiteTexture = LTexture2D::Create(TextureSpec);
		}
		LK_CORE_VERIFY(RendererData->WhiteTexture && RendererData->WhiteTexture->IsValid());

		RendererAPI = LRendererAPI::Create();
		RendererAPI->Initialize();

		RendererAPI->SetDepthEnabled(true);
		RendererAPI->SetPrimitiveTopology(ERenderTopology::Triangles);
	}

	void LRenderer::Destroy()
	{
		LK_CORE_VERIFY(RendererAPI);
		RendererAPI->Destroy();
	}

	void LRenderer::Clear()
	{
		RendererAPI->Clear();
	}

	void LRenderer::BeginFrame()
	{
		LRenderer::SwapQueues();

		LFramebuffer::TargetSwapChain();
		RendererAPI->Clear();

		if (TObjectPtr<LFramebuffer> ViewportFramebuffer = GetViewportFramebuffer())
		{
			ViewportFramebuffer->Bind();
			ViewportFramebuffer->Clear();
			ViewportFramebuffer->Unbind();
		}
	}

	void LRenderer::EndFrame()
	{
		CommandQueue[GetRenderQueueIndex()]->Execute();
	}

	void LRenderer::SwapQueues()
	{
		RenderCommandQueueSubmissionIndex = (RenderCommandQueueSubmissionIndex + 1) % RenderCommandQueueCount;
	}

	void LRenderer::SetPrimitiveTopology(const ERenderTopology InRenderTopology)
	{
		PrimitiveTopology = InRenderTopology;
		RendererAPI->SetPrimitiveTopology(InRenderTopology);
	}

	LRenderCommandQueue& LRenderer::GetRenderCommandQueue()
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
	#if LK_ENGINE_EDITOR
		return LEditorLayer::Get().ViewportFramebuffer;
	#endif

		LK_CORE_ASSERT(false, "Not implemented");
		return nullptr;
	}

	TObjectPtr<LShaderLibrary> LRenderer::GetShaderLibrary()
	{
		LK_CORE_ASSERT(RendererData->ShaderLibrary, "ShaderLibrary is nullptr");
		return RendererData->ShaderLibrary;
	}

	LRenderer* LRenderer::Get()
	{
		static LRenderer Renderer;
		return &Renderer;
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

	void LRenderer::SubmitLines(const LVertexBuffer& VertexBuffer, const LIndexBuffer& ib, const LShader& Shader)
	{
		Shader.Bind();
		VertexBuffer.Bind();
		ib.Bind();

		ERenderTopology InitialTopology = GetPrimitiveTopology();
		SetPrimitiveTopology(ERenderTopology::Lines);
		RendererAPI->Draw(VertexBuffer, ib, Shader);

		/* Reset initial topology */
		SetPrimitiveTopology(InitialTopology);
	}

	void LRenderer::SubmitIndexed(LVertexBuffer& VertexBuffer, const uint32_t Count)
	{
		VertexBuffer.Bind();
		RendererAPI->SubmitIndexed(Count);
	}

	void
	LRenderer::SubmitQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color, const uint64_t EntityID)
	{
		RendererAPI->SubmitQuad({ Position.x, Position.y, 0.0f }, Size, Color, EntityID);
	}

	void LRenderer::SubmitQuad(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(Position, Size, Color, EntityID);
	}

	void
	LRenderer::SubmitQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad({ Position.x, Position.y, 0.0f }, Size, Texture, EntityID);
	}

	void
	LRenderer::SubmitQuad(const glm::vec3& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(Position, Size, Texture, 0.0f, EntityID);
	}

	void LRenderer::SubmitSprite(LTransformComponent& TransformComponent,
								 const glm::vec2& Size,
								 const glm::vec4 Color,
								 uint64_t EntityID)
	{
		RendererAPI->SubmitQuad({ TransformComponent.Translation.x, TransformComponent.Translation.y },
								Size,
								Color,
								TransformComponent.Rotation2D,
								EntityID);
	}

	void LRenderer::SubmitSprite(
		LTransformComponent& TransformComponent, const glm::vec2& Size, TObjectPtr<LTexture> Texture, uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(TransformComponent.Translation, Size, Texture, TransformComponent.Rotation2D, EntityID);
	}

	void LRenderer::SubmitSprite(LTransformComponent& TransformComponent, 
								 const glm::vec2& Size,
								 TObjectPtr<LTexture> Texture,
								 const glm::vec4& Color,
								 uint64_t EntityID)
	{
		RendererAPI->SubmitQuad(TransformComponent.Translation, Size, Texture, Color, TransformComponent.Rotation2D, EntityID);
	}

	FRendererCapabilities& LRenderer::GetCapabilities()
	{
		return RendererAPI->GetCapabilities();
	}

	uint32_t LRenderer::GetCurrentFrameIndex()
	{
		return LApplication::Get().GetCurrentFrameIndex();
	}

	void LRenderer::BeginRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer, TObjectPtr<LRenderPass> renderPass)
	{
		RendererAPI->BeginRenderPass(RenderCommandBuffer, renderPass, false);
	}

	void LRenderer::EndRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer)
	{
		RendererAPI->EndRenderPass(RenderCommandBuffer);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer,
								   const glm::mat4& Transform,
								   uint32_t IndexCount /*= 0*/)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		RendererAPI->RenderGeometry(RenderCommandBuffer, Pipeline, VertexBuffer, IndexBuffer, Transform, IndexCount);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LShader> Shader,
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer,
								   const glm::mat4& Transform,
								   const uint32_t IndexCount /*= 0*/)
	{
		RendererAPI->RenderGeometry(RenderCommandBuffer, Pipeline, Shader, VertexBuffer, IndexBuffer, Transform, IndexCount);
	}

	void LRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
								   TObjectPtr<LPipeline> Pipeline,
								   TObjectPtr<LMaterial> Material,
								   TObjectPtr<LVertexBuffer> VertexBuffer,
								   TObjectPtr<LIndexBuffer> IndexBuffer,
								   const glm::mat4& Transform,
								   const uint32_t IndexCount /*= 0*/)
	{
		// LK_MARK_FUNC_NOT_IMPLEMENTED();
		RendererAPI->RenderGeometry(RenderCommandBuffer, Pipeline, Material, VertexBuffer, IndexBuffer, Transform, IndexCount);
	}

	TObjectPtr<LTexture2D> LRenderer::GetWhiteTexture()
	{
		LK_CORE_VERIFY(RendererData->WhiteTexture);
		return RendererData->WhiteTexture;
	}

	TObjectPtr<LTextureCube> LRenderer::GetWhiteTextureCube()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		return nullptr;
	}

	void LRenderer::DrawMesh(TObjectPtr<LMesh>& Mesh, const TObjectPtr<LShader> Shader)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LK_UNUSED(Mesh && Shader);
	}

	void LRenderer::RegisterShader(TObjectPtr<LShader> Shader, TObjectPtr<LMaterial> Material)
	{
		LK_CORE_VERIFY(Shader && Material);
		FShaderDependency& Materials = ShaderDependencies[Shader->GetHash()];
		LK_CORE_TRACE_TAG("Renderer", "Registering shader {} to material '{}'", Shader->GetName(), Material->GetName());
		ShaderDependencies[Shader->GetHash()].Materials.push_back(Material);
	}

	void LRenderer::SetDepthFunction(const EDepthFunction InDepthFunction)
	{
		RendererAPI->SetDepthFunction(InDepthFunction);
	}

}
