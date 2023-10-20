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
                //mouse_pos = Mouse::GetMousePosDockWindow();
                auto window = Application::Get()->GetWindow();
                float window_width, window_height, viewport_width, viewport_height;
                viewport_width = window->GetViewportWidth();
                viewport_height = window->GetViewportHeight();
                window_width = window->GetWidth();
                window_height = window->GetHeight();

                mouse_pos.x -= DockSpace::SidebarLeftSize.x;
                mouse_pos.y -= DockSpace::TopBarSize.y;
                //mouse_pos.y *= -1; // Flip because we count from (0, 0) in bottom left corner
                //LOG_DEBUG("SidebarLeftSize.x == {}", DockSpace::SidebarLeftSize.x);
                //LOG_DEBUG("BottomBarSize.y == {}", DockSpace::BottomBarSize.y);
                //LOG_DEBUG("TopBarSize.y == {}", DockSpace::TopBarSize.y);

		        //LOG_DEBUG("Mouse 2 (x, y) : ({}, {})", mouse_pos.x, mouse_pos.y);
                //mouse_pos.x -=
                float quad_width, quad_height, quad_pos_x, quad_pos_y;
                quad_width = sprite.Size.x;
                quad_height = sprite.Size.y;
                quad_pos_x = tc.Translation.x; // Min/Lowest x point
                quad_pos_y = tc.Translation.y; // Min/Lowest y point

                //LOG_TRACE("Quad - Pos: ({}, {})", quad_pos_x, quad_pos_y);
		        //LOG_DEBUG("Mouse (x, y) : ({}, {})", mouse_pos.x, mouse_pos.y);

                glm::vec2 bottom_left = { quad_pos_x, window_height - quad_pos_y };
                glm::vec2 top_right = { quad_pos_x + quad_width, window_height - (quad_pos_y + quad_height) };
                glm::vec2 top_left = { top_right.x - quad_width, top_right.y};
                glm::vec2 bottom_right = { quad_pos_x + quad_width, window_height - quad_height };

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    bool within_x_boundaries = (mouse_pos.x >= bottom_left.x && mouse_pos.x <= top_right.x);
                    bool within_y_boundaries = (mouse_pos.y > top_left.y && mouse_pos.y < bottom_left.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        LOG_WARN("CONTACT DETECTED : {} -> ({}, {})", entity.GetName().c_str(), mouse_pos.x, mouse_pos.y);
                        UILayer::SelectedEntityLabel = entity.GetName();
                    }
                    else
                    {
                        //LOG_INFO("NO CONTACT");
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