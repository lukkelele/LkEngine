#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <memory>
#include <string>


namespace LkEngine {

    // Forward Declaration
    class Window;
    
    class GraphicsContext
    {
    public:
        GraphicsContext(void* _windowHandle);
        virtual ~GraphicsContext() = default;
        virtual void Init();
        virtual void Destroy();
        virtual void SwapBuffers();
        virtual void InitImGui(const std::string& glslVersion);
        void SetDarkTheme();
    
    protected:
        std::shared_ptr<Window> m_Window = nullptr;
    };

}
