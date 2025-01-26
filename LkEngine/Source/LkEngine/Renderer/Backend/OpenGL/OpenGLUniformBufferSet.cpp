#include "LKpch.h"
#include "OpenGLUniformBufferSet.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LOpenGLUniformBufferSet::LOpenGLUniformBufferSet(const uint32_t InSize, const uint32_t InFramesInFlight)
		: FramesInFlight(InFramesInFlight)
	{
		LOBJECT_REGISTER();

		UniformBuffers.clear();

		if (InFramesInFlight == 0)
		{
			FramesInFlight = LRenderer::GetFramesInFlight();
		}

		for (uint32_t Frame = 0; Frame < FramesInFlight; Frame++)
		{
			UniformBuffers[Frame] = LUniformBuffer::Create(InSize);
		}
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::Get()
	{
		const uint32_t Frame = LRenderer::GetCurrentFrameIndex();
		return Get(Frame);
	}

	TObjectPtr<LUniformBuffer> LOpenGLUniformBufferSet::Get(const uint32_t Frame) 
	{
		LK_CORE_ASSERT(UniformBuffers.contains(Frame));
		return UniformBuffers.at(Frame);
	}

	void LOpenGLUniformBufferSet::Set(TObjectPtr<LUniformBuffer> InUniformBuffer, const uint32_t Frame)
	{
		UniformBuffers[Frame] = InUniformBuffer;
	}

}
