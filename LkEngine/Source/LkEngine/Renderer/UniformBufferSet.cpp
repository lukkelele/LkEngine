#include "LKpch.h"
#include "UniformBufferSet.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLUniformBufferSet.h"
//#include "LkEngine/Platform/Vulkan/VulkanUniformBufferSet.h"


namespace LkEngine {

	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t size, uint32_t framesInFlight)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLUniformBufferSet>::Create(size, framesInFlight);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanUniformBufferSet>::Create(size, framesInFlight);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}