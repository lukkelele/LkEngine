#include "LKpch.h"
#include "Debugger.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

    Debugger::Debugger()
    {
        m_Instance = this;
    }

    Debugger::~Debugger()
    {
    }

    void Debugger::Init()
    {
        m_Debugger2D = Debugger2D::Create();
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