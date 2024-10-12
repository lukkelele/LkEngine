#include "LKpch.h"
#include "Physics2D.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(LScene& scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Raycast2DResult> results = {};
        return results;
    }

    std::vector<Raycast2DResult> Physics2D::RaycastFromScreen(LScene& scene)
    {
        std::vector<Raycast2DResult> results = {};
        glm::vec2 MousePos = Mouse::GetScaledPos();

        // Exit early if no camera is attached to the scene
        auto* Editor = LEditorLayer::Get();
        if (Editor && Editor->IsEnabled())
        {
            //auto EditorCamera = Editor->GetEditorCamera();
        }
        else
        {
            if (scene.GetMainCamera() == nullptr)
            {
                LK_CORE_WARN_TAG("Raycast", "Scene {} has no camera attached to it!", scene.GetName());
                return results;
            }
        }

        std::vector<LEntity> SceneEntities = scene.GetEntities();
        for (LEntity& Entity : SceneEntities)
        {
            if (Entity.HasComponent<LTransformComponent>() && Entity.HasComponent<LSpriteComponent>())
            {
                LTransformComponent& tc = Entity.Transform();
                LSpriteComponent& sc = Entity.GetComponent<LSpriteComponent>();
                if (tc.IsStatic() || sc.IsPassthrough())
                {
                    continue;
                }

                auto cam = Editor->GetEditorCamera();
                glm::vec2 camPos = cam->GetPosition();

                float quadWidth = tc.Scale.x * sc.Size.x;
                float quadHeight = tc.Scale.y * sc.Size.y;
                glm::vec2 QuadPos = { tc.Translation.x, tc.Translation.y };

                // The position is placed in between the upper two points.

                // Place the origin in the middle of the screen.
                // This is done by adding half of the window width and height.

                LEditorLayer* Editor = LEditorLayer::Get();
                if (Editor && Editor->IsEnabled())
                {
                    // Center the quad
                    QuadPos.x += Editor->EditorWindowSize.x * 0.50f + Editor->GetLeftSidebarSize().x;
                    // Only add the tabbar size if there are any tabs 
                    if (LEditorTabManager::GetTabCount() == 0)
                    {
                        QuadPos.y += (Editor->EditorWindowSize.y * 0.50f) 
                                  + (Editor->GetBottomBarSize().y - Editor->GetMenuBarSize().y) 
                                  * LWindow::Get().GetScalerY();
                    }
                    else
                    {
                        QuadPos.y += (Editor->EditorWindowSize.y * 0.50f) 
                                  + (Editor->GetBottomBarSize().y - Editor->GetMenuBarSize().y - Editor->GetTabBarSize().y) 
                                  * LWindow::Get().GetScalerY();
                    }
                }
                else
                {
                    QuadPos.x += LWindow::Get().GetWidth() * 0.50f;
                    QuadPos.y += LWindow::Get().GetHeight() * 0.50f;
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
                glm::vec2 SpritePoint_BottomLeft = { QuadPos.x - quadWidth * 0.50f, QuadPos.y - quadHeight };
                glm::vec2 SpritePoint_BottomRight = { QuadPos.x + quadWidth * 0.50f, QuadPos.y - quadHeight };
                glm::vec2 SpritePoint_TopLeft = { QuadPos.x - quadWidth * 0.50f, QuadPos.y };
                glm::vec2 SpritePoint_TopRight = { QuadPos.x + quadWidth * 0.50f, QuadPos.y };
            #if 0
                glm::vec2 SpritePoint_BottomLeft = { QuadPos.x - quadWidth * 0.50f, QuadPos.y - quadHeight * 0.50f };
                glm::vec2 SpritePoint_BottomRight = { QuadPos.x + quadWidth * 0.50f, QuadPos.y - quadHeight * 0.50f };
                glm::vec2 SpritePoint_TopLeft = { QuadPos.x - quadWidth * 0.50f, QuadPos.y + quadHeight * 0.50f };
                glm::vec2 SpritePoint_TopRight = { QuadPos.x + quadWidth * 0.50f, QuadPos.y + quadHeight * 0.50f };
            #endif

                if (Mouse::IsButtonPressed(EMouseButton::Button0))
                {
                    // Add camera position to adjust for camera placement in the world
                    bool within_x_boundaries = (MousePos.x + camPos.x >= SpritePoint_BottomLeft.x) && (MousePos.x + camPos.x <= SpritePoint_TopRight.x);
                    bool within_y_boundaries = (MousePos.y + camPos.y <= SpritePoint_TopLeft.y) && (MousePos.y + camPos.y >= SpritePoint_BottomRight.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        float centerX = tc.Translation.x + quadWidth * 0.50f;
                        float centerY = tc.Translation.y + quadHeight * 0.50f;
                        LK_CORE_WARN("Hit: {} -> ({}, {})", Entity.Name().c_str(), MousePos.x, MousePos.y);

                        float x = centerX - MousePos.x;
                        float y = centerY - MousePos.y;
                        glm::vec2 Intersection = { x, y };
                        float Distance = sqrt(pow(x, 2) + pow(y, 2));
                        results.push_back(Raycast2DResult(Entity, Intersection, { 0, 0 }, Distance));
                    }
                }
            }
        }

        return results;
    }

}