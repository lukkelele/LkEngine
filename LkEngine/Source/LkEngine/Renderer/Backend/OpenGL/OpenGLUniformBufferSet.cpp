#include "LKpch.h"
#include "OpenGLUniformBufferSet.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LOpenGLUniformBufferSet::LOpenGLUniformBufferSet(const uint32_t size, 
													 const uint32_t framesInFlight)
		: m_FramesInFlight(framesInFlight)
	{
		if (framesInFlight == 0)
		{
			m_FramesInFlight = LRenderer::GetFramesInFlight();
		}

		for (uint32_t frame = 0; frame < m_FramesInFlight; frame++)
		{
			m_UniformBuffers[frame] = LUniformBuffer::Create(size);
		}
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::Get()
	{
		uint32_t frame = LRenderer::GetCurrentFrameIndex();
		return Get(frame);
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::RT_Get()
	{
		uint32_t frame = LRenderer::RT_GetCurrentFrameIndex();
		return Get(frame);
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::Get(uint32_t frame) 
	{
		LK_CORE_ASSERT(m_UniformBuffers.find(frame) != m_UniformBuffers.end());
		return m_UniformBuffers.at(frame);
	}

	void LOpenGLUniformBufferSet::Set(TObjectPtr<LUniformBuffer> uniformBuffer, 
									  uint32_t frame)
	{
		m_UniformBuffers[frame] = uniformBuffer;
	}

}
