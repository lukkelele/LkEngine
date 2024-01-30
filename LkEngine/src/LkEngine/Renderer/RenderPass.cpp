#include "LKpch.h"
#include "RenderPass.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderPass.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderPass.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:  return Ref<OpenGLRenderPass>::Create(spec);
			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderPass>::Create(spec);
		}
		LK_CORE_ASSERT(false, "Could not determine RenderAPI");
	}

}