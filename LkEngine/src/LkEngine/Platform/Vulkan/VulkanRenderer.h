#pragma once

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

    class VulkanRenderer : public RendererAPI
    {
    public:
        void Init();
        void Shutdown();
        void Clear();

        void BeginFrame();
        void EndFrame();

        void SetDrawMode(const RendererDrawMode& mode);
        void Draw(VertexBuffer& vb, const Shader& shader);
        void Draw(const VertexBuffer& vb, const IndexBuffer& ib, const Shader& shader);
        s_ptr<Renderer2D> GetRenderer2D() { return m_Renderer2D; }

        void SubmitLine(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, float rotation, uint64_t entityID = 0);
        void SubmitQuad(const glm::vec2& pos, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID);
        void SubmitQuad(const glm::vec3& pos, const glm::vec2& size, s_ptr<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID);
		void SubmitSprite(TransformComponent& tc, const glm::vec2& size, const glm::vec4 color, uint64_t entityID = 0);
		void SubmitIndexed(unsigned int count);

    private:
		s_ptr<Renderer2D> m_Renderer2D = nullptr;
    };

}