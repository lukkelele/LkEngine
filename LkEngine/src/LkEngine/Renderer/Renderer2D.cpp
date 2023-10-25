#include "LKpch.h"
#include "LkEngine/Renderer/Renderer2D.h"


namespace LkEngine {

    void Renderer2D::Init()
    {
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::Flush()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
    {

    }

    void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
    }

    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteComponent& src, int entityID)
    {
    }

    float Renderer2D::GetLineWidth()
    {
        return 0.0f;
    }

    void Renderer2D::SetLineWidth(float width)
    {
    }

    void Renderer2D::ResetStats()
    {
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return Statistics();
    }

    void Renderer2D::StartBatch()
    {
    }

    void Renderer2D::NextBatch()
    {
    }

}