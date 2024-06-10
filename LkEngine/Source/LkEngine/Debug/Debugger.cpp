#include "LKpch.h"
#include "Debugger.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"


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
        LK_CORE_DEBUG_TAG("Debugger", "Initializing...");
        m_Debugger2D = Debugger2D::Create();
    }

    void Debugger::DrawCursor(float x, float y)
    {
        LWindow& WindowRef = LWindow::Get();
        Renderer::Submit([&]()
        {
        });
    }

    void Debugger::DrawCursor(const ImVec2& cursorPos)
    {
        DrawCursor(cursorPos.x, cursorPos.y);
    }

    void Debugger::AttachDebugDrawer2D(void* world2D, int drawFlags)
    {
        if (Debugger2D::GetCurrentAPI() == Physics2D::API::Box2D)
        {
        }
        LK_ASSERT(m_Instance != nullptr, "Debugger2D needs to be initialized before debug drawer can be setup");

        b2Draw* drawDebugger = dynamic_cast<b2Draw*>(m_Debugger2D.get());
        auto* worldComponent = static_cast<Box2DWorldComponent*>(world2D);
        worldComponent->World->SetDebugDraw(drawDebugger);
        worldComponent->DebugDrawerAttached = true;

        int b2DrawFlags = 0;
        if (drawFlags == Debugger2D::DrawMode2D::Shape)
            drawFlags = b2Draw::e_shapeBit;
        else if (drawFlags == (Debugger2D::DrawMode2D::Shape | Debugger2D::DrawMode2D::Joints))
            drawFlags = b2Draw::e_shapeBit | b2Draw::e_jointBit;
        else
            drawFlags = 0;

		drawDebugger->SetFlags(b2Draw::e_shapeBit);

		LK_CORE_DEBUG("Added debug drawer for Box2D");
    }



}