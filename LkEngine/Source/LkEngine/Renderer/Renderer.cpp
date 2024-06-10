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
		std::vector<Ref<Pipeline>> Pipelines;
		std::vector<Ref<Material>> Materials;
	};
	static std::unordered_map<size_t, ShaderDependencies> s_ShaderDependencies;

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		RendererAPI::m_CurrentRendererAPI = api;
	}

	Ref<RenderContext> Renderer::GetContext()
	{
		return Application::Get()->GetWindow().GetRenderContext();
	}

	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;
		Ref<TextureLibrary> m_TextureLibrary;
		Ref<MaterialLibrary> m_MaterialLibrary;

		Ref<Texture2D> WhiteTexture;
		Ref<Texture2D> BlackTexture;

		EditorLayer* Editor;
	};

	static RendererData* Data = nullptr;
	constexpr static uint32_t RenderCommandQueueCount = 2;
	static RenderCommandQueue* CommandQueue[RenderCommandQueueCount];
	static std::atomic<uint32_t> RenderCommandQueueSubmissionIndex = 0;
	static RenderCommandQueue ResourceFreeQueue[3];

	Renderer::Renderer()
	{
		s_Instance = this;
	}

	void Renderer::Init()
	{
		Data = new RendererData();

		PrimitiveTopology = RenderTopology::Triangles;
		CommandQueue[0] = new RenderCommandQueue();
		CommandQueue[1] = new RenderCommandQueue();

		Data->m_ShaderLibrary = Ref<ShaderLibrary>::Create();

		Renderer::GetShaderLibrary()->Load("Renderer2D_Quad",    "Assets/Shaders/OpenGL/Renderer2D_Quad.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Line",    "Assets/Shaders/OpenGL/Renderer2D_Line.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Debug",   "Assets/Shaders/OpenGL/Renderer2D_Debug.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Screen",  "Assets/Shaders/OpenGL/Renderer2D_Screen.shader");
		Renderer::GetShaderLibrary()->Load("Renderer_Debug",     "Assets/Shaders/OpenGL/Renderer_Debug.shader");
		Renderer::GetShaderLibrary()->Load("Renderer_Skybox",    "Assets/Shaders/OpenGL/Renderer_Skybox.shader");
		Renderer::GetShaderLibrary()->Load("Renderer_Model",     "Assets/Shaders/OpenGL/Renderer_Model.shader");

		Data->m_TextureLibrary = Ref<TextureLibrary>::Create();
		Data->WhiteTexture = Data->m_TextureLibrary->GetWhiteTexture();
		//LoadTextures();

		Data->m_MaterialLibrary = Ref<MaterialLibrary>::Create();

		s_RendererAPI = RendererAPI::Create();
		s_RendererAPI->Init();

		s_Renderer2DAPI = s_RendererAPI->GetRenderer2DAPI();
	}

	void Renderer::Clear()
	{
		s_RendererAPI->Clear();
	}

	void Renderer::BeginFrame()
	{
        Renderer::SwapQueues();
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		CommandQueue[GetRenderQueueIndex()]->Execute();
		s_RendererAPI->EndFrame();
	}

	void Renderer::Shutdown()
	{
		s_RendererAPI->Shutdown();
	}

	void Renderer::SetPrimitiveTopology(const RenderTopology& topology)
	{
		PrimitiveTopology = topology;
		s_RendererAPI->SetPrimitiveTopology(topology);
	}

	void Renderer::SwapQueues()
	{
		RenderCommandQueueSubmissionIndex = (RenderCommandQueueSubmissionIndex + 1) % RenderCommandQueueCount;
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return *CommandQueue[RenderCommandQueueSubmissionIndex];
	}

	uint32_t Renderer::GetRenderQueueIndex()
	{
		return (RenderCommandQueueSubmissionIndex + 1) % RenderCommandQueueCount;
	}

	uint32_t Renderer::GetRenderQueueSubmissionIndex()
	{
		return RenderCommandQueueSubmissionIndex;
	}

	Ref<Framebuffer>& Renderer::GetViewportFramebuffer()
	{
		auto* editor = EditorLayer::Get();
		// Editor is enabled, thus the render target is a texture that the editor is responsible for
		if (editor && editor->IsEnabled())
		{
			return editor->m_ViewportFramebuffer;
		}
		else
		{
			// TODO:
			LK_CORE_ASSERT(false, "Not implemented yet!");
		}
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		LK_CORE_ASSERT(Data->m_ShaderLibrary != nullptr, "ShaderLibrary is nullptr!");
		return Data->m_ShaderLibrary;
	}

	void Renderer::SubmitMesh(Ref<Mesh>& mesh, Ref<Shader>& shader, const glm::mat4& transform)
	{
		s_RendererAPI->SubmitMesh(mesh, shader, transform);
	}

	void Renderer::SubmitImage(const Ref<Image> image)
	{
		s_RendererAPI->SubmitImage(image);
	}

	void Renderer::SubmitImage(const Ref<Image2D> image)
	{
		s_RendererAPI->SubmitImage(image);
	}

	void Renderer::SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID)
	{
		s_RendererAPI->SubmitLine(p0, p1, color, entityID);
	}

	void Renderer::SubmitLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();

		SetPrimitiveTopology(RenderTopology::Lines);
		s_RendererAPI->Draw(vb, ib, shader);
		SetPrimitiveTopology(RenderTopology::Triangles);
	}

	void Renderer::SubmitIndexed(VertexBuffer& vb, unsigned int count)
	{
        vb.Bind();
        //auto ib = vb.GetIndexBuffer();
		//if (ib != nullptr) int indexCount = count ? count : ib->GetCount();
		s_RendererAPI->SubmitIndexed(count);
	}

	void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		s_RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		s_RendererAPI->SubmitQuad(pos, size, color, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
	{
		s_RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, texture, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
	{
		s_RendererAPI->SubmitQuad(pos, size, texture, 0.0f, entityID);
	}

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        s_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
    {
        s_RendererAPI->SubmitQuad(tc.Translation, size, texture, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& color, uint64_t entityID)
    {
        s_RendererAPI->SubmitQuad(tc.Translation, size, texture, color, tc.Rotation2D, entityID);
    }

	// REMOVE
	void Renderer::BeginScene(const SceneCamera& camera)
	{
		s_Renderer2DAPI->BeginScene(camera);
	}

	// REMOVE
	void Renderer::BeginScene(const glm::mat4& viewProjectionMatrix)
	{
		s_Renderer2DAPI->BeginScene(viewProjectionMatrix);
	}

	RendererCapabilities& Renderer::GetCapabilities()
	{
		return s_RendererAPI->GetCapabilities();
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return Application::Get()->GetCurrentFrameIndex();
	}

	uint32_t Renderer::RT_GetCurrentFrameIndex()
	{
		// Swapchain owns the RenderThread frame index
		return Application::Get()->GetWindow().GetSwapChain()->GetCurrentBufferIndex();
	}

	void Renderer::BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass)
	{
		s_RendererAPI->BeginRenderPass(renderCommandBuffer, renderPass, false);
	}

	void Renderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
		s_RendererAPI->EndRenderPass(renderCommandBuffer);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		s_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader> shader, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		s_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, shader, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		s_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, material, vertexBuffer, indexBuffer, transform, indexCount);
	}
	
	Ref<TextureLibrary> Renderer::GetTextureLibrary()
	{
		return Data->m_TextureLibrary;
	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{
		//return Data->m_TextureLibrary->GetWhiteTexture();
		return Data->WhiteTexture;
	}

	Ref<TextureCube> Renderer::GetWhiteTextureCube()
	{
		LK_CORE_ASSERT(false, "Not implemented!");
		return nullptr;
	}

	void Renderer::DrawMesh(Ref<Mesh>& mesh, const Ref<Shader> shader)
	{
		s_RendererAPI->Draw(*mesh->GetMeshSource()->GetVertexBuffer(), *shader);
	}

	void Renderer::RegisterShaderDependency(Ref<Shader> shader, Ref<Material> material)
	{
		s_ShaderDependencies[shader->GetHash()].Materials.push_back(material);
	}

	Ref<MaterialLibrary> Renderer::GetMaterialLibrary()
	{
		return Data->m_MaterialLibrary;
	}

	void Renderer::LoadTextures()
	{
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
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Ice Skybox 
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.Name = "skybox-ice-back-512x512";
            textureSpec.DebugName = "skybox-ice-back-512x512";
            textureSpec.GenerateMips = false;
			textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary::Get()->AddTexture(textureSpec);
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
			TextureLibrary::Get()->AddTexture(textureSpec);
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
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Wood container 2
			textureSpec.Path = "Assets/Textures/container2.png";
			textureSpec.Name = "wood-container2";
			textureSpec.DebugName = "wood-container2";
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Bricks
			textureSpec.Path = "Assets/Textures/bricks_orange.jpg";
			textureSpec.Name = "bricks";
			textureSpec.DebugName = "bricks";
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Åle texture
			textureSpec.Path = "Assets/Textures/Misc/ale_1024x1024.png";
			textureSpec.Name = "ale1024";
			textureSpec.DebugName = "ale1024";
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Lukas texture
			textureSpec.Path = "Assets/Textures/Misc/lukas_1024.jpg";
			textureSpec.Name = "lukas_1024";
			textureSpec.DebugName = "lukas-1024x1024";
			textureSpec.SamplerWrap = TextureWrap::Repeat;
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Metal
            textureSpec.Path = "Assets/Textures/metal.png";
            textureSpec.Name = "metal-ground";
            textureSpec.DebugName = "metal-ground";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Wood
            textureSpec.Name = "wood";
            textureSpec.DebugName = "wood";
            textureSpec.Path = "Assets/Textures/wood.png";
            textureSpec.GenerateMips = true;
            textureSpec.SamplerWrap = TextureWrap::Repeat;
            textureSpec.SamplerFilter = TextureFilter::Linear;
			TextureLibrary::Get()->AddTexture(textureSpec);

			// Skybox
            textureSpec.Name = "skybox-ice-back";
            textureSpec.DebugName = "skybox-ice-back";
            textureSpec.Path = "Assets/Textures/Skybox/back.jpg";
            textureSpec.GenerateMips = false;
            textureSpec.Format = ImageFormat::RGBA32F;
            textureSpec.SamplerFilter = TextureFilter::Nearest;
            textureSpec.SamplerWrap = TextureWrap::Clamp;
			TextureLibrary::Get()->AddTexture(textureSpec);
        }
	}

	void Renderer::SetDepthFunction(const DepthFunction& depthFunc)
	{
		s_RendererAPI->SetDepthFunction(depthFunc);
	}


}
