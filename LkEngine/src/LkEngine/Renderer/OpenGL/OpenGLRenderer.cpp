#include "LKpch.h"
#include "LkEngine/Renderer/OpenGL/OpenGLRenderer.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	s_ptr<Renderer2D> RendererAPI::m_Renderer2D = nullptr;

    void OpenGLRenderer::Init()
    {
		m_Renderer2D = std::make_shared<Renderer2D>();
		m_Renderer2D->Init();
    }

    void OpenGLRenderer::Shutdown()
    {

    }

    void OpenGLRenderer::BeginFrame()
    {
    }

    void OpenGLRenderer::EndFrame()
    {
    }

	void OpenGLRenderer::Clear()
	{
		auto& c = Renderer::BackgroundColor;
		glClearColor(c.r, c.g, c.b, c.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderer::Draw(VertexBuffer& vb, const Shader& shader)
	{
		glDrawElements(Renderer::DrawMode, vb.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		glDrawElements(Renderer::DrawMode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawIndexed(unsigned int indexCount)
	{
		glDrawElements(Renderer::DrawMode, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void OpenGLRenderer::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, entityID);
	}

	void OpenGLRenderer::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
	}

	void OpenGLRenderer::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
	}

	void OpenGLRenderer::DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_Renderer2D->DrawQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

}
