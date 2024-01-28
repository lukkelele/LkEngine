#pragma once

#include "LkEngine/Renderer/RenderCommandBuffer.h"


namespace LkEngine {

	class OpenGLRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		OpenGLRenderCommandBuffer(uint32_t count = 0, std::string debugName = "");
		OpenGLRenderCommandBuffer(std::string debugName, bool swapchain);
		~OpenGLRenderCommandBuffer() override;

		virtual void Begin() override;
		virtual void End() override;
		virtual void Submit() override;

		virtual float GetExecutionGPUTime(uint32_t frameIndex, uint32_t queryIndex = 0) const override
		{
			if (queryIndex == UINT32_MAX || queryIndex / 2 >= m_TimestampNextAvailableQuery / 2)
				return 0.0f;

			return m_ExecutionGPUTimes[frameIndex][queryIndex / 2];
		}

		const PipelineStatistics& GetPipelineStatistics(uint32_t frameIndex) const;

		uint32_t BeginTimestampQuery() override;
		void EndTimestampQuery(uint32_t queryID) override;

	private:
		std::string m_DebugName;
		PipelineStatistics m_PipelineStatistics;

		bool m_OwnedBySwapChain = false;

		uint32_t m_TimestampQueryCount = 0;
		uint32_t m_TimestampNextAvailableQuery = 2;

		std::vector<std::vector<uint64_t>> m_TimestampQueryResults;
		std::vector<std::vector<float>> m_ExecutionGPUTimes;

		uint32_t m_PipelineQueryCount = 0;
	};

}
