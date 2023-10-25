#pragma once

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"


namespace LkEngine {

    class RenderCommand
    {
    public:
        static void Init()
        {
            s_Renderer->Init();
        }

        static void Draw(Entity& entity)
        {
		    if (!entity.HasComponent<MeshComponent>())
		    	return;
		    auto& mesh = entity.GetComponent<MeshComponent>();
		    mesh.BaseShader->Bind();
		    mesh.VAO->Bind();
            s_Renderer->DrawIndexed(*mesh.VAO);
        }

        static void DrawIndexed(VertexArray& va)
        {
            auto& ib = va.GetIndexBuffer();
            ib->Bind();
            s_Renderer->DrawIndexed(va);
        }

        static void DrawIndexed(VertexArray& va, uint32_t indexCount)
        {
            auto& ib = va.GetIndexBuffer();
            uint32_t count = indexCount ? indexCount : va.GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        }


        static void SetLineWidth(float width)
        {
            glLineWidth(width);
        }

        static void DrawLines(VertexArray& va, uint32_t lineVertexCount)
        {

        }



    private:
        static u_ptr<Renderer> s_Renderer;
    };

}