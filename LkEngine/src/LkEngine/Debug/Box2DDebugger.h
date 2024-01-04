#pragma once

#include "Debugger2D.h"

#include <box2d/b2_draw.h>


namespace LkEngine {

    class Box2DDebugger : public Debugger2D, public b2Draw
    {
    public:
        Box2DDebugger();
        ~Box2DDebugger() = default;

        // Debugger2D functions
        void DrawQuad(const glm::vec2* vertices, const glm::vec4& color) override;
        void DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) override;
        void DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) override;
        void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color) override;

        // Box2D functions
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
        // TODO: Not implemented yet
        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
        void DrawTransform(const b2Transform& xf);
        void DrawPoint(const b2Vec2& p, float size, const b2Color& color);
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
