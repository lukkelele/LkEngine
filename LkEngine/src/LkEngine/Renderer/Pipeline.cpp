#include "LKpch.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLPipeline.h"
//#include "LkEngine/Platform/Vulkan/VulkanPipeline.h"

#include "Shader.h"
#include "VertexBuffer.h"


namespace LkEngine {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:  return Ref<OpenGLPipeline>::Create(spec);
			//case RendererAPIType::Vulkan:  return Ref<VulkanPipeline>::Create(spec);
			case RendererAPIType::None:    return nullptr;
		}
		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		throw std::runtime_error("Pipeline::Create   Unknown RendererAPI");
	}


}