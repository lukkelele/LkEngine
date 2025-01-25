#include "LKpch.h"
#include "Physics2D.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Editor/EditorTabManager.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(LScene& SpriteComponentene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Raycast2DResult> results = {};
        return results;
    }

    std::vector<Raycast2DResult> Physics2D::RaycastFromScreen(LScene& SpriteComponentene)
    {
        std::vector<Raycast2DResult> Results = {};
        glm::vec2 MousePos = LMouse::GetScaledPos();

        std::vector<LEntity> SceneEntities = SpriteComponentene.GetEntities();
        for (LEntity& Entity : SceneEntities)
        {
            if (Entity.HasComponent<LTransformComponent>() && Entity.HasComponent<LSpriteComponent>())
            {
                LTransformComponent& TransformComponent = Entity.Transform();
                LSpriteComponent& SpriteComponent = Entity.GetComponent<LSpriteComponent>();
                if (TransformComponent.IsStatic() || SpriteComponent.IsPassthrough())
                {
                    continue;
                }

            #if 0
                TObjectPtr<LEditorCamera> Camera = Editor->GetEditorCamera();
                glm::vec2 CameraPos = Camera->GetPosition();

                float QuadWidth = TransformComponent.Scale.x * SpriteComponent.Size.x;
                float QuadHeight = TransformComponent.Scale.y * SpriteComponent.Size.y;
                glm::vec2 QuadPos = { TransformComponent.Translation.x, TransformComponent.Translation.y };

                /* FIXME: Rewrite all of this awful code. */

                // The position is placed in between the upper two points.

                // Place the origin in the middle of the SpriteComponentreen.
                // This is done by adding half of the window width and height.
                if (Editor && Editor->IsEnabled())
                {
                    // Center the Quad
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


                const float AngleRad = glm::radians(TransformComponent.GetRotation2D());
                glm::mat2 RotationMatrix = {
                    glm::cos(AngleRad), -glm::sin(AngleRad),
                    glm::sin(AngleRad), glm::cos(AngleRad)
                };

                // >> Sprite Points
                // Bottom left: Move half size to left, down entire sprite height
                // Bottom left: Move half size to right, down entire sprite height
                // Top left: Move half size to left, stay at y
                // Top right: Move half size to right, stay at y
                glm::vec2 SpritePoint_BottomLeft = { QuadPos.x - QuadWidth * 0.50f, QuadPos.y - QuadHeight };
                glm::vec2 SpritePoint_BottomRight = { QuadPos.x + QuadWidth * 0.50f, QuadPos.y - QuadHeight };
                glm::vec2 SpritePoint_TopLeft = { QuadPos.x - QuadWidth * 0.50f, QuadPos.y };
                glm::vec2 SpritePoint_TopRight = { QuadPos.x + QuadWidth * 0.50f, QuadPos.y };
            #if 0
                glm::vec2 SpritePoint_BottomLeft = { QuadPos.x - QuadWidth * 0.50f, QuadPos.y - QuadHeight * 0.50f };
                glm::vec2 SpritePoint_BottomRight = { QuadPos.x + QuadWidth * 0.50f, QuadPos.y - QuadHeight * 0.50f };
                glm::vec2 SpritePoint_TopLeft = { QuadPos.x - QuadWidth * 0.50f, QuadPos.y + QuadHeight * 0.50f };
                glm::vec2 SpritePoint_TopRight = { QuadPos.x + QuadWidth * 0.50f, QuadPos.y + QuadHeight * 0.50f };
            #endif

                if (LMouse::IsButtonPressed(EMouseButton::Button0))
                {
                    // Add Cameraera position to adjust for Cameraera placement in the world
                    const bool bInBoundsX = (MousePos.x + CameraPos.x >= SpritePoint_BottomLeft.x) 
                        && (MousePos.x + CameraPos.x <= SpritePoint_TopRight.x);

                    const bool bInBoundsY = (MousePos.y + CameraPos.y <= SpritePoint_TopLeft.y) 
                        && (MousePos.y + CameraPos.y >= SpritePoint_BottomRight.y);

                    if (bInBoundsX && bInBoundsY)
                    {
                        float centerX = TransformComponent.Translation.x + QuadWidth * 0.50f;
                        float centerY = TransformComponent.Translation.y + QuadHeight * 0.50f;
                        LK_CORE_WARN("Hit: {} -> ({}, {})", Entity.Name().c_str(), MousePos.x, MousePos.y);

                        float x = centerX - MousePos.x;
                        float y = centerY - MousePos.y;
                        glm::vec2 Intersection = { x, y };
                        float Distance = sqrt(pow(x, 2) + pow(y, 2));
                        Results.push_back(Raycast2DResult(Entity, Intersection, { 0, 0 }, Distance));
                    }
                }
            #endif
            }
        }

        return Results;
    }

}
