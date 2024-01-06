#pragma once

#include <glm/glm.hpp>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Physics2D/Physics2D.h"


namespace LkEngine {

    // Forward declaration
    class Scene;

    class Debugger2D
    {
    public:
        enum DrawMode2D : int
        {
            None           = 1 << 0,
            Shape          = 1 << 1,
            Joints         = 1 << 2,
        };
    public:
        virtual ~Debugger2D() = default;

        static s_ptr<Debugger2D> Create();
        static Debugger2D* Get() { return m_Instance; }
        static void SetType(Physics2D::API type) { m_API = type; }
        static Physics2D::API GetCurrentAPI() { return m_API; }

        static ShapeType Determine2DShapeToDraw(int vertexCount)
        {
            switch (vertexCount)
            {
                case 3: return ShapeType::Triangle;
                case 4: return ShapeType::Quad;
            }
            return ShapeType::Null;
        }

    protected:
        inline static Physics2D::API m_API = Physics2D::API::None;
        inline static Debugger2D* m_Instance = nullptr;
    };
}