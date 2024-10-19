#pragma once

#include "Framebuffer.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "SwapChain.h"
#include "RenderCommandQueue.h"
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Image.h"
#include "Material.h"
#include "Color.h"

#include "RendererSpecification.h"


namespace LkEngine {

    class LSceneCamera;

    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 Texcoord;
        float TexIndex;
        float TexArray;
        float TilingFactor;
    };

    struct LineVertex
    {
        glm::vec3 Position{};
        glm::vec4 Color{};
    };

	class IRenderer2DAPI
	{
	public:
        virtual ~IRenderer2DAPI() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void BeginScene(const LSceneCamera& camera) = 0;
        virtual void BeginScene(const LSceneCamera& camera, const glm::mat4& transform) = 0;
        virtual void BeginScene(const glm::mat4& transform) = 0;
        virtual void Flush() = 0;
        virtual void EndScene() = 0;

        virtual void DrawImage(const TObjectPtr<LImage>& Image) = 0;

        virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;
        virtual void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation, uint64_t entityID = 0) = 0;

        virtual void DrawQuad(const glm::vec2& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture2D, 
                              const float rotation, 
                              uint64_t entityID = 0) = 0;

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture, 
                              const float rotation, 
                              uint64_t entityID = 0) = 0;

        virtual void DrawQuad(const glm::vec3& pos, 
                              const glm::vec2& size, 
                              TObjectPtr<LTexture2D> texture, 
                              const glm::vec4& tintColor, 
                              const float rotation, 
                              uint64_t entityID = 0) = 0;

        virtual void DrawRotatedQuad(const glm::vec2& pos, 
                                     const glm::vec2& size, 
                                     const glm::vec4& color, 
                                     const float rotation, 
                                     uint64_t entityID = 0) = 0;

        virtual void DrawRotatedQuad(const glm::vec3& pos, 
                                     const glm::vec2& size, 
                                     const glm::vec4& color, 
                                     const float rotation, 
                                     uint64_t entityID = 0) = 0;

        
        // DrawLine
        virtual void DrawLine(const glm::vec2& p0, 
                              const glm::vec2& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) = 0;

        virtual void DrawLine(const glm::vec3& p0, 
                              const glm::vec3& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) = 0;
        // ~DrawLine

        virtual float GetLineWidth() = 0;
        virtual void SetLineWidth(float width) = 0;

        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture) = 0;
        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture, int slot) = 0;

        virtual const Renderer2DSpecification& GetSpecification() const = 0;

        //FORCEINLINE static IRenderer2DAPI* Get() { return Renderer2DAPI; }
        FORCEINLINE static IRenderer2DAPI& Get() { return *Renderer2DAPI; }

        virtual TObjectPtr<LShader> GetQuadShader() = 0;
        virtual TObjectPtr<LShader> GetLineShader() = 0;

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

        inline static constexpr glm::vec2 TextureCoords[] = { 
            { 0.0f, 0.0f }, // Bottom Left 
            { 0.0f, 1.0f }, // Top Left
            { 1.0f, 1.0f }, // Top Right
            { 1.0f, 0.0f }  // Bottom Right
        };
        
    protected:
        virtual void StartBatch() = 0;
        virtual void NextBatch() = 0;

        virtual void AddQuadBuffer() = 0;
        virtual void AddLineBuffer() = 0;
        virtual QuadVertex*& GetWriteableQuadBuffer() = 0;
        virtual LineVertex*& GetWriteableLineBuffer() = 0;

    protected:
        //inline static Ref<IRenderer2DAPI> Renderer2DAPI = nullptr;
        inline static IRenderer2DAPI* Renderer2DAPI = nullptr;

    private:
        friend class Renderer2D; /// REMOVE
        friend class LRenderer2D;
        friend class LEditorLayer;
	};

}