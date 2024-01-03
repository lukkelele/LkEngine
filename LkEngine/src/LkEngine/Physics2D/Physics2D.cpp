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

                //glm::vec2 camPos = cam.GetPos();
                //glm::vec2 camPos = cam.GetPosWithOffset();
                glm::vec2 camPos = cam.GetPos();
                glm::vec2 camOffset = cam.GetOffset();
                camPos.x += camOffset.x;
                camPos.y += camOffset.y;

                float quadWidth = tc.Scale.x * sc.Size.x;
                float quadHeight = tc.Scale.y * sc.Size.y;
                glm::vec2 quadPos = { tc.Translation.x, tc.Translation.y };

                // The position is placed in between the upper two points.

                // Place the origin in the middle of the screen
                // This is done by adding half of the window width and height

                // If the editor layer is enabled, adjust the quad pos by taking the editor windows into consideration
                if (editor && editor->IsEnabled())
                {
                    auto editorWindowSize = editor->GetEditorWindowSize();
                    quadPos.x += editorWindowSize.x * 0.50f + editor->GetLeftSidebarSize().x;
                    quadPos.y += editorWindowSize.y * 0.50f + editor->GetBottomBarSize().y;
                }
                else
                {
                    quadPos.x += windowWidth * 0.50f;
                    quadPos.y += windowHeight * 0.50f;
                }


                float angleRad = glm::radians(tc.GetRotation2D());
                glm::mat2 rotMat = {
                    glm::cos(angleRad), -glm::sin(angleRad),
                    glm::sin(angleRad), glm::cos(angleRad)
                };

                // >> Sprite Points
                // Bottom left: Move half size to left, down entire sprite height
                // Bottom left: Move half size to right, down entire sprite height
                // Top left: Move half size to left, stay at y
                // Top right: Move half size to right, stay at y
                glm::vec2 spritePoint_BottomLeft = { quadPos.x - quadWidth * 0.50f, quadPos.y - quadHeight };
                glm::vec2 spritePoint_BottomRight = { quadPos.x + quadWidth * 0.50f, quadPos.y - quadHeight };
                glm::vec2 spritePoint_TopLeft = { quadPos.x - quadWidth * 0.50f, quadPos.y };
                glm::vec2 spritePoint_TopRight = { quadPos.x + quadWidth * 0.50f, quadPos.y };

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    // Add camera position to adjust for camera placement in the world
                    bool within_x_boundaries = (mousePos.x + camPos.x >= spritePoint_BottomLeft.x) && (mousePos.x + camPos.x <= spritePoint_TopRight.x);
                    bool within_y_boundaries = (mousePos.y + camPos.y <= spritePoint_TopLeft.y) && (mousePos.y + camPos.y >= spritePoint_BottomRight.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        float centerX = tc.Translation.x + quadWidth * 0.50f;
                        float centerY = tc.Translation.y + quadHeight * 0.50f;
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