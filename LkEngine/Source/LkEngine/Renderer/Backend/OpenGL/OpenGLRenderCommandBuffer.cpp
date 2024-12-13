#include "LKpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(uint32_t Count, std::string_view InName) 
	{
		LOBJECT_REGISTER();

		LK_UNUSED(Count);
		LK_UNUSED(InName);
	}

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(std::string_view InName, bool InSwapchain)
	{
		LOBJECT_REGISTER();

		LK_UNUSED(InName);
		LK_UNUSED(InSwapchain);
	}

	LOpenGLRenderCommandBuffer::~LOpenGLRenderCommandBuffer()
	{
	}

	void LOpenGLRenderCommandBuffer::Begin()
	{
		m_TimestampNextAvailableQuery = 2;
	}
	
	void LOpenGLRenderCommandBuffer::End()
	{
	}

	void LOpenGLRenderCommandBuffer::Submit()
	{
	}
	
	const PipelineStatistics& LOpenGLRenderCommandBuffer::GetPipelineStatistics(const uint32_t FrameIndex) const
	{
		/* Unused for now. */
		LK_UNUSED(FrameIndex);
		return m_PipelineStatistics;
	}

	uint32_t LOpenGLRenderCommandBuffer::BeginTimestampQuery()
	{
		return 1;
	}

	void LOpenGLRenderCommandBuffer::EndTimestampQuery(uint32_t queryID)
	{
	}

}
