#pragma once
#include "LkEngine/Core/Window.h"


namespace LkEngine {

    constexpr int OpenGL_Major_Version = 3;
    constexpr int OpenGL_Minor_Version = 3;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";
    constexpr const char* OpenGL_GLSL = OpenGL_GLSL_45;

    // Forward Declaration
    class Renderer;
    
    class Windows_Window : public Window
    {
    public:
        Windows_Window(const char* title, uint32_t width, uint32_t height);
        virtual ~Windows_Window();
    
        void Init(const std::string& glslVersion = OpenGL_GLSL);
        void OnUpdate();
        void Exit();
        void Clear();
        void BeginFrame();
        void EndFrame();
        bool IsVSync() const;
        void SetVSync(bool enabled);
    };

}