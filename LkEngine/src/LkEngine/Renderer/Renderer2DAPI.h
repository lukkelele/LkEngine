#pragma once

#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Framebuffer.h"
#include "SwapChain.h"
#include "RenderCommandQueue.h"
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"
#include "Image.h"
#include "Material.h"
#include "Camera.h"
#include "Color.h"


namespace LkEngine {

    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;
        int EntityID; 
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        int EntityID; 
    };

    struct Renderer2DSpecification
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxLines = 1000;
        bool SwapChainTarget = false;
    };

	class Renderer2DAPI : public RefCounted
	{
	public:
        virtual ~Renderer2DAPI() = default;

        static Ref<Renderer2DAPI> Create(const Renderer2DSpecification& specification);
        virtual Ref<Renderer2DAPI> GetRenderer2D() = 0;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        virtual void BeginScene(const Camera& camera) = 0;
        virtual void BeginScene(const Camera& camera, const glm::mat4& transform) = 0;
        virtual void EndScene() = 0;
        virtual void Flush() = 0;

        virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0) = 0;

        virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;

        virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture> texture2D, float rotation, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, float rotation, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture> texture, const glm::vec4& tintColor, float rotation, uint64_t entityID = 0) = 0;

        virtual void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
        virtual void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;

        virtual void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, uint64_t entityID = 0) = 0;

        virtual float GetLineWidth() = 0;
        virtual void SetLineWidth(float width) = 0;

        virtual void AddTextureToSlot(Ref<Texture> texture) = 0;
        virtual void AddTextureToSlot(Ref<Texture> texture, int slot) = 0;

        virtual Ref<Shader> GetQuadShader() = 0;
        virtual Ref<Shader> GetLineShader() = 0;

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t QuadVertexArray_RendererID = 0;
            uint32_t QuadVertexBuffer_RendererID = 0;
            uint32_t LineCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };

        virtual Statistics GetStats() = 0;
        virtual void ResetStats() = 0;

#if 0
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float TilingFactor;
            int EntityID; 
        };

        struct LineVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            int EntityID; 
        };
#endif
        
    protected:
        virtual void StartBatch() = 0;
        virtual void NextBatch() = 0;

        virtual void AddQuadBuffer() = 0;
        virtual void AddLineBuffer() = 0;
        virtual QuadVertex*& GetWriteableQuadBuffer() = 0;
        virtual LineVertex*& GetWriteableLineBuffer() = 0;

    protected:
        inline static Ref<Renderer2DAPI> m_Renderer2D = nullptr;

    private:
        friend class Renderer2D;
	};

}