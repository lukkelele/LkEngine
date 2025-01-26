#pragma once

#include "UniformBuffer.h"


namespace LkEngine {

	class LUniformBufferSet : public LObject
	{
	public:
		virtual ~LUniformBufferSet() = default;

		virtual TObjectPtr<LUniformBuffer> Get() = 0;
		virtual TObjectPtr<LUniformBuffer> Get(const uint32_t Frame) = 0;

		virtual void Set(TObjectPtr<LUniformBuffer> InUniformBuffer, uint32_t Frame) = 0;

		static TObjectPtr<LUniformBufferSet> Create(const uint32_t InSize, const uint32_t InFramesInFlight = 0);

	private:
		LCLASS(LUniformBufferSet)
	};

}
