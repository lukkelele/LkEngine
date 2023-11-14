#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	int Renderer::DrawMode = GL_TRIANGLES;

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

	void Renderer::SubmitLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		SetDrawMode(LK_DRAWMODE_LINES);
		m_RendererAPI->Draw(vb, ib, shader);
		SetDrawMode(LK_DRAWMODE_TRIANGLES);
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
