#pragma once

#include <array>
#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Components.h"
#include <glm/glm.hpp>


namespace LkEngine {

    class Editor
    {
    public:
        Editor() = default;
        ~Editor() = default;

        static glm::vec2 Sprite_GetBottomLeftPoint(SpriteComponent& sc, TransformComponent& tc);
        static glm::vec2 Sprite_GetBottomRightPoint(SpriteComponent& sc, TransformComponent& tc);
        static glm::vec2 Sprite_GetTopLeftPoint(SpriteComponent& sc, TransformComponent& tc);
        static glm::vec2 Sprite_GetTopRightPoint(SpriteComponent& sc, TransformComponent& tc);
        static std::array<glm::vec2, 4> Sprite_GetEdgePoints(SpriteComponent& sc, TransformComponent& tc);

    };

}
