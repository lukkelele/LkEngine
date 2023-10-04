#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#define UI_SIDEBAR_LEFT  0
#define UI_SIDEBAR_RIGHT 1


namespace LkEngine::UI {



    class Viewport
    {
    public:
        Viewport() = default;
        ~Viewport() = default;

        static void BeginViewportBase()
        {
		    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking
			    | ImGuiWindowFlags_NoDecoration;

            static bool FileMenuItemSelected = false;
            static bool EditMenuItemSelected = false;

            // auto viewport = ImGui::GetMainViewport();
            ImGuiContext& g = *GImGui;
            ImGuiViewportP* viewport = g.Viewports[0];
            ImGuiWindow* menu_bar_window = ImGui::FindWindowByName("##MainStatusBar");

            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::Begin("Viewport", (bool*)true, flags);

            static float mainMenuHeight = 74.0f;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
            ImGui::SetNextWindowPos(ImVec2(0, viewport->Pos.y / 2));
            ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, mainMenuHeight));
            ImGui::Begin("##MainStatusBar", NULL, window_flags);
            ImGui::SetNextWindowSize(ImVec2(400, 600));
            ImGui::BeginMenuBar();

            // Report size into work area (for next frame) using actual window size
            menu_bar_window = ImGui::GetCurrentWindow();
            if (menu_bar_window->BeginCount == 1)
                viewport->WorkOffsetMin.y += menu_bar_window->Size.y; 
            
            g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);

            if (ImGui::BeginMenu("File")) 
            {
                ImGui::MenuItem("New");
                FileMenuItemSelected = true;
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Edit", NULL, &EditMenuItemSelected))
            {
                EditMenuItemSelected = true;
            }

            int left_SidebarWidth = 120;
            char sidebarID[50];
            // ImGuiWindowFlags left_SidebarFlags = ImGuiWindowFlags_NoMove;
            ImGuiWindowFlags left_SidebarFlags = ImGuiWindowFlags_None;
            sprintf(sidebarID, "sidebar%d", UI_SIDEBAR_LEFT);
            ImGui::BeginViewportSideBar(sidebarID, viewport, ImGuiDir_Left, left_SidebarWidth, left_SidebarFlags);

            ImGui::End();


            ImGui::EndMenuBar();

            ImGui::End();
        }

        static void EndViewportBase()
        {
            ImGui::End();
        }


    };

}