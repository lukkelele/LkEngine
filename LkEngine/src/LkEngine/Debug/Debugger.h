#pragma once

#include <LkEngine/Debug/Debugger2D.h>

#include <imgui/imgui.h>


namespace LkEngine {

    class Debugger
    {
    public:
        Debugger();
        ~Debugger();

        static Debugger* Get() { return m_Instance; }
        static s_ptr<Debugger2D> GetDebugger2D() { return m_Debugger2D; }

        void Init();

        static void DrawCursor(float x, float y);
        static void DrawCursor(const ImVec2& cursorPos);

    private:
        inline static Debugger* m_Instance = nullptr;
        inline static s_ptr<Debugger2D> m_Debugger2D = nullptr;
    };

}