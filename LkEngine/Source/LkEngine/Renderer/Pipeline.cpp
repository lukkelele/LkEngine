#include "LKpch.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLPipeline.h"
//#include "LkEngine/Platform/Vulkan/VulkanPipeline.h"

#include "Shader.h"
#include "VertexBuffer.h"


namespace LkEngine {

	Ref<Pipeline> Pipeline::Create(const FPipelineSpecification& InPipelineSpecification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:
			{
				return Ref<OpenGLPipeline>::Create(InPipelineSpecification);
			}

			case ERendererAPI::Vulkan:
			{
				LK_CORE_ASSERT(false, "Vulkan not supported!");
				return nullptr;
				//return Ref<VulkanPipeline>::Create(spec);
			}
			case ERendererAPI::None:    
			{
				LK_CORE_ASSERT(false, "Render API is none");
				return nullptr;
			}
		}

		LK_CORE_ASSERT(false, "Unknown RendererAPI");
	}


}