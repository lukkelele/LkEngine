#pragma once

#include "Pipeline.h"
#include "RenderCommandQueue.h"

namespace LkEngine {

	/**
	 * LRenderCommandBuffer
	 */
	class LRenderCommandBuffer : public LObject
	{
	public:
		virtual ~LRenderCommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual float GetExecutionGpuTime(const uint32_t FrameIndex, const uint32_t QueryIndex = 0) const = 0;
		virtual const FPipelineStatistics& GetPipelineStatistics(const uint32_t FrameIndex) const = 0;

		virtual uint32_t BeginTimestampQuery() = 0;
		virtual void EndTimestampQuery(const uint32_t QueryID) = 0;

		static TObjectPtr<LRenderCommandBuffer> Create(const uint32_t Count = 0, const std::string& InDebugName = "");
		static TObjectPtr<LRenderCommandBuffer> CreateFromSwapChain(const std::string& InDebugName = "");

	private:
		LCLASS(LRenderCommandBuffer);
	};


}
