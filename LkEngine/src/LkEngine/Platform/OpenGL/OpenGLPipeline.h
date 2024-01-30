#pragma once

#include "LkEngine/Renderer/Pipeline.h"



namespace LkEngine {

    class OpenGLPipeline : public Pipeline
    {
    public:
		OpenGLPipeline(const PipelineSpecification& spec);
		~OpenGLPipeline() override;

		void Invalidate() override;

		void SetLayout(const VertexBufferLayout& layout) override;
		Ref<Shader> GetShader() const override { return m_Specification.Shader; }

		PipelineSpecification& GetSpecification() override { return m_Specification; }
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }

	private:
		PipelineSpecification m_Specification;
    };

}