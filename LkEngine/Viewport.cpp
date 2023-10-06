#include "LkEngine/UI/Viewport.h"


namespace LkEngine::UI {

    unsigned int Viewport::StyleStackCount = 0;

    void Viewport::BeginViewportBase()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking
    	    | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

        static bool FileMenuItemSelected = false;
        static bool EditMenuItemSelected = false;

        // auto viewport = ImGui::GetMainViewport();
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        StyleStackCount++;
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::Begin("##main-viewport", (bool*)true, flags);

        ImGui::BeginMenuBar();

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

        ImGui::EndMenuBar();
        DrawLeftSidebar();
        ImGui::PopStyleVar(StyleStackCount);
    }

    void Viewport::EndViewportBase()
    {
        ImGui::End();
        StyleStackCount = 0;
    }

    void Viewport::DrawLeftSidebar()
    {
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float LeftSidebar_MinimumWidth = 160.0f;
        static float LeftSidebar_Width = 180.0f;
        float LeftSidebar_Height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(LeftSidebar_Width, LeftSidebar_Height);
        static ImGuiWindowFlags LeftSidebar_Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking 
            | ImGuiWindowFlags_NavFlattened | ImGuiWindowFlags_NoBringToFrontOnFocus 
            | ImGuiWindowFlags_NoTitleBar;

		auto& colors = ImGui::GetStyle().Colors;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, colors[ImGuiCol_MenuBarBg]);
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::BeginChild("##LeftSideBar", LeftSidebar_Size, true, LeftSidebar_Flags);
        ImGui::Text("Left Sidebar");

        ImGui::BeginGroup();
        {
            if (ImGui::Button("Button 1")) {}
            if (ImGui::Button("Button 2")) {}
            if (ImGui::Button("Button 3")) {}
        }
        ImGui::EndGroup();

        ImGui::BeginGroup();
        {
            ImGui::Text("Width");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("##leftsidebar-width", &LeftSidebar_Width, LeftSidebar_MinimumWidth, 380.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndGroup();
        ImGui::EndChild();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(2);
    }

    void Viewport::DrawRightSidebar()
    {
    }

}