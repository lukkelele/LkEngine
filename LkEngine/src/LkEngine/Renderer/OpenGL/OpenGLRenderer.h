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

        void Draw(VertexBuffer& vb, const Shader& shader);
        void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader);
		void SubmitIndexed(unsigned int count);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0);
		void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);
    };

}