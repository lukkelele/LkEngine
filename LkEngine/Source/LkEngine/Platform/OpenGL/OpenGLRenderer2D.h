#pragma once

#include "LkEngine/Renderer/RendererCore.h"
#include "LkEngine/Renderer/Renderer2DAPI.h"

#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureArray.h"



namespace LkEngine {

    class LEntity;

    struct FOpenGLRenderer2DSpecification : public FRenderer2DSpecification
    {
        uint8_t TextureArraysUsed = 1;
    };

	/* TODO: Wait until IRenderer2D is available.*/
	//class LOpenGLRenderer2D : public IRenderer2DAPI, public LObject
	class LOpenGLRenderer2D : public LRenderer2DAPI
	{
    public:
        LOpenGLRenderer2D(const FOpenGLRenderer2DSpecification& InSpecification = FOpenGLRenderer2DSpecification());
        ~LOpenGLRenderer2D() = default;

        virtual void Initialize() override;
        virtual void Destroy() override;

        FORCEINLINE virtual TObjectPtr<LShader> GetQuadShader() override
        { 
            return m_QuadShader; 
        }

        FORCEINLINE virtual TObjectPtr<LShader> GetLineShader() override 
        { 
            return m_LineShader; 
        }

    #if 0
        virtual void Init() override;
        virtual void Shutdown() override;

        virtual void BeginScene(const LSceneCamera& camera) override;
        virtual void BeginScene(const LSceneCamera& camera, const glm::mat4& transform) override;
        virtual void BeginScene(const glm::mat4& viewProjectionMatrix) override;
        virtual void EndScene() override;
        virtual void Flush() override;

        virtual void DrawImage(const TObjectPtr<LImage>& Image) override;




        // DrawLine
        virtual void DrawLine(const glm::vec2& p0, 
                              const glm::vec2& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) override;

        virtual void DrawLine(const glm::vec3& p0, 
                              const glm::vec3& p1, 
                              const glm::vec4& color, 
                              uint64_t entityID = 0) override;
        // ~DrawLine


        const Renderer2DSpecification& GetSpecification() const override { return m_Specification; }

        /// FIXME
        virtual float GetLineWidth() override;
        virtual void SetLineWidth(float width) override;

        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture) override;
        virtual void AddTextureToSlot(TObjectPtr<LTexture2D> texture, int slot) override;

        void ResetStats() override;
        Statistics GetStats() override;

    private:
        virtual void StartBatch() override;
        virtual void NextBatch() override;

        virtual void AddQuadBuffer() override;
        virtual void AddLineBuffer() override;

        virtual FQuadVertex*& GetWriteableQuadBuffer() override;
        virtual FLineVertex*& GetWriteableLineBuffer() override;

        void AddTextureArray(const TObjectPtr<OpenGLTextureArray>& textureArray);

    public:
        static constexpr int MaxTextureSlots = 32;
        static constexpr int MaxTextureArrays = 10;
    private:
        int m_Topology;

        RenderCommandQueue m_RenderQueue;

        const uint32_t m_MaxVertices = 0;
        const uint32_t m_MaxIndices = 0;
        const uint32_t m_MaxLineVertices;
        const uint32_t m_MaxLineIndices;

		TObjectPtr<LRenderCommandBuffer> m_RenderCommandBuffer;


        /* Texture Slots. */
        std::array<TObjectPtr<LTexture2D>, MaxTextureSlots> m_TextureSlots;

        /* Texture Arrays. */
        std::array<TObjectPtr<OpenGLTextureArray>, MaxTextureArrays> m_TextureArrays;
    #endif
        FOpenGLRenderer2DSpecification m_Specification;

        /* Quad. */
        uint32_t m_QuadIndexCount = 0;
		TObjectPtr<LRenderPass> m_QuadPass;
		TObjectPtr<LMaterial> m_QuadMaterial;
        FQuadVertex* m_QuadVertexBufferBase = nullptr;
        FQuadVertex* m_QuadVertexBufferPtr = nullptr;
        TObjectPtr<LShader> m_QuadShader;
		TObjectPtr<LIndexBuffer> m_QuadIndexBuffer;
        TObjectPtr<LVertexBuffer> m_QuadVertexBuffer;
        glm::vec4 m_QuadVertexPositions[4] = {};

        /* Line.*/
        float m_LineWidth = 3.0f;
        uint32_t m_LineIndexCount = 0;
		TObjectPtr<LVertexBuffer> m_LineVertexBuffer{};
		TObjectPtr<LShader> m_LineShader{};
		FLineVertex* m_LineVertexBufferBase{};
		FLineVertex* m_LineVertexBufferPtr{};
		TObjectPtr<LIndexBuffer> m_LineIndexBuffer{};
		TObjectPtr<LMaterial> m_LineMaterial{};

        TObjectPtr<LShader> m_TextureShader = nullptr;
        TObjectPtr<LTexture2D> m_WhiteTexture = nullptr;

        struct FCameraData
        {
            glm::mat4 ViewProjection{};
        };
        FCameraData CameraBuffer{};
        TObjectPtr<LOpenGLUniformBuffer> CameraUniformBuffer;

        //FRenderer2DStatistics Statistics;

        friend class LEditorLayer;
        friend class LOpenGLRenderer;
        friend class OpenGLRenderer; /// REMOVE

        LCLASS(LOpenGLRenderer2D)
	};


}