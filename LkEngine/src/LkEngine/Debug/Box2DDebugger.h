#pragma once
#ifdef LK_PHYSICS_API_BOX2D

#include "Debugger2D.h"

#include <box2d/b2_draw.h>


namespace LkEngine {

    class Box2DDebugger : public Debugger2D, public b2Draw
    {
    public:
        Box2DDebugger();
        ~Box2DDebugger() = default;

        // Debugger2D functions
        void DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) override;
        void DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) override;
        void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color) override;

        // Box2D functions
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

    };

}

#endif // LK_PHYSICS_API_BOX2D