#include "LKpch.h"
#include "Renderer2D.h"

#include "RendererAPI.h"
#include "LkEngine/Platform/OpenGL/OpenGLRenderer2D.h"

#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine {

    void LRenderer2D::Init()
    {
    }

    void LRenderer2D::Shutdown() 
    {
    }

    void LRenderer2D::BeginScene(const LSceneCamera& camera) 
    {
        Renderer2D->BeginScene(camera);
    }

    void LRenderer2D::BeginScene(const LSceneCamera& camera, const glm::mat4& transform) 
    {
        Renderer2D->BeginScene(camera, transform);
    }

    void LRenderer2D::EndScene() 
    {
        Renderer2D->EndScene();
    }

    void LRenderer2D::Flush() 
    {
        Renderer2D->Flush();
    }

    void LRenderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(transform, color, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, color, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, color, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID)
    {
        Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, color, rotation, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, TObjectPtr<LTexture> texture2D, float rotation, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, texture2D, rotation, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, TObjectPtr<LTexture> texture, float rotation, uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, texture, rotation, entityID);
    }

    void LRenderer2D::DrawQuad(const glm::vec3& pos, 
                               const glm::vec2& size, 
                               TObjectPtr<LTexture> texture, 
                               const glm::vec4& tintColor, 
                               float rotation, 
                               uint64_t entityID) 
    {
        Renderer2D->DrawQuad(pos, size, texture, tintColor, rotation, entityID);
    }

    void LRenderer2D::DrawRotatedQuad(const glm::vec2& pos, 
                                      const glm::vec2& size, 
                                      const glm::vec4& color, 
                                      const float rotation, 
                                      uint64_t entityID) 
    {
        Renderer2D->DrawRotatedQuad(pos, size, color, rotation, entityID);
    }

    void LRenderer2D::DrawRotatedQuad(const glm::vec3& pos, 
                                      const glm::vec2& size, 
                                      const glm::vec4& color, 
                                      const float rotation, 
                                      uint64_t entityID) 
    {
        Renderer2D->DrawRotatedQuad(pos, size, color, rotation, entityID);
    }

    void LRenderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID) 
    {
        Renderer2D->DrawLine(p0, p1, color, entityID);
    }

    void LRenderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID) 
    {
        Renderer2D->DrawLine(p0, p1, color, entityID);
    }

    float LRenderer2D::GetLineWidth() 
    {
        return 0.0f;
    }

    void LRenderer2D::SetLineWidth(float width) 
    {
    }

}
