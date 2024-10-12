#include "LKpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLRenderCommandBuffer::OpenGLRenderCommandBuffer(uint32_t Count, std::string_view InDebugName) 
	{
	}

	OpenGLRenderCommandBuffer::OpenGLRenderCommandBuffer(std::string_view InDebugName, bool bSwapchain)
	{
	}

	OpenGLRenderCommandBuffer::~OpenGLRenderCommandBuffer()
	{
	}

	void OpenGLRenderCommandBuffer::Begin()
	{
		m_TimestampNextAvailableQuery = 2;
	}
	
	void OpenGLRenderCommandBuffer::End()
	{
	}

	void OpenGLRenderCommandBuffer::Submit()
	{
	}
	
	const PipelineStatistics& OpenGLRenderCommandBuffer::GetPipelineStatistics(const uint32_t FrameIndex) const
	{
		/* Unused for now. */
		LK_UNUSED(FrameIndex);
		return m_PipelineStatistics;
	}

	uint32_t OpenGLRenderCommandBuffer::BeginTimestampQuery()
	{
		return 1;
	}

	void OpenGLRenderCommandBuffer::EndTimestampQuery(uint32_t queryID)
	{
	}


}