#include "LKpch.h"
#include "LkEngine/UI/UI.h"


namespace LkEngine::UI {

    ImGuiWindowClass* UIWindowClass = nullptr;
    ImGuiWindowClass* RendererWindowClass = nullptr;
    
    void BeginMainRenderWindow()
    {
        //ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        ImGui::SetNextWindowClass(RendererWindowClass);
        ImGui::SetNextWindowBgAlpha(0);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::Begin(MainRenderWindow_Label, NULL, flags);
        ImGui::PopStyleColor();
    }

    void EndMainRenderWindow()
    {
        ImGui::End();
    }

    void TopBar()
    {
        static bool top_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        static int val = 0;
        ImGui::Begin(TopBar_Label, &top_bar_open, flags);
        ImGui::Text("Top Bar Content");
        ImGui::End();
    }

    void BottomBar()
    {
        static bool bottom_bar_open = true;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::Begin(Bottom_Bar_Label, &bottom_bar_open, flags);
        ImGui::Text("Bottom Bar");
        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void LeftSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];
        float sidebar_left_height = viewport->WorkSize.y;
        float sidebar_left_width = viewport->WorkSize.x;
        static float sidebar_left_minwidth = 140.0f;
        ImVec2 Sidebar_Left_Size = ImVec2(sidebar_left_width, sidebar_left_height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGui::SetNextWindowClass(UIWindowClass);
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoNav;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::SetNextWindowSize(ImVec2(sidebar_left_width, sidebar_left_height));
        ImGui::Begin(Sidebar_Left_Label, &sidebar_open, flags);
        ImGui::PopStyleVar(1);
        ImGui::Text("Left Sidebar");

        ImGui::BeginGroup();
        if (ImGui::Button("Button 1")) {}
        if (ImGui::Button("Button 2")) {}
        if (ImGui::Button("Button 3")) {}
        ImGui::EndGroup();

        ImGui::End();
    }

    void RightSidebar()
    {
        static bool sidebar_open = true;
        ImGuiContext& g = *GImGui;
        ImGuiViewportP* viewport = g.Viewports[0];

        static float min_width = 160.0f;
        static float width = 180.0f;
        static float height = viewport->WorkSize.y;
        ImVec2 LeftSidebar_Size = ImVec2(width, height);
		auto& colors = ImGui::GetStyle().Colors;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowClass(UIWindowClass);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin(Sidebar_Right_Label, &sidebar_open, flags);
        ImGui::Text("Right Sidebar");

        ImGui::BeginGroup();
        {
            if (ImGui::Button("Button 1")) {}
            if (ImGui::Button("Button 2")) {}
            if (ImGui::Button("Button 3")) {}
        }
        ImGui::EndGroup();

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

}