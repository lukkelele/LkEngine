#include "LKpch.h"
#include "LkEngine/Physics/2D/Physics2D.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(s_ptr<Scene> scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Entity> scene_entities = scene->GetEntities();
        for (auto& entity : scene_entities)
        {
            if (entity.HasComponent<MeshComponent>() && entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                auto& tc = entity.GetComponent<TransformComponent>();
                auto& mesh = entity.GetComponent<MeshComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
            }
        }
        Entity e;
        Raycast2DResult res(e, {0, 0}, {0, 0}, 0.0f);
        std::vector<Raycast2DResult> raycast_results;
        raycast_results.push_back(res);
        return raycast_results;
    }

}