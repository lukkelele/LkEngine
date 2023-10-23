#include "LKpch.h"
#include "LkEngine/Physics/2D/Physics2D.h"
#include "LkEngine/Application.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(s_ptr<Scene> scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Entity> scene_entities = scene->GetEntities();
        //LOG_DEBUG("CHECKING RAYCAST ENTITY");
        for (auto& entity : scene_entities)
        {
            if (entity.HasComponent<MeshComponent>() && entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                auto& tc = entity.GetComponent<TransformComponent>();
                auto& mesh = entity.GetComponent<MeshComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                //glm::vec2 mouse_pos = Mouse::GetMousePos();
                //glm::vec2 mouse_pos = Mouse::CenterPos;
                glm::vec2 mouse_pos = Mouse::ScaledCenterPos;

                if (EditorLayer::Enabled)
                {
                    auto editor_layer = EditorLayer::Get();
                }

                auto window = Window::Get();
                float window_width = window->GetWidth();
                float window_height = window->GetHeight();

                //mouse_pos.x *= window_width;
                //mouse_pos.y *= window_height;
      
                float quad_width, quad_height;
                quad_width = sprite.Size.x;
                quad_height = sprite.Size.y;
                glm::vec3 quad_pos = tc.Translation;

		        //LOG_DEBUG("Mouse (x, y) : ({}, {})", mouse_pos.x, mouse_pos.y);

                glm::vec2 bottom_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y - quad_height * 0.50f };
                glm::vec2 top_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y + 0.50f * quad_height };
                glm::vec2 top_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y + 0.50f * quad_height };
                glm::vec2 bottom_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y - quad_height * 0.50f };

                //bottom_left.x *= Mouse::Scalers.x;
                //bottom_left.y *= Mouse::Scalers.y;
                //bottom_right.x *= Mouse::Scalers.x;
                //bottom_right.y *= Mouse::Scalers.y;
                //top_left.x *= Mouse::Scalers.x;
                //top_left.y *= Mouse::Scalers.y;
                //top_right.x *= Mouse::Scalers.x;
                //top_right.y *= Mouse::Scalers.y;

                printf("\n");
                printf("(%.2f, %.2f)>-----<(%.2f, %.2f)\n", top_left.x, top_left.y, top_right.x, top_right.y);
                printf("|                           |\n");
                printf("|                           |\n");
                printf("(%.2f, %.2f)>-----<(%.2f,  %.2f)\n", bottom_left.x, bottom_left.y, bottom_right.x, bottom_right.y);
                printf("\n");

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    bool within_x_boundaries = (mouse_pos.x >= bottom_left.x && mouse_pos.x <= top_right.x);
                    bool within_y_boundaries = (mouse_pos.y <= top_left.y && mouse_pos.y >= bottom_right.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        LOG_WARN("CONTACT DETECTED : {} -> ({}, {})", entity.GetName().c_str(), mouse_pos.x, mouse_pos.y);
                        UILayer::SelectedEntityLabel = entity.GetName();
                    }
                    else
                    {
                        //LOG_INFO("NO CONTACT");
                        //UILayer::SelectedEntityLabel = "";
                    }
                }
            }
        }

        Entity e;
        Raycast2DResult res(e, {0, 0}, {0, 0}, 0.0f);
        std::vector<Raycast2DResult> raycast_results;
        raycast_results.push_back(res);
        return raycast_results;
    }

}