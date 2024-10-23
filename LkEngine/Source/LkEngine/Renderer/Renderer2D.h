#pragma once

#include "Renderer2DAPI.h"


namespace LkEngine {

    class LSceneCamera;

    /// @FIXME:
    /// Rewrite this
    /// Too many virtual nested calls here.
    /// Performance SUCKS

    class LRenderer2D
    {
    public:
        static void Init();
        static void Shutdown();
        static void BeginScene(const LSceneCamera& camera);
        static void BeginScene(const LSceneCamera& camera, const glm::mat4& transform);
        static void EndScene();
        static void Flush();

        static void DrawImage(const TObjectPtr<LImage> image);

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0);

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);

        static void DrawQuad(const glm::vec2& pos, 
                             const glm::vec2& size, 
                             TObjectPtr<LTexture> texture2D, 
                             float rotation, 
                             uint64_t entityID = 0);

        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, TObjectPtr<LTexture> texture, float rotation, uint64_t entityID = 0);
        static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, TObjectPtr<LTexture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID = 0);

        static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);

        static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0);
        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0);

        static float GetLineWidth();
        static void SetLineWidth(float width);

        /// FIXME: REMOVE
        FORCEINLINE void SetRenderer(IRenderer2DAPI* InRenderer) 
        { 
            LK_CORE_ASSERT(false);
            LK_UNUSED(InRenderer);
        }

    protected:
        inline static TUniquePtr<IRenderer2DAPI> Renderer2D = nullptr;

        friend class RendererAPI;
    };

}

