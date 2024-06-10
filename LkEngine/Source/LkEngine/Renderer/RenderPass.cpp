#include "LKpch.h"
#include "RenderPass.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderPass.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderPass.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& Specification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLRenderPass>::Create(Specification);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan:  return Ref<VulkanRenderPass>::Create(spec);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}