#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "Renderer2DAPI.h"


namespace LkEngine {

    class LImage;
    class LSceneCamera;
    class LTexture;
	class LTextureArray;

	class LRenderer2D : public LObject
    {
    public:
		LRenderer2D(const FRenderer2DSpecification InSpecification);
		~LRenderer2D() = default;

        void Initialize();
        void Shutdown();

        void BeginScene(const LSceneCamera& Camera);
        void BeginScene(const LSceneCamera& Camera, const glm::mat4& Transform);
        void EndScene();

	private:
		void StartBatch();
		void NextBatch();
        void Flush();

    public:
        void DrawImage(const TObjectPtr<LImage> Image);

        void DrawLine(const glm::vec2& P0, const glm::vec2& P1, const glm::vec4& Color);

		void DrawQuad(const glm::mat4& Transform, const glm::vec4& Color);
        void DrawQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color);
		void DrawQuad(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color);
		void DrawQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture);

		void DrawQuad(const glm::vec2& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture, const glm::vec4& TintColor);
		void DrawQuad(const glm::vec3& Position, const glm::vec2& Size, TObjectPtr<LTexture> Texture, const glm::vec4& TintColor);
        
        void DrawRotatedQuad(const glm::vec2& Position, const glm::vec2& Size, const glm::vec4& Color, const float Rotation);

        float GetLineWidth();
        void SetLineWidth(const float NewLineWidth);

        FORCEINLINE FQuadVertex*& GetWriteableQuadBuffer() { return QuadVertexBufferPtr; }
		FORCEINLINE FLineVertex*& GetWriteableLineBuffer() { return LineVertexBufferPtr; }

		struct FRendererStatistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t QuadVertexArray_RendererID = 0;
			uint32_t QuadVertexBuffer_RendererID = 0;
			uint32_t LineCount = 0;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};

        /* TODO: Change type to LVector2. */
        inline static constexpr glm::vec2 TextureCoords[] = { 
            { 0.0f, 0.0f },     /*  Bottom Left.  */
            { 0.0f, 1.0f },     /*  Top Left.     */
            { 1.0f, 1.0f },     /*  Top Right.    */
            { 1.0f, 0.0f }      /*  Bottom Right. */
        };
        
	public:
        static constexpr int MaxTextureSlots = 32;
        static constexpr int MaxTextureArrays = 10;
	private:
		FRenderer2DSpecification Specification{};
		FRendererStatistics Statistics{};

        const uint32_t MaxVertices = 0;
        const uint32_t MaxIndices = 0;
        const uint32_t MaxLineVertices = 0;
        const uint32_t MaxLineIndices = 0;

        /* Texture Slots. */
        std::array<TObjectPtr<LTexture2D>, MaxTextureSlots> TextureSlots;

        /* Texture Arrays. */
        std::array<TObjectPtr<LTextureArray>, MaxTextureArrays> TextureArrays;

        /* Quad. */
        uint32_t QuadIndexCount = 0;
		FQuadVertex* QuadVertexBufferBase{};
		FQuadVertex* QuadVertexBufferPtr{};
		TObjectPtr<LRenderPass> QuadPass{};
		TObjectPtr<LMaterial> QuadMaterial;
		TObjectPtr<LShader> QuadShader{};
	    TObjectPtr<LIndexBuffer> QuadIndexBuffer{};
        TObjectPtr<LVertexBuffer> QuadVertexBuffer;
        glm::vec4 QuadVertexPositions[4] = {};

        /* Line.*/
        float LineWidth = 3.0f;
        uint32_t LineIndexCount = 0;
		TObjectPtr<LVertexBuffer> LineVertexBuffer{};
		TObjectPtr<LShader> LineShader{};
		FLineVertex* LineVertexBufferBase{};
		FLineVertex* LineVertexBufferPtr{};
		TObjectPtr<LIndexBuffer> LineIndexBuffer{};
		TObjectPtr<LMaterial> LineMaterial{};

        TObjectPtr<LShader> TextureShader{};
		TObjectPtr<LTexture2D> WhiteTexture{};

		TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer;

        struct FCameraData
        {
            glm::mat4 ViewProjection{};
        };
        FCameraData CameraBuffer{};
        TObjectPtr<LUniformBuffer> CameraUniformBuffer;

    private:
		LCLASS(LRenderer2D);
    };

}

