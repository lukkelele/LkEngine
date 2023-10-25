#pragma once

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Editor/EditorCamera.h"


namespace LkEngine {

    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();
        static void Flush();

        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
        static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawSprite(const glm::mat4& transform, SpriteComponent& src, int entityID);

        static float GetLineWidth();
        static void SetLineWidth(float width);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };
        static void ResetStats();
        static Statistics GetStats();

    private:
        static void StartBatch();
        static void NextBatch();
    };

}