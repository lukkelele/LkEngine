#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	int Renderer::DrawMode = GL_TRIANGLES;
	glm::vec4 Renderer::BackgroundColor = { 0.50f, 0.50f, 0.50f, 1.0f };

	void Renderer::Init()
	{
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
		//ib->Unbind();
		//mesh.VAO->Unbind();
		//mesh.BaseShader->Unbind();
	}

	void Renderer::Draw(VertexArray& va, const Shader& shader) 
	{
		shader.Bind();
		va.Bind();
		auto& ib = va.GetIndexBuffer();
		ib->Bind();
		glDrawElements(DrawMode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
		//va.Unbind();
		//ib->Unbind();
		//shader.Unbind();
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

	void Renderer::DrawIndexed(VertexArray& va)
	{
		va.Bind();
		auto& ib = va.GetIndexBuffer();
		ib->Bind();
		glDrawElements(GL_LINES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::SetDrawMode(int mode)
	{
		DrawMode = mode;
	}

}
