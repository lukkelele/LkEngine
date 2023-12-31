#pragma once

#include <stdint.h>

#include <imgui/imgui.h>


namespace LkEngine {

    class Debugger
    {
    public:
        Debugger();
        ~Debugger() = default;

        static void DrawCursor(float x, float y);
        static void DrawCursor(const ImVec2& cursorPos);

    private:
        inline static Debugger* m_Instance = nullptr;
    };

}