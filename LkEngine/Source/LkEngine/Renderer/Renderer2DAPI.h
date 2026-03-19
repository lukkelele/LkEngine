#pragma once

#include "RendererCore.h"
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

	/**
	 * LRenderer2D
     * 
     *  Base implementation for the 2D renderer.
	 */
	class LRenderer2DAPI : public LObject
	{
	public:
        LRenderer2DAPI();
        virtual ~LRenderer2DAPI() = default;

        virtual void Initialize() = 0;
        virtual void Destroy() = 0;

        // Scene
        virtual void BeginScene(const LSceneCamera& Camera) = 0;
        virtual void BeginScene(const LSceneCamera& Camera, const glm::mat4& Transform) = 0;
        virtual void BeginScene(const glm::mat4& Transform) = 0;
        virtual void Flush() = 0;
        virtual void EndScene() = 0;
        // ~Scene

        virtual void DrawImage(const TObjectPtr<LImage>& Image) = 0;
        virtual void DrawQuad(const glm::mat4& Transform, const glm::vec4& Color) = 0;
        virtual void DrawQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color) = 0;
        virtual void DrawLine(const glm::vec2& P0, const glm::vec2& P1, const glm::vec4& Color) = 0;

        virtual float GetLineWidth() = 0;
        virtual void SetLineWidth(const float NewLineWidth) = 0;

        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> Texture) = 0;
        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> Texture, const int Slot) = 0;

        virtual const FRenderer2DSpecification& GetSpecification() const = 0;

        virtual TObjectPtr<LShader> GetQuadShader() = 0;
        virtual TObjectPtr<LShader> GetLineShader() = 0;

    protected:
        virtual void StartBatch() = 0;
        virtual void NextBatch() = 0;

        virtual void AddQuadBuffer() = 0;
        virtual void AddLineBuffer() = 0;
        virtual FQuadVertex*& GetWriteableQuadBuffer() = 0;
        virtual FLineVertex*& GetWriteableLineBuffer() = 0;

    private:
		LCLASS(LRenderer2DAPI);
	};

}
