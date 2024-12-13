#include "LKpch.h"
#include "OpenGLUniformBufferSet.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LOpenGLUniformBufferSet::LOpenGLUniformBufferSet(const uint32_t InSize, const uint32_t InFramesInFlight)
		: m_FramesInFlight(InFramesInFlight)
	{
		LOBJECT_REGISTER();
		if (InFramesInFlight == 0)
		{
			m_FramesInFlight = LRenderer::GetFramesInFlight();
		}

		for (uint32_t Frame = 0; Frame < m_FramesInFlight; Frame++)
		{
			m_UniformBuffers[Frame] = LUniformBuffer::Create(InSize);
		}
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::Get()
	{
		const uint32_t Frame = LRenderer::GetCurrentFrameIndex();
		return Get(Frame);
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::RT_Get()
	{
		const uint32_t Frame = LRenderer::RT_GetCurrentFrameIndex();
		return Get(Frame);
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
