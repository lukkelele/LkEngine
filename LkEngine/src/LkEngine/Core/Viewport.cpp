#include "LKpch.h"
#include "LkEngine/Core/Viewport.h"
#ifdef PLATFORM_WINDOWS
#include "LkEngine/Platform/Windows/Windows_Window.h"
#else
// #include "LkEngine/Platform/Linux/Linux_Window.h"
#endif

namespace LkEngine {

    Viewport* Viewport::m_CurrentViewport = nullptr;

    Viewport::Viewport(Window* window)
    {
        m_CurrentViewport = this;
    #ifdef PLATFORM_WINDOWS
        m_MainWindow = std::make_shared<Windows_Window>(*(static_cast<Windows_Window*>(window)));
    #else // PLATFORM_LINUX
    #endif
    }

    Viewport::~Viewport()
    {
    }

    void Viewport::OnSizeChange()
    {
        SizeHasChanged = true;
    }

}