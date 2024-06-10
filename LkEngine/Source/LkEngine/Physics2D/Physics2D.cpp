#include "LKpch.h"
#include "Physics2D.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    std::vector<Raycast2DResult> Physics2D::Raycast(Scene& scene, const glm::vec2& point0, const glm::vec2& point1)
    {
        std::vector<Raycast2DResult> results = {};
        return results;
    }

    std::vector<Raycast2DResult> Physics2D::RaycastFromScreen(Scene& scene)
    {
        std::vector<Raycast2DResult> results = {};
        glm::vec2 mousePos = Mouse::GetScaledPos();

        // Exit early if no camera is attached to the scene
        auto* editor = EditorLayer::Get();
        if (editor != nullptr && editor->IsEnabled())
        {
            //auto editorCamera = editor->GetEditorCamera();
        }
        else
        {
            if (scene.GetMainCamera() == nullptr)
            {
                LK_CORE_WARN_TAG("Raycast", "Scene {} has no camera attached to it!", scene.GetName());
                return results;
            }
        }

        std::vector<Entity> sceneEntities = scene.GetEntities();
        for (auto& entity : sceneEntities)
        {
            if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteComponent>())
            {
                auto& tc = entity.Transform();
                auto& sc = entity.Sprite();
                if (tc.IsStatic() || sc.IsPassthrough())
                    continue;

               // auto cam = scene.GetMainCamera();
                auto cam= editor->GetEditorCamera();
                glm::vec2 camPos = cam->GetPosition();

                //glm::vec2 camOffset = cam.GetOffset();
                //camPos.x += camOffset.x;
                //camPos.y += camOffset.y; // - or + ?

                float quadWidth = tc.Scale.x * sc.Size.x;
                float quadHeight = tc.Scale.y * sc.Size.y;
                glm::vec2 quadPos = { tc.Translation.x, tc.Translation.y };

                // The position is placed in between the upper two points.

                // Place the origin in the middle of the screen
                // This is done by adding half of the window width and height

                auto* editor = EditorLayer::Get();
                if (editor && editor->IsEnabled())
                {
                    // Center the quad
                    quadPos.x += editor->EditorWindowSize.x * 0.50f + editor->GetLeftSidebarSize().x;
                    // Only add the tabbar size if there are any tabs 
                    if (editor->GetTabCount() == 0)
                        quadPos.y += (editor->EditorWindowSize.y * 0.50f) + (editor->GetBottomBarSize().y - editor->GetMenuBarSize().y) * Window::Get().GetScalerY();
                    else
                        quadPos.y += (editor->EditorWindowSize.y * 0.50f) + (editor->GetBottomBarSize().y - editor->GetMenuBarSize().y - editor->GetTabBarSize().y) * Window::Get().GetScalerY();
                }
                else
                {
                    quadPos.x += Window::Get().GetWidth() * 0.50f;
                    quadPos.y += Window::Get().GetHeight() * 0.50f;
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
            #if 0
                glm::vec2 spritePoint_BottomLeft = { quadPos.x - quadWidth * 0.50f, quadPos.y - quadHeight * 0.50f };
                glm::vec2 spritePoint_BottomRight = { quadPos.x + quadWidth * 0.50f, quadPos.y - quadHeight * 0.50f };
                glm::vec2 spritePoint_TopLeft = { quadPos.x - quadWidth * 0.50f, quadPos.y + quadHeight * 0.50f };
                glm::vec2 spritePoint_TopRight = { quadPos.x + quadWidth * 0.50f, quadPos.y + quadHeight * 0.50f };
            #endif

                if (Mouse::IsButtonPressed(MouseButton::Button0))
                {
                    // Add camera position to adjust for camera placement in the world
                    bool within_x_boundaries = (mousePos.x + camPos.x >= spritePoint_BottomLeft.x) && (mousePos.x + camPos.x <= spritePoint_TopRight.x);
                    bool within_y_boundaries = (mousePos.y + camPos.y <= spritePoint_TopLeft.y) && (mousePos.y + camPos.y >= spritePoint_BottomRight.y);
                    if (within_x_boundaries && within_y_boundaries)
                    {
                        float centerX = tc.Translation.x + quadWidth * 0.50f;
                        float centerY = tc.Translation.y + quadHeight * 0.50f;
                        LK_CORE_WARN("Hit: {} -> ({}, {})", entity.Name().c_str(), mousePos.x, mousePos.y);

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