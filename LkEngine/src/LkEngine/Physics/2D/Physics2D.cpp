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
                glm::vec2 mouse_pos;
                mouse_pos = Mouse::GetMousePos();
                //mouse_pos = Mouse::GetMousePosNormalized();
		        //LOG_DEBUG("Mouse NORMALIZED (x, y) : ({}, {})", mouse_pos.x, mouse_pos.y);
                auto window = Application::Get()->GetWindow();
                float window_width, window_height, viewport_width, viewport_height;


                mouse_pos.x -= DockSpace::SidebarLeftSize.x;
                mouse_pos.y += DockSpace::TopBarSize.y;

                viewport_width = window->GetViewportWidth();
                viewport_height = window->GetViewportHeight();
                //window_width = window->GetWidth();
                //window_height = window->GetHeight();
                window_width = DockSpace::CenterWindowSize.x;
                window_height = DockSpace::CenterWindowSize.y;

                mouse_pos.y = window_height - mouse_pos.y;
                float mx = mouse_pos.x / window_width;
                float my = mouse_pos.y / window_height;

                float quad_width, quad_height;
                quad_width = sprite.Size.x;
                quad_height = sprite.Size.y;
                glm::vec3 quad_pos = tc.Translation;

		        //LOG_DEBUG("Mouse (x, y) : ({}, {})", mouse_pos.x, mouse_pos.y);

                glm::vec2 bottom_left = { quad_pos.x, quad_pos.y };
                glm::vec2 top_right = { quad_pos.x + quad_width, quad_pos.y + quad_height };
                glm::vec2 top_left = { quad_pos.x, quad_pos.y + quad_height};
                glm::vec2 bottom_right = { quad_pos.x + quad_width, quad_pos.y };

                glm::vec2 bottom_left_norm = { bottom_left.x / window_width, bottom_left.y / window_width };
                glm::vec2 top_right_norm = { (quad_pos.x + quad_width) / window_width, (quad_pos.y + quad_height) / window_height };

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    bool within_x_boundaries = (mouse_pos.x >= bottom_left_norm.x && mouse_pos.x <= top_right_norm.x);
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

                //if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                //{
				//    EditorLayer::SelectedEntity = {};
                //    UILayer::SelectedEntityLabel = "";
                //}
            }
        }

        Entity e;
        Raycast2DResult res(e, {0, 0}, {0, 0}, 0.0f);
        std::vector<Raycast2DResult> raycast_results;
        raycast_results.push_back(res);
        return raycast_results;
    }

}