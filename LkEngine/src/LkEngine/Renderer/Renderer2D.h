#pragma once

#include "Renderer2DAPI.h"


namespace LkEngine {

    class Renderer2D : public RefCounted
    {
    public:
        static void Init();
        static void Shutdown();
        static void BeginScene(const Camera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
        static void Flush();

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0);

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture2D, float rotation, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID = 0);

        static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);

        static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0);

        static float GetLineWidth();
        static void SetLineWidth(float width);

        static Ref<Renderer2DAPI> Create(const Renderer2DSpecification& specification);
        void SetRenderer(const Ref<RendererAPI>& renderer) { m_Renderer2D = renderer; }

    protected:
        inline static Ref<Renderer2DAPI> m_Renderer2D = nullptr;

        friend class RendererAPI;
    };

}

