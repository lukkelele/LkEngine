#include "LKpch.h"
#include "LkEngine/Physics/2D/Physics2D.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(s_ptr<Scene> scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Entity> scene_entities = scene->GetEntities();
        std::vector<Raycast2DResult> results = {};
        for (auto& entity : scene_entities)
        {
            if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                glm::vec2 mousePos = Mouse::ScaledPos;

                auto& tc = entity.GetComponent<TransformComponent>();
                auto& sc = entity.GetComponent<SpriteComponent>();
                auto& cam = *scene->GetActiveCamera();
                glm::vec2 camPos = cam.GetPos();

                if (EditorLayer::Enabled)
                {
                    auto editor_layer = EditorLayer::Get();
                }
                float quad_width = sc.Size.x;
                float quad_height = sc.Size.y;
                glm::vec3 quad_pos = tc.Translation;

                glm::vec2 bottom_left = { quad_pos.x, quad_pos.y };
                glm::vec2 bottom_right = { quad_pos.x + quad_width, quad_pos.y };
                glm::vec2 top_right = { quad_pos.x + quad_width, quad_pos.y + quad_height };
                glm::vec2 top_left = { quad_pos.x - quad_width, quad_pos.y + quad_height };

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    bool within_x_boundaries = ((mousePos.x + camPos.x) >= bottom_left.x && (mousePos.x + camPos.x) <= top_right.x);
                    bool within_y_boundaries = ((mousePos.y + camPos.y) <= top_left.y && (mousePos.y + camPos.y) >= bottom_right.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        LOG_WARN("Hit: {} -> ({}, {})", entity.GetName().c_str(), mousePos.x, mousePos.y);
                        EditorLayer::SelectedEntityID = entity.GetComponent<IDComponent>().ID;
                        float x = point1.x - point0.x;
                        float y = point1.y - point0.y;
                        glm::vec2 intersection = { point1.x - point0.x, point1.y - point0.y };
                        float distance = sqrt(pow(x, 2) + pow(y, 2));
                        results.push_back(Raycast2DResult(entity, intersection, { 0, 0 }, distance));
                    }
                    else
                    {
                    }
                }
            }
        }
        return results;
    }

    std::vector<Raycast2DResult> Physics2D::RaycastFromScreen(s_ptr<Scene> scene)
    {
        std::vector<Entity> scene_entities = scene->GetEntities();
        std::vector<Raycast2DResult> results = {};
        for (auto& entity : scene_entities)
        {
            //if (entity.HasComponent<MeshComponent>() && entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                glm::vec2 mousePos = Mouse::GetScaledPos();

                auto& tc = entity.GetComponent<TransformComponent>();
                auto& sc = entity.GetComponent<SpriteComponent>();
                auto& cam = *scene->GetActiveCamera();
                glm::vec2 camPos = cam.GetPos();

                if (EditorLayer::Enabled)
                {
                    auto editor_layer = EditorLayer::Get();
                }
                float quad_width, quad_height;
                quad_width = tc.Scale.x * sc.Size.x;
                quad_height = tc.Scale.y * sc.Size.y;
                //glm::vec3 quad_pos = tc.Translation;
                glm::vec2 quad_pos = { tc.Translation.x, tc.Translation.y };

                //float rotAngle = tc.GetRotation2D();
                //glm::mat2 rotMat = glm::mat2(glm::cos(glm::radians(rotAngle)), -glm::sin(glm::radians(rotAngle)),
                //    glm::sin(glm::radians(rotAngle)), glm::cos(glm::radians(rotAngle)));

                float angleDeg = glm::degrees(tc.GetRotation2D());
                float angleRad = glm::radians(tc.GetRotation2D());
                //LOG_DEBUG("Angle: {}", angleDeg);
                glm::mat2 rotMat = {
                    glm::cos(angleRad), -glm::sin(angleRad),
                    glm::sin(angleRad), glm::cos(angleRad)
                };

                glm::vec2 bottom_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y - quad_height * 0.50f };
                glm::vec2 bottom_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y - quad_height * 0.50f };
                glm::vec2 top_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y + quad_height * 0.50f };
                glm::vec2 top_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y + quad_height * 0.50f};

                
                //LOG_TRACE("BottomLeft: ({}, {})", bottom_left.x, bottom_left.y);
                float centerX = tc.Translation.x + quad_width * 0.50f;
                float centerY = tc.Translation.y + quad_height * 0.50f;

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    bool within_x_boundaries = ((mousePos.x + camPos.x >= bottom_left.x) && ((mousePos.x + camPos.x) <= top_right.x));
                    bool within_y_boundaries = ((mousePos.y + camPos.y) <= top_left.y && (mousePos.y + camPos.y >= bottom_right.y));
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        //LOG_WARN("Hit: {} -> ({}, {})", entity.GetName().c_str(), mousePos.x, mousePos.y);
                        float x = centerX - mousePos.x;
                        float y = centerY - mousePos.y;
                        glm::vec2 intersection = { x, y };
                        float distance = sqrt(pow(x, 2) + pow(y, 2));
                        results.push_back(Raycast2DResult(entity, intersection, { 0, 0 }, distance));
                    }
                }
            }
        }
        return results;
    }

}