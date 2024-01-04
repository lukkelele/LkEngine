#pragma once

#include "LkEngine/Core/Base.h"

#include <glm/glm.hpp>


namespace LkEngine {

    class Debugger2D
    {
    public:
        virtual ~Debugger2D() = default;

        static s_ptr<Debugger2D> Create();
        static Debugger2D* Get() { return m_Instance; }

        virtual void DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) = 0;
        virtual void DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) = 0;
        virtual void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color) = 0;

        static ShapeType Determine2DShapeToDraw(int vertexCount)
        {
            switch (vertexCount)
            {
                case 3: return ShapeType::Triangle;
                case 4: return ShapeType::Quad;
                case 6: return ShapeType::Hexagon;
            }
            return ShapeType::Null;
        }

    protected:
        inline static Debugger2D* m_Instance = nullptr;
    };
}