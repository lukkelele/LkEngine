#include "LKpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLRenderCommandBuffer::OpenGLRenderCommandBuffer(uint32_t count, std::string debugName) 
	{
	}

	OpenGLRenderCommandBuffer::OpenGLRenderCommandBuffer(std::string debugName, bool swapchain)
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
	
	const PipelineStatistics& OpenGLRenderCommandBuffer::GetPipelineStatistics(uint32_t frameIndex) const
	{
		(void)(frameIndex); // unused for now
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