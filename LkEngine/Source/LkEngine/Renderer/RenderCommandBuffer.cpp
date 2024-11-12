#include "LKpch.h"
#include "RenderCommandBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderCommandBuffer.h"


namespace LkEngine {

	TObjectPtr<LRenderCommandBuffer> LRenderCommandBuffer::Create(const uint32_t count, const std::string& debugName)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:  
			{
				return TObjectPtr<LOpenGLRenderCommandBuffer>::Create(count, debugName);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LRenderCommandBuffer> LRenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:  
			{
				return TObjectPtr<LOpenGLRenderCommandBuffer>::Create(debugName, true);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}