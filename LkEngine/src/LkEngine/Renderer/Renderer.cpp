#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	int Renderer::DrawMode = GL_TRIANGLES;
	glm::vec4 Renderer::BackgroundColor = { 0.50f, 0.50f, 0.50f, 1.0f };
	s_ptr<Renderer2D> Renderer::m_Renderer2D = nullptr;

	void Renderer::Init()
	{
		m_Renderer2D = std::make_shared<Renderer2D>();
		m_Renderer2D->Init();
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::Clear() 
	{
		auto& c = BackgroundColor;
		glClearColor(c.x, c.y, c.z, c.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	s_ptr<Renderer2D> Renderer::GetRenderer2D() 
	{ 
		return m_Renderer2D; 
	}

	void Renderer::Draw(Entity& entity)
	{
		if (!entity.HasComponent<MeshComponent>())
			return;
		auto& mesh = entity.GetComponent<MeshComponent>();
		mesh.BaseShader->Bind();
		mesh.VAO->Bind();
		auto& ib = mesh.VAO->GetIndexBuffer();
		ib->Bind();
		glDrawElements(DrawMode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(VertexArray& va, const Shader& shader) 
	{
		shader.Bind();
		va.Bind();
		auto& ib = va.GetIndexBuffer();
		ib->Bind();
		glDrawElements(DrawMode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		glDrawElements(DrawMode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const MeshComponent& mesh)
	{
		LK_ASSERT(mesh.VAO);
		mesh.BaseShader->Bind();
		mesh.VAO->Bind();
		auto& ib = mesh.VAO->GetIndexBuffer();
		glDrawElements(DrawMode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawTriangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void Renderer::DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) 
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawIndexed(VertexArray& va, unsigned int count)
	{
		glDrawElements(GL_TRIANGLES /*DrawMode*/, count, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::SetDrawMode(int mode)
	{
		DrawMode = mode;
	}

	void Renderer::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void Renderer::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, entityID);
	}

	void Renderer::DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_Renderer2D->DrawQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

	//void Renderer::Submit(std::function<void> func)
	//{
	//}

}
