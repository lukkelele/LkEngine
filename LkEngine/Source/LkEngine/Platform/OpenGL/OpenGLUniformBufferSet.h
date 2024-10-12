#pragma once

#include "LkEngine/Renderer/UniformBufferSet.h"

#include "OpenGLUniformBuffer.h"


namespace LkEngine {

	class LOpenGLUniformBufferSet : public LUniformBufferSet
	{
	public:
		LOpenGLUniformBufferSet(uint32_t size, uint32_t framesInFlight);
		~LOpenGLUniformBufferSet() = default;

		TObjectPtr<LUniformBuffer> Get() override;
		TObjectPtr<LUniformBuffer> RT_Get() override;
		TObjectPtr<LUniformBuffer> Get(uint32_t frame) override;

		void Set(TObjectPtr<LUniformBuffer> uniformBuffer, uint32_t frame) override;

	private:
		uint32_t m_FramesInFlight = 0;
		std::map<uint32_t, TObjectPtr<LUniformBuffer>> m_UniformBuffers;

		LCLASS(LOpenGLUniformBufferSet)
	};



}