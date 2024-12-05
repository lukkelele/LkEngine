#include "LKpch.h"
#include "RenderPass.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderPass.h"

#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LRenderPass> LRenderPass::Create(const FRenderPassSpecification& Specification)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLRenderPass>::Create(Specification);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}
