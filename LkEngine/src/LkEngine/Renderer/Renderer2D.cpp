#include "LKpch.h"
#include "Renderer2D.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer2D.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderer2D.h"

#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine {

    void Renderer2D::Init()
    {
    }

    void Renderer2D::Shutdown() 
    {
    }

    void Renderer2D::BeginScene(const SceneCamera& camera) 
    {
        m_Renderer2D->BeginScene(camera);
    }

    void Renderer2D::BeginScene(const SceneCamera& camera, const glm::mat4& transform) 
    {
        m_Renderer2D->BeginScene(camera, transform);
    }

    void Renderer2D::EndScene() 
    {
        m_Renderer2D->EndScene();
    }

    void Renderer2D::Flush() 
    {
        m_Renderer2D->Flush();
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        m_Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture2D, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, texture2D, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, texture, rotation, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawQuad(pos, size, texture, tintColor, rotation, entityID);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawRotatedQuad(pos, size, color, rotation, entityID);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID) 
    {
        m_Renderer2D->DrawRotatedQuad(pos, size, color, rotation, entityID);
    }

    void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID) 
    {
        m_Renderer2D->DrawLine(p0, p1, color, entityID);
    }

    void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID) 
    {
        m_Renderer2D->DrawLine(p0, p1, color, entityID);
    }

    float Renderer2D::GetLineWidth() 
    {
        return 0.0f;
    }

    void Renderer2D::SetLineWidth(float width) 
    {
    }

}
