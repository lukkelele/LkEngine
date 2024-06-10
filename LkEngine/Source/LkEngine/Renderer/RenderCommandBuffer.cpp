#include "LKpch.h"
#include "RenderCommandBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderCommandBuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderCommandBuffer.h"


namespace LkEngine {

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(uint32_t count, const std::string& debugName)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:  
			{
				return Ref<OpenGLRenderCommandBuffer>::Create(count, debugName);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(count, debugName);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	Ref<RenderCommandBuffer> RenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:  
			{
				return Ref<OpenGLRenderCommandBuffer>::Create(debugName, true);
			}

			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(debugName, true);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}