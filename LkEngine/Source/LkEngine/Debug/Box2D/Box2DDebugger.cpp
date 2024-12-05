#include "LKpch.h"
#include "Box2DDebugger.h"
#include "LkEngine/Renderer/Renderer.h"


#if 0 /* Disabled for now. */
namespace LkEngine {

    Box2DDebugger::Box2DDebugger()
    {
        m_Instance = this;
        Debugger2D::SetType(Physics2D::API::Box2D);
    }

    void Box2DDebugger::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) 
    {
        // Quad
        if (vertexCount == 4)
        {
            // Four vertices
            glm::vec2 bottomLeft  = glm::vec2(vertices[0].x, vertices[0].y);
            glm::vec2 bottomRight = glm::vec2(vertices[1].x, vertices[1].y);
            glm::vec2 topRight    = glm::vec2(vertices[2].x, vertices[2].y);
            glm::vec2 topLeft     = glm::vec2(vertices[3].x, vertices[3].y);

            glm::vec2 center  = 0.5f * (bottomLeft + topRight);
            glm::vec2 extents = 0.5f * glm::vec2(topRight.x - bottomLeft.x, topRight.y - bottomLeft.y);

            glm::vec3 pos = { center.x, center.y, -10.0f };
            glm::vec2 size = { 2.0f * extents.x, 2.0f * extents.y };
            glm::vec4 col = { color.r, color.g, color.b, color.a };

            LRenderer::SubmitQuad(pos, size, col, 0);
        }
        // Triangle
        else if (vertexCount == 3)
        {
            throw std::runtime_error("DrawPolygon::Triangle, not implemented");
        }
    }

    void Box2DDebugger::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DrawPolygon(vertices, vertexCount, color);
    }

    void Box2DDebugger::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
    {
    }

    void Box2DDebugger::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
    {
        LK_CORE_TRACE("Box2DDebugger::DrawSolidCircle");
    }

    void Box2DDebugger::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        LK_CORE_TRACE("Box2DDebugger::DrawSegment");
    }

    void Box2DDebugger::DrawTransform(const b2Transform& xf)
    {
        LK_CORE_TRACE("Box2DDebugger::DrawTransform");
    }

    void Box2DDebugger::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
    {
        LK_CORE_TRACE("Box2DDebugger::DrawPoint");
    }

}
#endif
