#include "LKpch.h"
#include "DebugLayer.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Scene.h"

#include <imgui/imgui.h>


namespace LkEngine {

    DebugLayer::DebugLayer()
        : Layer("DebugLayer")
    {
    }

    void DebugLayer::OnAttach()
    {
    }

    void DebugLayer::OnDetach()
    {
    }

    void DebugLayer::OnUpdate(float ts)
    {
    }

    void DebugLayer::OnImGuiRender()
    {
    }

    namespace Debug {

        Entity CreateDebugSprite(s_ptr<Scene> scene, const glm::vec2& size, const glm::vec2& pos, const glm::vec4& color)
        {
            return CreateDebugSprite(*scene, size, { pos.x, pos.y, 0.0f }, color);
        }

        Entity CreateDebugSprite(Scene* scene, const glm::vec2& size, const glm::vec2& pos, const glm::vec4& color)
        {
            return CreateDebugSprite(*scene, size, { pos.x, pos.y, 0.0f }, color);
        }

        Entity CreateDebugSprite(Scene& scene, const glm::vec2& size, const glm::vec2& pos, const glm::vec4& color)
        {
            return CreateDebugSprite(scene, size, { pos.x, pos.y, 0.0f }, color);
        }

        Entity CreateDebugSprite(Scene& scene, const glm::vec2& size, const glm::vec3& pos, const glm::vec4& color)
        {
            DebugLayer::s_DebugEntities++;
            Entity debugEntity = scene.CreateEntity("DebugEntity-" + std::to_string(DebugLayer::s_DebugEntities));

            SpriteComponent& debugSprite = debugEntity.AddComponent<SpriteComponent>();
            debugSprite.Size = size;
            debugSprite.Color = color;

            TransformComponent& debugTransform = debugEntity.AddComponent<TransformComponent>();
            debugTransform.Translation = pos;

            //debugTransform.Translation.x -= debugSprite.Size.x * 0.50f;
            //debugTransform.Translation.x += debugSprite.Size.y * 0.50f;

            return debugEntity;
        }

    }

}