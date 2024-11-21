#pragma once

#include "LkEngine/Renderer/RendererCore.h"
#include "LkEngine/Renderer/Renderer2DAPI.h"

#include "OpenGLFramebuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLArrayTexture.h"


namespace LkEngine {

    class LEntity;

    struct FOpenGLRenderer2DSpecification : public FRenderer2DSpecification
    {
        uint8_t TextureArraysUsed = 1;
    };

    /// REMOVE THIS ENTIRE CLASSSSSSSSSSSSSSS
	/* REMOVE THIS CLASS ! */
	/* TODO: Wait until IRenderer2D is available.*/
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
