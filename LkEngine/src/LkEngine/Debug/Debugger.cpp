#include "LKpch.h"
#include "LkEngine/Debug/Debugger.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

    Debugger::Debugger()
    {
        m_Instance = this;
    }

    void Debugger::DrawCursor(float x, float y)
    {
        auto* window = Window::Get();
        Renderer::Submit([&]()
        {
            //RenderCommand                        
        });
    }

    void Debugger::DrawCursor(const ImVec2& cursorPos)
    {
        DrawCursor(cursorPos.x, cursorPos.y);
    }

}