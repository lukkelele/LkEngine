#pragma once

#include "LkEngine/Renderer/UniformBufferSet.h"

#include "OpenGLUniformBuffer.h"


namespace LkEngine {

	class LOpenGLUniformBufferSet : public LUniformBufferSet
	{
	public:
		LOpenGLUniformBufferSet(const uint32_t InSize, const uint32_t InFramesInFlight);
		LOpenGLUniformBufferSet() = delete;
		~LOpenGLUniformBufferSet() = default;

		virtual TObjectPtr<LUniformBuffer> Get() override;
		virtual TObjectPtr<LUniformBuffer> Get(const uint32_t Frame) override;

		virtual void Set(TObjectPtr<LUniformBuffer> InUniformBuffer, const uint32_t Frame) override;

	private:
		uint32_t FramesInFlight = 0;
		std::map<uint32_t, TObjectPtr<LUniformBuffer>> UniformBuffers;

		LCLASS(LOpenGLUniformBufferSet)
	};

}
