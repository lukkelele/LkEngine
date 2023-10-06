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

        static void BeginViewportBase();
        static void EndViewportBase();
        static void DrawLeftSidebar();
        static void DrawRightSidebar();

    private:
        static unsigned int StyleStackCount;

    };

}