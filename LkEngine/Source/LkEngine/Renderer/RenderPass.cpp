#include "LKpch.h"
#include "RenderPass.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderPass.h"

#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LRenderPass> LRenderPass::Create(const RenderPassSpecification& Specification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLRenderPass>::Create(Specification);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}