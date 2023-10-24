#include "LKpch.h"
#include "LkEngine/Editor/Editor.h"


namespace LkEngine {

    glm::vec2 Editor::Sprite_GetBottomLeftPoint(SpriteComponent& sc, TransformComponent& tc)
    {
        float quadWidth = sc.Size.x;
        float quadHeight = sc.Size.y;
        glm::vec3 quadPos = tc.Translation;
        glm::vec2 bottomLeft = { quadPos.x, quadPos.y };
        return bottomLeft;
    }

    glm::vec2 Editor::Sprite_GetBottomRightPoint(SpriteComponent& sc, TransformComponent& tc)
    {
        float quadWidth = sc.Size.x;
        float quadHeight = sc.Size.y;
        glm::vec3 quadPos = tc.Translation;
        glm::vec2 bottomRight = { quadPos.x + quadWidth, quadPos.y };
        return bottomRight;
    }

    glm::vec2 Editor::Sprite_GetTopLeftPoint(SpriteComponent& sc, TransformComponent& tc)
    {
        float quadWidth = sc.Size.x;
        float quadHeight = sc.Size.y;
        glm::vec3 quadPos = tc.Translation;
        glm::vec2 topLeft = { quadPos.x, quadPos.y + quadHeight };
        return topLeft;
    }

    glm::vec2 Editor::Sprite_GetTopRightPoint(SpriteComponent& sc, TransformComponent& tc)
    {
        float quadWidth = sc.Size.x;
        float quadHeight = sc.Size.y;
        glm::vec3 quadPos = tc.Translation;
        glm::vec2 topRight = { quadPos.x + quadWidth, quadPos.y + quadHeight };
        return topRight;
    }

    std::array<glm::vec2, 4> Editor::Sprite_GetEdgePoints(SpriteComponent& sc, TransformComponent& tc)
    {
        std::array<glm::vec2, 4> points = {
            Sprite_GetBottomLeftPoint(sc, tc),
            Sprite_GetBottomRightPoint(sc, tc),
            Sprite_GetTopLeftPoint(sc, tc),
            Sprite_GetTopRightPoint(sc, tc),
        };
        return points;
    }

}