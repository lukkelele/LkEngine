#include "LKpch.h"
#include "RenderCommandBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderCommandBuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderCommandBuffer.h"


namespace LkEngine {

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(uint32_t count, const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:  return Ref<OpenGLRenderCommandBuffer>::Create(count, debugName);
			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(count, debugName);
			case RendererAPIType::None:    return nullptr;
		}
		LK_CORE_ASSERT(false, "Unknown RendererAPI");
	}

	Ref<RenderCommandBuffer> RenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:  return Ref<OpenGLRenderCommandBuffer>::Create(debugName, true);
			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(debugName, true);
			case RendererAPIType::None:    return nullptr;
		}
		LK_CORE_ASSERT(false, "Unknown RendererAPI");
	}



}