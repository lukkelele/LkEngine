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

    void Debugger::AttachDebugDrawer2D(void* world2D)
    {
    //#ifdef LK_PHYSICS_API_BOX2D
        if (Debugger2D::GetCurrentAPI() == Physics2D::API::Box2D)
        // Box2D implementation
		//b2Draw* drawDebugger = dynamic_cast<b2Draw*>(Debugger2D::Get());
        LK_ASSERT(m_Instance != nullptr, "Debugger2D needs to be initialized before debug drawer can be setup");

		b2Draw* drawDebugger = dynamic_cast<b2Draw*>(m_Debugger2D.get());
        auto* worldComponent = static_cast<Box2DWorldComponent*>(world2D);
		worldComponent->World->SetDebugDraw(drawDebugger);
		drawDebugger->SetFlags(b2Draw::e_shapeBit);
		LOG_DEBUG("Added debug drawer for Box2D");

    //#else
        // Not implemented

    //#endif
    }



}