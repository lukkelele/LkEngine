#pragma once

#include "LkEngine/Core/Base.h"

#include "PipelineSpecification.h"


namespace LkEngine {

	class Shader;
	class VertexBuffer;

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

	class Pipeline : public RefCounted
	{
	public:
		virtual ~Pipeline() = default;

		virtual FPipelineSpecification& GetSpecification() = 0;
		virtual const FPipelineSpecification& GetSpecification() const = 0;
		virtual void Invalidate() = 0;
		virtual Ref<Shader> GetShader() const = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		static Ref<Pipeline> Create(const FPipelineSpecification& InPipelineSpecification);
	};

}