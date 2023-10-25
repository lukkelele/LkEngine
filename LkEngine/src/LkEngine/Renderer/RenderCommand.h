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

    private:
        static u_ptr<Renderer> s_Renderer;
    };

}