#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	int Renderer::DrawMode = GL_TRIANGLES;
	glm::vec4 Renderer::BackgroundColor = { 0.50f, 0.50f, 0.50f, 1.0f };

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		RendererAPI::m_CurrentRendererAPI = api;
	}

	void Renderer::Init()
	{
		m_RendererAPI = RendererAPI::Create();
		m_RendererAPI->Init();
	}

	void Renderer::Clear()
	{
		m_RendererAPI->Clear();
	}

	void Renderer::BeginFrame()
	{
		m_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		m_RendererAPI->EndFrame();
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::SetDrawMode(int mode)
	{
		DrawMode = mode;
	}


#if 0
	void Renderer::Draw(VertexBuffer& vb, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		auto& ib = vb.GetIndexBuffer();
		ib->Bind();
		m_RendererAPI->Submit(vb, shader);
	}

	void Renderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		m_RendererAPI->Submit(vb, ib, shader);
	}
#endif

	void Renderer::SubmitLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		SetDrawMode(LK_DRAW_LINES);
		m_RendererAPI->Draw(vb, ib, shader);
		SetDrawMode(LK_DRAW_TRIANGLES);
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

	void Renderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_RendererAPI->SubmitQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

}
