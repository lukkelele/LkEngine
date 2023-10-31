#pragma once

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

    class OpenGLRenderer : public RendererAPI
    {
    public:
        void Init();
        void Shutdown();

        void Clear();
        void BeginFrame();
        void EndFrame();
        void Draw(VertexArray& va, const Shader& shader);
        void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		void DrawIndexed(VertexArray& va, unsigned int count);
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
		void DrawSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);

    };

}