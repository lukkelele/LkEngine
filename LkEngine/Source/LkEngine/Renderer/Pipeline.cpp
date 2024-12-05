#include "LKpch.h"

#include "RendererAPI.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLPipeline.h"

#include "Shader.h"
#include "VertexBuffer.h"


namespace LkEngine {

	TObjectPtr<LPipeline> LPipeline::Create(const FPipelineSpecification& InSpecification)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:
			{
				return TObjectPtr<LOpenGLPipeline>::Create(InSpecification);
			}

			case ERendererAPI::Vulkan:
			{
				LK_CORE_ASSERT(false, "Vulkan not supported right now");
				return nullptr;
			}

			case ERendererAPI::None:    
			{
				LK_CORE_ASSERT(false, "RenderAPI is none");
				return nullptr;
			}
		}

		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
