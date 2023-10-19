#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>
#include "LkEngine/UI/Property.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/UI/DockSpace.h"


namespace LkEngine {

    class Entity;

    class UILayer : public Layer
    {
    public:
        UILayer() = default;
        ~UILayer() = default;

        void OnAttach();
        void OnDetach();
        void OnImGuiRender();

        static void Init();
        static void Begin();
        static void End();
        static void BeginMainRenderWindow();
        static void EndMainRenderWindow();
        static void TopBar();
        static void BottomBar();
        static void LeftSidebar();
        static void RightSidebar();
        static void AppInfo();
        static void DockWindowsMenu();

        static void SceneMenu();
        static void CreateEntityMenu();
        static void SceneEntities();
        static void SelectedEntityMenu();

        static void CameraControls();

    public:
        static float PositionStepSize;
        static ImGuiWindowClass* UILayerWindowClass;        
        static ImGuiWindowClass* RendererWindowClass;   
        static ImGuiWindowClass* ExternalWindowClass;   
        static std::string SelectedEntityLabel;
        static bool ShowImGuiDemoWindow;
    };

}