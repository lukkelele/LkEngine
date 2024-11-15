#include "LKpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(uint32_t Count, std::string_view InDebugName) 
	{
	}

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(std::string_view InDebugName, bool bSwapchain)
	{
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