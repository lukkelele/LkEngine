#pragma once

#include "LkEngine/Debug/Debugger2D.h"

#include <box2d/b2_draw.h>


namespace LkEngine {

    class Box2DDebugger : public Debugger2D, public b2Draw
    {
    public:
        Box2DDebugger();
        ~Box2DDebugger() = default;

        // b2Draw functions
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
        void DrawTransform(const b2Transform& xf) override;
        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
    };

    namespace Utils {

        static std::vector<glm::vec2> ConvertB2VecToGlmVec2(const b2Vec2* vertices, int32 vertexCount)
        {
            std::vector<glm::vec2> glmVertices;
            glmVertices.reserve(vertexCount);
            for (int i = 0; i < vertexCount; i++) 
                glmVertices.push_back(glm::vec2(vertices[i].x, vertices[i].y));
            return glmVertices;
        }

    }

}
