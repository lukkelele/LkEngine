#pragma once

#include "LkEngine/Renderer/UniformBufferSet.h"

#include "OpenGLUniformBuffer.h"


namespace LkEngine {

	class OpenGLUniformBufferSet : public UniformBufferSet
	{
	public:
		OpenGLUniformBufferSet(uint32_t size, uint32_t framesInFlight);
		~OpenGLUniformBufferSet();

		Ref<UniformBuffer> Get() override;
		Ref<UniformBuffer> RT_Get() override;
		Ref<UniformBuffer> Get(uint32_t frame) override;

		void Set(Ref<UniformBuffer> uniformBuffer, uint32_t frame) override;

	private:
		uint32_t m_FramesInFlight = 0;
		std::map<uint32_t, Ref<UniformBuffer>> m_UniformBuffers;
	};



}