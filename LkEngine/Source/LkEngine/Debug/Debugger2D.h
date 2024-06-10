#pragma once

#include <glm/glm.hpp>

#include "LkEngine/Core/Base.h"
#include "LkEngine/Physics2D/Physics2D.h"


namespace LkEngine {

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

        static TSharedPtr<Debugger2D> Create();

        FORCEINLINE static void SetType(Physics2D::API InAPI) { m_API = InAPI; }
        FORCEINLINE static Physics2D::API GetCurrentAPI() { return m_API; }

        static Debugger2D* Get() { return m_Instance; }

        FORCEINLINE static EShapeType Determine2DShapeToDraw(const int VertexCount)
        {
            switch (VertexCount)
            {
                case 3: return EShapeType::Triangle;
                case 4: return EShapeType::Quad;
            }
            return EShapeType::Null;
        }

    protected:
        inline static Physics2D::API m_API = Physics2D::API::None;
        inline static Debugger2D* m_Instance = nullptr;
    };

}