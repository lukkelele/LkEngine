#include "LKpch.h"
#include "LkEngine/Renderer/RenderCommand.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    u_ptr<Renderer> RenderCommand::s_Renderer;

    void RenderCommand::Init()
    {
        s_Renderer->Init();
    }

    void RenderCommand::SetLineWidth(float width)
    {
        glLineWidth(width);
    }

    void RenderCommand::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
    {
        s_Renderer->Draw(va, ib, shader);
    }

    void RenderCommand::DrawIndexed(VertexArray& va)
    {
        auto& ib = va.GetIndexBuffer();
        ib->Bind();
        s_Renderer->DrawIndexed(va, ib->GetCount());
    }

    void RenderCommand::DrawIndexed(VertexArray& va, uint32_t _indexCount)
    {
        va.Bind();
        auto& ib = va.GetIndexBuffer();
        int indexCount = _indexCount ? _indexCount : ib->GetCount();
        s_Renderer->DrawIndexed(va, indexCount);
    }

    void RenderCommand::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID)
    {
    }

    void RenderCommand::DrawLines(VertexArray& va, uint32_t lineIndexCount)
    {
        va.Bind();
        glDrawArrays(GL_LINES, 0, lineIndexCount);
    }

    void RenderCommand::DrawSprite(TransformComponent& tc, SpriteComponent& sc, uint64_t entityID)
    {
        glm::vec2 scaledSize = { tc.Scale.x * sc.Size.x, tc.Scale.y * sc.Size.y };
        s_Renderer->DrawSprite(tc, sc.Size, sc.Color, entityID);
    }

    void RenderCommand::DrawSprite(Entity& entity)
    {
	    if (!entity.HasComponent<SpriteComponent>() && !entity.HasComponent<TransformComponent>())
	    	return;
	    auto& tc = entity.GetComponent<TransformComponent>();
	    auto& sc = entity.GetComponent<SpriteComponent>();
        glm::vec2 scaledSize = { tc.Scale.x * sc.Size.x, tc.Scale.y * sc.Size.y };
        s_Renderer->DrawSprite(tc, sc.Size, sc.Color, entity.GetUUID());
    }

    void RenderCommand::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        s_Renderer->DrawQuad(pos, size, color, entityID);
    }

    void RenderCommand::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID)
    {
        s_Renderer->DrawQuad(pos, size, color, entityID);
    }

    void RenderCommand::DrawGrid(int columns, int rows, const glm::vec4& color, float spacingX, float spacingY)
    {
        auto& window = *Window::Get();
        auto [windowWidth, windowHeight] = window.GetSize();
        float quadWidth = (windowWidth - columns * spacingX) / columns;
        float quadHeight = (windowHeight - rows * spacingY) / rows;
        //float quadWidth = windowWidth / columns;
        //float quadHeight = windowHeight / rows;
        glm::vec2 quadSize = { quadWidth, quadHeight };

        float cursorX = 0.0f;
        float cursorY = 0.0f;
        glm::vec2 windowPos = { 0.0f, 0.0f };
        if (EditorLayer::IsEnabled())
        {
            windowPos = EditorLayer::Get()->GetViewportPos();
        }
        cursorX = windowPos.x;
        cursorY = windowPos.y;
        //cursorX += spacingX;
        //cursorY += spacingY;
        float cursorXBegin = windowPos.x; // + spacingX;
        float cursorYBegin = windowPos.y; // + spacingY;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < columns; col++)
            {
                //cursorX += spacingX * 0.50f;
                cursorX += spacingX * 1.00f;
                Renderer::DrawQuad({ cursorX, cursorY }, quadSize, color);
                cursorX += quadWidth;
                //cursorX += (quadWidth + spacingX);
                //cursorX += (quadWidth + 2 * spacingX);
            }
            //cursorX = 0.0f;
            cursorX = cursorXBegin;
            cursorY += (quadHeight + spacingY);
        }
        
        //Renderer::DrawQuad();
    }


}