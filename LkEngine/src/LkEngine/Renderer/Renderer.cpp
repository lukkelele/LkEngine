#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	int Renderer::DrawMode = GL_TRIANGLES;
	glm::vec4 Renderer::BackgroundColor = { 0.50f, 0.50f, 0.50f, 1.0f };

	s_ptr<RendererAPI> Renderer::m_RendererAPI = nullptr;

	void RendererAPI::SetAPI(RendererAPIType api)
	{
		RendererAPI::m_CurrentRendererAPI = api;
	}

	void Renderer::Init()
	{
		m_RendererAPI = RendererAPI::Create();
		m_RendererAPI->Init();
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::Clear() 
	{
		m_RendererAPI->Clear();
	}

	s_ptr<Renderer2D> Renderer::GetRenderer2D() 
	{ 
		return std::shared_ptr<Renderer2D>(m_RendererAPI->m_Renderer2D); 
	}

	void Renderer::Draw(Entity& entity)
	{
		if (!entity.HasComponent<MeshComponent>())
			return;
		auto& mesh = entity.GetComponent<MeshComponent>();
		mesh.BaseShader->Bind();
		//mesh.VBO->Bind();
		mesh.VBO->Bind();
		auto& ib = mesh.VBO->GetIndexBuffer();
		ib->Bind();
		glDrawElements(DrawMode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(VertexBuffer& vb, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		auto& ib = vb.GetIndexBuffer();
		ib->Bind();
		m_RendererAPI->Draw(vb, shader);
	}

	void Renderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		m_RendererAPI->Draw(vb, ib, shader);
	}

	void Renderer::DrawLines(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		vb.Bind();
		ib.Bind();
		SetDrawMode(LK_DRAW_LINES);
		m_RendererAPI->Draw(vb, ib, shader);
		SetDrawMode(LK_DRAW_TRIANGLES);
	}

	void Renderer::DrawIndexed(VertexBuffer& vb, unsigned int count)
	{
		vb.Bind();
		vb.GetIndexBuffer()->Bind();
		m_RendererAPI->DrawIndexed(count);
	}

	void Renderer::SetDrawMode(int mode)
	{
		DrawMode = mode;
	}

	void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_RendererAPI->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void Renderer::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_RendererAPI->DrawQuad(pos, size, color, entityID);
	}

	void Renderer::DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_RendererAPI->DrawQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

}
