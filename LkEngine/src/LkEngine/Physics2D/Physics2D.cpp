#include "LKpch.h"
#include "LkEngine/Physics2D/Physics2D.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(Scene& scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Raycast2DResult> results = {};
        return results;
    }

    std::vector<Raycast2DResult> Physics2D::RaycastFromScreen(Scene& scene)
    {
        std::vector<Raycast2DResult> results = {};
        auto window = Window::Get();
        float windowWidth = (float)window->GetWidth();
        float windowHeight = (float)window->GetHeight();
        glm::vec2 mousePos = Mouse::GetScaledPos();
        //glm::vec2 mousePos = Mouse::ScaledPos;
        //LOG_WARN("Scaled Mouse (Physics2D Getter) - ({} ,{})", mousePos.x, mousePos.y);
		//LOG_ERROR("Scaled Mouse (Physics2D Mouse->Members) - ({} ,{})", Mouse::ScaledPos.x, Mouse::ScaledPos.y);

        auto* editor = EditorLayer::Get();

        std::vector<Entity> sceneEntities = scene.GetEntities();
        for (auto& entity : sceneEntities)
        {
            if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                auto& tc = entity.GetComponent<TransformComponent>();
                auto& sc = entity.GetComponent<SpriteComponent>();
                if (tc.IsStatic() || sc.IsPassthrough())
                    continue;
                auto& cam = *scene.GetActiveCamera();

                glm::vec2 camPos = cam.GetPos();

                float quad_width = tc.Scale.x * sc.Size.x;
                float quad_height = tc.Scale.y * sc.Size.y;
                glm::vec2 quad_pos = { tc.Translation.x, tc.Translation.y };

                // The position is placed in between the upper two points.

                // Place the origin in the middle of the screen
                // This is done by adding half of the window width and height

                // If the editor layer is enabled, adjust the quad pos by taking the editor windows into consideration
            #if 0
                if (editor && editor->IsEnabled())
                {
                    auto editorWindowSize = editor->GetEditorWindowSize();
                    quad_pos.x += editorWindowSize.x * 0.50f + editor->GetLeftSidebarSize().x;
                    quad_pos.y += editorWindowSize.y * 0.50f + editor->GetBottomBarSize().y;
                }
                else
                {
                    quad_pos.x += windowWidth * 0.50f;
                    quad_pos.y += windowHeight * 0.50f;
                }
            #endif

                UI::SetOriginInMiddleOfScreen(quad_pos);

                float angleRad = glm::radians(tc.GetRotation2D());
                glm::mat2 rotMat = {
                    glm::cos(angleRad), -glm::sin(angleRad),
                    glm::sin(angleRad), glm::cos(angleRad)
                };

                // Sprite Points
                glm::vec2 bottom_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y - quad_height };
                glm::vec2 bottom_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y - quad_height };
                glm::vec2 top_left = { quad_pos.x - quad_width * 0.50f, quad_pos.y };
                glm::vec2 top_right = { quad_pos.x + quad_width * 0.50f, quad_pos.y };


                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    // Add camera position to adjust for camera placement in the world
                    bool within_x_boundaries = (mousePos.x + camPos.x >= bottom_left.x) && (mousePos.x + camPos.x <= top_right.x);
                    bool within_y_boundaries = (mousePos.y + camPos.y <= top_left.y) && (mousePos.y + camPos.y >= bottom_right.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        float centerX = tc.Translation.x + quad_width * 0.50f;
                        float centerY = tc.Translation.y + quad_height * 0.50f;
                        LOG_WARN("Hit: {} -> ({}, {})", entity.GetName().c_str(), mousePos.x, mousePos.y);

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