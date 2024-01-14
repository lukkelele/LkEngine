#include "LKpch.h"
#include "RenderPass.h"

#include "LkEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "LkEngine/Platform/OpenGL/OpenGLRenderPass.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan:  return Ref<VulkanRenderPass>::Create(spec);
			case RendererAPIType::OpenGL:  return Ref<OpenGLRenderPass>::Create(spec);
			case RendererAPIType::None:    LK_CORE_ASSERT(false, "RendererAPI is set to RendererAPIType::None"); return nullptr;
		}
		LK_CORE_ASSERT(false, "RenderPass::Create -> No RendererAPI detected");
	}

}