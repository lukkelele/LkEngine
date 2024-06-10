#include "LKpch.h"
#include "OpenGLUniformBufferSet.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	OpenGLUniformBufferSet::OpenGLUniformBufferSet(uint32_t size, uint32_t framesInFlight)
		: m_FramesInFlight(framesInFlight)
	{
		if (framesInFlight == 0)
			m_FramesInFlight = Renderer::GetFramesInFlight();

		for (uint32_t frame = 0; frame < m_FramesInFlight; frame++)
			m_UniformBuffers[frame] = UniformBuffer::Create(size);
	}

	OpenGLUniformBufferSet::~OpenGLUniformBufferSet()
	{
	}

	Ref<UniformBuffer> OpenGLUniformBufferSet::Get()
	{
		uint32_t frame = Renderer::GetCurrentFrameIndex();
		return Get(frame);
	}

	Ref<UniformBuffer> OpenGLUniformBufferSet::RT_Get()
	{
		uint32_t frame = Renderer::RT_GetCurrentFrameIndex();
		return Get(frame);
	}

	Ref<UniformBuffer> OpenGLUniformBufferSet::Get(uint32_t frame) 
	{
		LK_CORE_ASSERT(m_UniformBuffers.find(frame) != m_UniformBuffers.end());
		return m_UniformBuffers.at(frame);
	}

	void OpenGLUniformBufferSet::Set(Ref<UniformBuffer> uniformBuffer, uint32_t frame)
	{
		m_UniformBuffers[frame] = uniformBuffer;
	}

}
