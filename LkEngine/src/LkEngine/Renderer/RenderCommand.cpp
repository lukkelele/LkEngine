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

#if 0
    void RenderCommand::Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader)
    {
        s_Renderer->Submit(vb, ib, shader);
    }
#endif

    void RenderCommand::DrawIndexed(VertexBuffer& vb)
    {
        auto& ib = vb.GetIndexBuffer();
        ib->Bind();
        s_Renderer->SubmitIndexed(vb, ib->GetCount());
    }

    void RenderCommand::DrawIndexed(VertexBuffer& vb, uint32_t _indexCount)
    {
        vb.Bind();
        auto& ib = vb.GetIndexBuffer();
        int indexCount = _indexCount ? _indexCount : ib->GetCount();
        s_Renderer->SubmitIndexed(vb, indexCount);
    }

    void RenderCommand::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID)
    {
    }

    void RenderCommand::DrawLines(VertexBuffer& vb, uint32_t lineIndexCount)
    {
        vb.Bind();
        glDrawArrays(GL_LINES, 0, lineIndexCount);
    }

    void RenderCommand::DrawSprite(TransformComponent& tc, SpriteComponent& sc, uint32_t entityID)
    {
        glm::vec2 scaledSize = { 
            tc.Scale.x * sc.Size.x, 
            tc.Scale.y * sc.Size.y 
        };
        s_Renderer->SubmitSprite(tc, scaledSize, sc.Color, entityID);
    }

    void RenderCommand::DrawSprite(Entity& entity)
    {
	    if (!entity.HasComponent<SpriteComponent>() && !entity.HasComponent<TransformComponent>())
	    	return;
	    auto& tc = entity.GetComponent<TransformComponent>();
	    auto& sc = entity.GetComponent<SpriteComponent>();
        LOG_TRACE("Sprite Scale -> ({}, {})", tc.Scale.x, tc.Scale.y);
        glm::vec2 scaledSize = { tc.Scale.x * sc.Size.x, tc.Scale.y * sc.Size.y };
        s_Renderer->SubmitSprite(tc, scaledSize, sc.Color, entity.GetUUID());
    }

    void RenderCommand::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint32_t entityID)
    {
        s_Renderer->SubmitQuad(pos, size, color, entityID);
    }

    void RenderCommand::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint32_t entityID)
    {
        s_Renderer->SubmitQuad(pos, size, color, entityID);
    }

    void RenderCommand::DrawQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, uint32_t entityID)
    {
        s_Renderer->SubmitQuad(pos, size, texture, entityID);
    }

    void RenderCommand::DrawQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, uint32_t entityID)
    {
        s_Renderer->SubmitQuad(pos, size, texture, entityID);
    }

    void RenderCommand::DrawGrid(int columns, int rows, const glm::vec4& color, float spacingX, float spacingY)
    {
        auto& window = *Window::Get();
        //auto [windowWidth, windowHeight] = window.GetSize();
        glm::vec2 windowSize = window.GetSize();
        float windowWidth = windowSize.x;
        float windowHeight = windowSize.y;

        glm::vec2 windowPos = { 0.0f, 0.0f };
        if (EditorLayer::IsEnabled())
        {
            //windowPos = EditorLayer::Get()->GetViewportBounds(1, 0);
        }

        float quadWidth = windowWidth / columns;
        float quadHeight = windowHeight / rows;
        glm::vec2 quadSize = { quadWidth, quadHeight };
        glm::vec2 smallerQuadSize = { quadWidth - 10.0f, quadHeight - 10.f };

        //float cursorX = windowPos.x - quadWidth;
        float cursorX = windowPos.x + quadWidth * 0.50f;
        float cursorY = windowPos.y + quadHeight * 0.50f;

        float cursorXBegin = cursorX; // + spacingX;
        float cursorYBegin = cursorY; // + spacingY;

        static glm::vec4 colors[] = {
            { 0.3f,  1.00f, 0.30f, color.a },
            { 1.0f,  0.5f,  0.25f, color.a },
            { 0.75f, 0.80f, 0.20f, color.a },
            { 0.40f, 0.50f, 0.15f, color.a },
            { 0.0f,  1.00f, 0.90f, color.a },
            { 0.75f, 0.80f, 0.30f, color.a },
            { 0.12f, 0.35f, 0.90f, color.a },
            { 0.24f, 0.22f, 0.48f, 1.0f },
        };

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < columns; col++)
            {
                RenderCommand::DrawQuad({ cursorX, cursorY }, quadSize, { 0.0f, 0.0f, 0.0f, 1.0f });

                glm::vec2 smallerQuadPos = { cursorX + spacingX / 2.0f, cursorY + spacingY / 2.0f };
                RenderCommand::DrawQuad(smallerQuadPos, smallerQuadSize, colors[(LK_ARRAYSIZE(colors) * (col + 1 + row)) % 3]);

                cursorX += quadWidth;
            }
            cursorX = cursorXBegin;
            cursorY += quadHeight;
        }
    }


}