#include "LKpch.h"
#include "RenderCommandQueue.h"

#include "Renderer.h"


namespace LkEngine {

	LRenderCommandQueue::LRenderCommandQueue()
	{
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024]; // 10MB Buffer
		m_CommandBufferPtr = m_CommandBuffer;
		std::memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	LRenderCommandQueue::~LRenderCommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* LRenderCommandQueue::Allocate(FRenderCommandFn CommandFunction, const uint32_t InSize)
	{
		*(FRenderCommandFn*)m_CommandBufferPtr = CommandFunction;
		m_CommandBufferPtr += sizeof(FRenderCommandFn);

		*(uint32_t*)m_CommandBufferPtr = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;

		return memory;
	}

	void LRenderCommandQueue::Execute()
	{
		byte* CommandBuffer = m_CommandBuffer;

		LFramebuffer& ViewportFramebuffer = *LRenderer::GetViewportFramebuffer();
		ViewportFramebuffer.Bind();
		ViewportFramebuffer.BindTexture(0);

		for (uint32_t i = 0; i < m_CommandCount; i++)
		{
			FRenderCommandFn Function = *(FRenderCommandFn*)CommandBuffer;
			CommandBuffer += sizeof(FRenderCommandFn);

			const uint32_t Size = *(uint32_t*)CommandBuffer;
			CommandBuffer += sizeof(uint32_t);
			Function(CommandBuffer);
			CommandBuffer += Size;
		}

		ViewportFramebuffer.Unbind();

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

}
