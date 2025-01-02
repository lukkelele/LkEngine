#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	class LRenderCommandQueue
	{
	public:
		typedef void(*FRenderCommandFn)(void*);

		LRenderCommandQueue();
		~LRenderCommandQueue();

		void* Allocate(FRenderCommandFn CommandFunction, const uint32_t Size);
		void Execute();

	private:
		uint8_t* m_CommandBuffer{};
		uint8_t* m_CommandBufferPtr{};
		uint32_t m_CommandCount = 0;
	};

}
