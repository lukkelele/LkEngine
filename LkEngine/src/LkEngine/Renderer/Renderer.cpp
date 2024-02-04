#include "LKpch.h"
#include "Renderer.h"

//#include "RendererAPI.h"
#include "Texture.h"
#include "GraphicsContext.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		RendererAPI::m_CurrentRendererAPI = api;
	}

	Ref<GraphicsContext> Renderer::GetContext()
	{
		return Application::Get()->GetWindow().GetContext();
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
		m_Instance = this;
	}

	void Renderer::Init()
	{
		Data = new RendererData();

		DrawMode = RendererDrawMode::Triangles;
		CommandQueue[0] = new RenderCommandQueue();
		CommandQueue[1] = new RenderCommandQueue();

		Data->m_ShaderLibrary = Ref<ShaderLibrary>::Create();

		Data->m_TextureLibrary = Ref<TextureLibrary>::Create();
		Data->m_TextureLibrary->Init();

		Data->m_MaterialLibrary = Ref<MaterialLibrary>::Create();
		Data->m_MaterialLibrary->Init();

		Data->WhiteTexture = Data->m_TextureLibrary->GetWhiteTexture2D();

		Renderer::GetShaderLibrary()->Load("Renderer2D_Quad",   "assets/Shaders/OpenGL/Renderer2D_Quad.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Line",   "assets/Shaders/OpenGL/Renderer2D_Line.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Debug",  "assets/Shaders/OpenGL/Renderer2D_Debug.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Screen", "assets/Shaders/OpenGL/Renderer2D_Screen.shader");
		Renderer::GetShaderLibrary()->Load("BasicModel",        "assets/Shaders/OpenGL/Renderer_Model.shader");

		m_RendererAPI = RendererAPI::Create();
		m_RendererAPI->Init();

		m_Renderer2DAPI = m_RendererAPI->GetRenderer2DAPI();
	}

	void Renderer::Clear()
	{
		m_RendererAPI->Clear();
	}

	void Renderer::BeginFrame()
	{
        Renderer::SwapQueues();
		m_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		CommandQueue[GetRenderQueueIndex()]->Execute();
		m_RendererAPI->EndFrame();
	}

	void Renderer::Shutdown()
	{
		m_RendererAPI->Shutdown();
	}

	void Renderer::SetDrawMode(const RendererDrawMode& drawMode)
	{
		DrawMode = drawMode;
		m_RendererAPI->SetDrawMode(drawMode);
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

	void Renderer::SubmitImage(const Ref<Image> image)
	{
		m_RendererAPI->SubmitImage(image);
	}

	void Renderer::SubmitImage(const Ref<Image2D> image)
	{
		m_RendererAPI->SubmitImage(image);
	}

	void Renderer::SubmitLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID)
	{
		m_RendererAPI->SubmitLine(p0, p1, color, entityID);
	}

	void Renderer::SubmitLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();

		SetDrawMode(RendererDrawMode::Lines);
		m_RendererAPI->Draw(vb, ib, shader);
		SetDrawMode(RendererDrawMode::Triangles);
	}

	void Renderer::SubmitIndexed(VertexBuffer& vb, unsigned int count)
	{
        vb.Bind();
        //auto ib = vb.GetIndexBuffer();
		//if (ib != nullptr) int indexCount = count ? count : ib->GetCount();
		m_RendererAPI->SubmitIndexed(count);
	}

	void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad(pos, size, color, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, texture, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad(pos, size, texture, 0.0f, entityID);
	}

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad(tc.Translation, size, texture, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& color, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad(tc.Translation, size, texture, color, tc.Rotation2D, entityID);
    }

	std::string Renderer::GetDrawModeStr()
	{
		switch (DrawMode)
		{
			case RendererDrawMode::Triangles: return "Triangles";
			case RendererDrawMode::Lines:     return "Lines";
			default:                          return "Unknown_DrawMode";
		}
	}

	// REMOVE
	void Renderer::BeginScene(const SceneCamera& camera)
	{
		m_Renderer2DAPI->BeginScene(camera);
	}

	// REMOVE
	void Renderer::BeginScene(const glm::mat4& viewProjectionMatrix)
	{
		m_Renderer2DAPI->BeginScene(viewProjectionMatrix);
	}

	RendererCapabilities& Renderer::GetCapabilities()
	{
		return m_RendererAPI->GetCapabilities();
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
		m_RendererAPI->BeginRenderPass(renderCommandBuffer, renderPass, false);
	}

	void Renderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
		m_RendererAPI->EndRenderPass(renderCommandBuffer);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		m_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Shader> shader, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		m_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, shader, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount /*= 0*/)
	{
		m_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, material, vertexBuffer, indexBuffer, transform, indexCount);
	}
	
	Ref<TextureLibrary> Renderer::GetTextureLibrary()
	{
		return Data->m_TextureLibrary;
	}

}
