#pragma once

#include "LkEngine/Renderer/Pipeline.h"


namespace LkEngine {

    class LOpenGLPipeline : public LPipeline
    {
    public:
		LOpenGLPipeline(const FPipelineSpecification& InPipelineSpecification);
		~LOpenGLPipeline() = default;

		virtual void Invalidate() override;

		virtual void SetLayout(const VertexBufferLayout& layout) override;

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

		void BindTextureArray(const RendererID& rendererID);

		FORCEINLINE std::deque<RendererID> GetBoundTextureArrays() const 
		{
			return BoundTextureArrays; 
		}

	private:
		FPipelineSpecification PipelineSpecification;
		std::deque<RendererID> BoundTextureArrays{};
    };

}