#pragma once

#include "LkEngine/Renderer/Pipeline.h"


namespace LkEngine {

	/** @TODO: Needs fixing */
    class LOpenGLPipeline : public LPipeline
    {
    public:
		LOpenGLPipeline(const FPipelineSpecification& InPipelineSpecification);
		~LOpenGLPipeline() = default;

		virtual void Invalidate() override;

		virtual void SetLayout(const FVertexBufferLayout& InLayout) override;

		FORCEINLINE virtual TObjectPtr<LShader> GetShader() const override 
		{ 
			return PipelineSpecification.Shader; 
		}

		FORCEINLINE virtual FPipelineSpecification& GetSpecification() override 
		{ 
			return PipelineSpecification; 
		}

		FORCEINLINE virtual const FPipelineSpecification& GetSpecification() const override 
		{ 
			return PipelineSpecification; 
		}

		virtual void BindTextureArray(const LRendererID TextureArrayID) override;

		FORCEINLINE std::deque<LRendererID>& GetBoundTextureArrays()
		{
			return BoundTextureArrays; 
		}

		FORCEINLINE const std::deque<LRendererID>& GetBoundTextureArrays() const 
		{
			return BoundTextureArrays; 
		}

	private:
		FPipelineSpecification PipelineSpecification;
		std::deque<LRendererID> BoundTextureArrays{};

		LCLASS(LOpenGLPipeline);
    };

}
