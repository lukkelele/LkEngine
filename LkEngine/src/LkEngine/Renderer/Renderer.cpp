#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		RendererAPI::m_CurrentRendererAPI = api;
	}

	struct RendererData
	{
		s_ptr<ShaderLibrary> m_ShaderLibrary;

		s_ptr<Texture2D> WhiteTexture;
		s_ptr<Texture2D> BlackTexture;
	};

	static RendererData* Data = nullptr;
	constexpr static uint32_t RenderCommandQueueCount = 2;
	static RenderCommandQueue* CommandQueue[RenderCommandQueueCount];
	static std::atomic<uint32_t> RenderCommandQueueSubmissionIndex = 0;

	Renderer::Renderer()
	{
		m_Instance = this;
		RenderCommand::SetRenderer(this);
	}

	void Renderer::Init()
	{
		DrawMode = RendererDrawMode::Triangles;
		CommandQueue[0] = new RenderCommandQueue();
		CommandQueue[1] = new RenderCommandQueue();

		Data = new RendererData();
		Data->m_ShaderLibrary = ShaderLibrary::Create();
		m_TextureLibrary = TextureLibrary::Create("assets/textures");
		m_TextureLibrary->Init();

		uint32_t whiteTextureData = 0xFFFFFFFF;
		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA;
		spec.Width = 1;
		spec.Height = 1;
		Data->WhiteTexture = Texture2D::Create(spec, Buffer(&whiteTextureData, sizeof(uint32_t)));

		constexpr uint32_t blackTextureData = 0xFF000000;
		Data->BlackTexture = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));

		Renderer::GetShaderLibrary()->Load("Renderer2D_Quad", "assets/shaders/Renderer2D_Quad.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Line", "assets/shaders/Renderer2D_Line.shader");

		LOG_DEBUG("Creating Renderer API");
		m_RendererAPI = RendererAPI::Create();
		m_RendererAPI->Init();
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

	s_ptr<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return Data->m_ShaderLibrary;
	}

	void Renderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color)
	{
		m_RendererAPI->SubmitLine(p1, p2, color);
	}

	void Renderer::SubmitLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		//SetDrawMode(LK_DRAWMODE_LINES);
		SetDrawMode(RendererDrawMode::Lines);
		m_RendererAPI->Draw(vb, ib, shader);
		SetDrawMode(RendererDrawMode::Triangles);
		//SetDrawMode(LK_DRAWMODE_TRIANGLES);
	}

	void Renderer::SubmitIndexed(VertexBuffer& vb, unsigned int count)
	{
		vb.Bind();
		vb.GetIndexBuffer()->Bind();
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

	void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad({ pos.x, pos.y, 0.0f }, size, texture, entityID);
	}

	void Renderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID)
	{
		m_RendererAPI->SubmitQuad(pos, size, texture, entityID);
	}

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, s_ptr<Texture> texture, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, texture, tc.Rotation2D, entityID);
    }

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& color, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, texture, color, tc.Rotation2D, entityID);
    }

	std::string Renderer::GetDrawModeStr()
	{
		switch (DrawMode)
		{
			case RendererDrawMode::Triangles: return "Triangles";
			case RendererDrawMode::Lines:     return "Lines";
			default:                          return "Unknown_Draw_Mode";
		}
	}

	void Renderer::BeginScene(const glm::mat4& viewProjection)
	{

	}

}
