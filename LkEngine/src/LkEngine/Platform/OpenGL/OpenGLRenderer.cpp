#include "LKpch.h"
#include "LkEngine/Platform/OpenGL/OpenGLRenderer.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    void OpenGLRenderer::Init()
    {
		m_Renderer2D = std::make_shared<Renderer2D>();
		m_Renderer2D->Init();
		LK_ASSERT(m_Renderer2D);
    }

    void OpenGLRenderer::Shutdown()
    {
    }

    void OpenGLRenderer::BeginFrame()
    {
		Clear();
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

	void OpenGLRenderer::SetDrawMode(const RendererDrawMode& mode)
	{
		switch (mode)
		{
			case RendererDrawMode::Lines:
				m_DrawMode = GL_LINES;
				break;
			case RendererDrawMode::Triangles:
				m_DrawMode = GL_TRIANGLES;
				break;
		}
	}

	void OpenGLRenderer::Draw(VertexBuffer& vb, const Shader& shader)
	{
		glDrawElements(m_DrawMode, vb.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader) 
	{
		glDrawElements(m_DrawMode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color)
	{
		//glDrawElements(m_DrawMode, indexCount, GL_UNSIGNED_INT, nullptr);
		m_Renderer2D->DrawLine(p1, p2, color);
	}

	void OpenGLRenderer::SubmitIndexed(unsigned int indexCount)
	{
		glDrawElements(m_DrawMode, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, rotation, entityID);
	}

	void OpenGLRenderer::SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID)
	{
		m_Renderer2D->DrawQuad(pos, size, texture, rotation, entityID);
	}

	void OpenGLRenderer::SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID)
    {
        m_Renderer2D->DrawQuad({ tc.Translation.x, tc.Translation.y }, size, color, tc.Rotation2D, entityID);
    }

}
