#pragma once

#include "LkEngine/Core/Core.h"

#include "PipelineSpecification.h"


namespace LkEngine {

	class LShader;

	struct PipelineStatistics
	{
		uint64_t InputAssemblyVertices = 0;
		uint64_t InputAssemblyPrimitives = 0;
		uint64_t VertexShaderInvocations = 0;
		uint64_t ClippingInvocations = 0;
		uint64_t ClippingPrimitives = 0;
		uint64_t FragmentShaderInvocations = 0;
		uint64_t ComputeShaderInvocations = 0;
	};

	class LPipeline : public LObject
	{
	public:
		virtual ~LPipeline() = default;

		virtual FPipelineSpecification& GetSpecification() = 0;
		virtual const FPipelineSpecification& GetSpecification() const = 0;
		virtual void Invalidate() = 0;

		virtual TObjectPtr<LShader> GetShader() const = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		virtual void BindTextureArray(const LRendererID InRendererID) = 0;

		/** Factory function. */
		static TObjectPtr<LPipeline> Create(const FPipelineSpecification& InSpecification);

	private:
		LCLASS(LPipeline)
	};

}