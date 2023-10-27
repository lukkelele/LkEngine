#pragma once

#include "LkEngine/Renderer/Renderer.h"
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

        static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
        {
            s_Renderer->Draw(va, ib, shader);
        }

        static void Draw(Entity& entity)
        {
		    if (!entity.HasComponent<MeshComponent>())
		    	return;
		    auto& mesh = entity.GetComponent<MeshComponent>();
		    mesh.BaseShader->Bind();
		    mesh.VAO->Bind();
            s_Renderer->DrawIndexed(*mesh.VAO, mesh.VAO->GetIndexBuffer()->GetCount());
        }

        static void DrawIndexed(VertexArray& va)
        {
            auto& ib = va.GetIndexBuffer();
            ib->Bind();
            s_Renderer->DrawIndexed(va, ib->GetCount());
        }

        static void DrawIndexed(VertexArray& va, uint32_t _indexCount)
        {
            va.Bind();
            auto& ib = va.GetIndexBuffer();
            int indexCount = _indexCount ? _indexCount : ib->GetCount();
            s_Renderer->DrawIndexed(va, indexCount);
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