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

        static void DrawEntity(Entity& entity)
        {
		    if (!entity.HasComponent<MeshComponent>())
		    	return;
#if 0
		    auto& mesh = entity.GetComponent<MeshComponent>();
		    mesh.BaseShader->Bind();
		    mesh.VAO->Bind();
			mesh.BaseShader->SetUniformMat4f("u_ViewProj", Scene::ActiveScene->GetActiveCamera()->GetViewProjection());
            if (entity.HasComponent<TransformComponent>())
            {
		        auto& transform = entity.GetComponent<TransformComponent>();
		        mesh.BaseShader->SetUniformMat4f("u_TransformMatrix", transform.GetTransform());
            }
		    mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
            s_Renderer->DrawIndexed(*mesh.VAO, mesh.VAO->GetIndexBuffer()->GetCount());
#endif
        }

        // TODO: The way of using static/non-static render functions for submitting drawcalls is just 
        //       weird and unnecessarily difficult, fix 
        static void DrawSprite(Entity& entity)
        {
		    if (!entity.HasComponent<SpriteComponent>() && !entity.HasComponent<TransformComponent>())
		    	return;
	        auto& tc = entity.GetComponent<TransformComponent>();
	        auto& sc = entity.GetComponent<SpriteComponent>();
            auto& renderer2D = Renderer2D::Get();
            //LOG_TRACE("");
            glm::vec3 pos = tc.Translation;
            renderer2D->DrawQuad({ tc.Translation.x, tc.Translation.y }, sc.Size, sc.Color, (uint64_t)entity.GetUUID());
            //renderer2D->DrawQuad(pos, sc.Color, entityID);
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