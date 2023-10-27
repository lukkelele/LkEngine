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
            va.Bind();
            LOG_DEBUG("RenderCommand::DrawIndexed -> {}", indexCount);
            //GL_CALL(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
            GL_CALL(glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr));
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