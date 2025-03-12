#include "LKpch.h"
#include "OpenGLRenderCommandBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(const uint32_t Count, std::string_view InDebugName) 
		: DebugName(InDebugName)
	{
		LOBJECT_REGISTER();

		LK_UNUSED(Count);
		LK_UNUSED(InDebugName);

		LK_CORE_TRACE_TAG("OpenGLRenderCommandBuffer", "Created command buffer ({}), count: {}", 
						  (!InDebugName.empty() ? InDebugName : "Unnamed"), Count);
	}

	LOpenGLRenderCommandBuffer::LOpenGLRenderCommandBuffer(std::string_view InDebugName, const bool InSwapchain)
	{
		LOBJECT_REGISTER();

		LK_UNUSED(InSwapchain);
		LK_CORE_TRACE_TAG("OpenGLRenderCommandBuffer", "Created command buffer ({})", (!InDebugName.empty() ? InDebugName : "Unnamed"));
	}

	LOpenGLRenderCommandBuffer::~LOpenGLRenderCommandBuffer()
	{
	}

	void LOpenGLRenderCommandBuffer::Begin()
	{
		TimestampNextAvailableQuery = 2;
	}
	
	void LOpenGLRenderCommandBuffer::End()
	{
	}

	void LOpenGLRenderCommandBuffer::Submit()
	{
	}
	
	const FPipelineStatistics& LOpenGLRenderCommandBuffer::GetPipelineStatistics(const uint32_t FrameIndex) const
	{
		/* Unused for now. */
		LK_UNUSED(FrameIndex);
		return PipelineStatistics;
	}

	uint32_t LOpenGLRenderCommandBuffer::BeginTimestampQuery()
	{
		return 1;
	}

	void LOpenGLRenderCommandBuffer::EndTimestampQuery(const uint32_t QueryID)
	{
	}

}
