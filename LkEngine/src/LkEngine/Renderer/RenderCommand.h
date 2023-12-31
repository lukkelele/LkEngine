#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"

// TODO:
// - Begin/End scene functions should not be of two different types, a generalized version should be used
//   to remove the need for a '2D' version of the begin/end function

namespace LkEngine {

    class RenderCommand
    {
    public:
        static void Init();
        static void DrawSprite(TransformComponent& tc, SpriteComponent& sc, uint32_t entityID = 0);
        static void DrawSprite(TransformComponent& tc, SpriteComponent& sc, MaterialComponent& mc, uint32_t entityID = 0);
        static void DrawSprite(TransformComponent& tc, SpriteComponent& sc, s_ptr<Texture> texture, uint32_t entityID = 0);
        static void DrawIndexed(VertexBuffer& vb);
        static void DrawIndexed(VertexBuffer& vb, uint32_t indexCount);
        static void SetLineWidth(float width);
        static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint32_t entityID = 0);
        static void DrawLines(VertexBuffer& vb, uint32_t lineIndexCount);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint32_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint32_t entityID = 0);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, uint32_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, uint32_t entityID = 0);
        static void DrawGrid(int columns, int rows, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f }, float spacingX = 10.0f, float spacingY = 10.0f);
        static void DrawCircle(const glm::vec2& p0);

    private:
        //static u_ptr<Renderer> m_Renderer;
        //inline static u_ptr<Renderer> m_Renderer;
        //inline static s_ptr<Renderer> m_Renderer = nullptr;
        inline static Renderer* m_Renderer = nullptr;
    };

}