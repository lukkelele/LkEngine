#pragma once
#include <string>
#include <memory>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine {

    class Renderer
    {
    public:    
        Renderer(const std::string& glslVersion);
        virtual ~Renderer() = default;
    
        static Renderer* Instance() { return m_Instance; }
        
        void Init();
        void Clear();

        std::string GetGLSLVersion() { return m_GlslVersion; }
    
    private:
        static Renderer* m_Instance;
        std::string m_GlslVersion = "";
    };

}
