#pragma once

#include "UniformBuffer.h"


namespace LkEngine {

	/**
	 * LUniformBufferSet
	 */
	class LUniformBufferSet : public LObject
	{
	public:
		virtual ~LUniformBufferSet() {}

		virtual TObjectPtr<LUniformBuffer> Get() = 0;
		virtual TObjectPtr<LUniformBuffer> RT_Get() = 0;
		virtual TObjectPtr<LUniformBuffer> Get(uint32_t frame) = 0;

		virtual void Set(TObjectPtr<LUniformBuffer> uniformBuffer, uint32_t frame) = 0;

		static TObjectPtr<LUniformBufferSet> Create(const uint32_t size, 
													const uint32_t framesInFlight = 0);

	private:
		LCLASS(LUniformBufferSet)
	};


}