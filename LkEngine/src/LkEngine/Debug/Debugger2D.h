#pragma once

#include "LkEngine/Core/Base.h"

#include <glm/glm.hpp>


namespace LkEngine {

    class Debugger2D
    {
    public:
        enum class Type { None = 0, Box2D };
    public:
        virtual ~Debugger2D() = default;

        static s_ptr<Debugger2D> Create();
        static Debugger2D* Get() { return m_Instance; }
        static void SetType(Type type) { m_APIType = type; }
        static Type GetType() { return m_APIType; }

        virtual void DrawQuad(const glm::vec2* vertices, const glm::vec4& color) {}
        virtual void DrawPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) {}
        virtual void DrawSolidPolygon(const glm::vec2* vertices, int vertexCount, const glm::vec4& color) {}
        virtual void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color) {}

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
        inline static Type m_APIType = Type::None;
        inline static Debugger2D* m_Instance = nullptr;
    };
}