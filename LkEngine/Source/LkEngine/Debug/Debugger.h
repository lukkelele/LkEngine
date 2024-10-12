#pragma once

#include <LkEngine/Debug/Debugger2D.h>

#include <imgui/imgui.h>


/// REMOVE ME
namespace LkEngine {

    class Debugger
    {
    public:
        Debugger();
        ~Debugger();

        static Debugger* Get() { return m_Instance; }
        static TSharedPtr<Debugger2D> GetDebugger2D() { return m_Debugger2D; }

        void Init();

        static void DrawCursor(float x, float y);
        static void DrawCursor(const ImVec2& cursorPos);

        static void AttachDebugDrawer2D(void* world2D, int drawFlags = Debugger2D::DrawMode2D::Shape);

    private:
        inline static Debugger* m_Instance = nullptr;
        inline static TSharedPtr<Debugger2D> m_Debugger2D = nullptr;
    };

}