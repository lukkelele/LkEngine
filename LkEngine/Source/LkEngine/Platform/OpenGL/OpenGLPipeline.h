#pragma once

#include "LkEngine/Renderer/Pipeline.h"



namespace LkEngine {

    class OpenGLPipeline : public Pipeline
    {
    public:
		OpenGLPipeline(const FPipelineSpecification& InPipelineSpecification);
		~OpenGLPipeline() override;

		void Invalidate() override;

		void SetLayout(const VertexBufferLayout& layout) override;
		Ref<Shader> GetShader() const override { return PipelineSpecification.Shader; }

		FORCEINLINE FPipelineSpecification& GetSpecification() override { return PipelineSpecification; }
		FORCEINLINE const FPipelineSpecification& GetSpecification() const override { return PipelineSpecification; }

		void BindTextureArray(const RendererID& rendererID);

		FORCEINLINE std::deque<RendererID> GetBoundTextureArrays() const { return BoundTextureArrays; }

	private:
		FPipelineSpecification PipelineSpecification;
		std::deque<RendererID> BoundTextureArrays{};

    };

}