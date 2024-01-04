#include "LKpch.h"
#include "Box2DDebugger.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    Box2DDebugger::Box2DDebugger()
    {
        m_Instance = this;
    }

    // Debugger2D functions are just invoking the b2Draw functions
    void Box2DDebugger::DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color)
    {
        DrawPolygon(&b2Vec2(vertices->x, vertices->y), (int32)vertexCount, const_cast<b2Color&&>(b2Color(color.r, color.g, color.b, color.a)));
    }

    void Box2DDebugger::DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color)
    {
        DrawSolidPolygon(&b2Vec2(vertices->x, vertices->y), (int32)vertexCount, const_cast<b2Color&&>(b2Color(color.r, color.g, color.b, color.a)));
    }

    void Box2DDebugger::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color)
    {
        DrawCircle(b2Vec2(center.x, center.y), radius, const_cast<b2Color&&>(b2Color(color.r, color.g, color.b, color.a)));
    }

    // -------------------------------------------------------------------------
    // Box2D
    // -------------------------------------------------------------------------

    void Box2DDebugger::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) 
    {
    }

    void Box2DDebugger::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
    }

    void Box2DDebugger::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
    {
    }

}