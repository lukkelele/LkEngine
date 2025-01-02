#include "LKpch.h"
#include "RenderCommandBuffer.h"

#include "RendererAPI.h"

#include "Backend/OpenGL/OpenGLRenderCommandBuffer.h"


namespace LkEngine {

	TObjectPtr<LRenderCommandBuffer> LRenderCommandBuffer::Create(const uint32_t Count, const std::string& DebugName)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:  
			{
				return TObjectPtr<LOpenGLRenderCommandBuffer>::Create(Count, DebugName);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LRenderCommandBuffer> LRenderCommandBuffer::CreateFromSwapChain(const std::string& DebugName)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:  
			{
				return TObjectPtr<LOpenGLRenderCommandBuffer>::Create(DebugName, true);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}
