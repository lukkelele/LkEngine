#pragma once

#include "LkEngine/Renderer/RenderCommandBuffer.h"


namespace LkEngine {

	class LOpenGLRenderCommandBuffer : public LRenderCommandBuffer
	{
	public:
		LOpenGLRenderCommandBuffer(const uint32_t Count = 0, std::string_view InDebugName = "");
		LOpenGLRenderCommandBuffer(std::string_view InDebugName, const bool InSwapchain);
		~LOpenGLRenderCommandBuffer();

		virtual void Begin() override;
		virtual void End() override;
		virtual void Submit() override;

		virtual float GetExecutionGpuTime(const uint32_t FrameIndex, const uint32_t QueryIndex = 0) const override
		{
			if ((QueryIndex == UINT32_MAX) || ((QueryIndex / 2) >= TimestampNextAvailableQuery / 2))
			{
				return 0.0f;
			}

			return ExecutionGpuTimes[FrameIndex][(QueryIndex / 2)];
		}

		virtual const FPipelineStatistics& GetPipelineStatistics(const uint32_t FrameIndex) const override;

		virtual uint32_t BeginTimestampQuery() override;
		virtual void EndTimestampQuery(const uint32_t QueryID) override;

	private:
		FPipelineStatistics PipelineStatistics;
		std::string DebugName{};

		bool bOwnedBySwapChain = false;

		uint32_t TimestampQueryCount = 0;
		uint32_t TimestampNextAvailableQuery = 2;

		std::vector<std::vector<uint64_t>> TimestampQueryResults{};
		std::vector<std::vector<float>> ExecutionGpuTimes{};

		uint32_t PipelineQueryCount = 0;

		LCLASS(LOpenGLRenderCommandBuffer);
	};

}
