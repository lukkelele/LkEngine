#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    Renderer* Renderer::m_Instance = nullptr;
    
    Renderer::Renderer(const std::string& glslVersion)
        : m_GlslVersion(glslVersion)
    {
        m_Instance = this;
    }
    
    void Renderer::Init()
    {
    }
    
    void Renderer::Clear()
    {
        glClearColor(0.42f, 0.20f, 0.30f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    // void Renderer::DrawImage(Image* _image)
    // {
    //     Image* image = static_cast<OpenGL_Image*>(_image);
    //     ImTextureID textureID = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(image->GetRendererID()));
    //     ImGui::Image(textureID, ImVec2(image->GetWidth(), image->GetHeight()));
    // }

}
