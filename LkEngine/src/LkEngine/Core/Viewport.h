#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Window.h"


namespace LkEngine {

    struct GlfwData
    {
        int Width;
        int Height;
        int WindowsCount;
        GlfwData() : Width(0), Height(0), WindowsCount(0)
        {}
    };

    class Viewport
    {
    public:
        Viewport(Window* window = nullptr);
        ~Viewport();

        static Viewport* GetCurrent() { return m_CurrentViewport; }

        GlfwData GetGlfwData() const { return m_GlfwData; }
        s_ptr<Window> GetMainWindow() const { return m_MainWindow; }
        bool HasSizeChanged() { return SizeHasChanged; }
        void OnSizeChange();

    private:
        GlfwData m_GlfwData;
        bool SizeHasChanged = false;
        s_ptr<Window> m_MainWindow = nullptr;
        static Viewport* m_CurrentViewport;

        friend class Window;
    };

}