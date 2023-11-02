#pragma once

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"


namespace LkEngine {

    class RenderCommand
    {
    public:
        static void Init();
        //static void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader);
        static void DrawSprite(TransformComponent& tc, SpriteComponent& sc, uint64_t entityID = 0);
        static void DrawSprite(Entity& entity); // remove
        static void DrawIndexed(VertexBuffer& vb);
        static void DrawIndexed(VertexBuffer& vb, uint32_t indexCount);
        static void SetLineWidth(float width);
        static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawLines(VertexBuffer& vb, uint32_t lineIndexCount);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawGrid(int columns, int rows, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f }, float spacingX = 10.0f, float spacingY = 10.0f);

    private:
        static u_ptr<Renderer> s_Renderer;
    };

}