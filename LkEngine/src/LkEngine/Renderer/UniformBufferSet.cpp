#include "LKpch.h"
#include "UniformBufferSet.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLUniformBufferSet.h"
//#include "LkEngine/Platform/Vulkan/VulkanUniformBufferSet.h"


namespace LkEngine {

	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t size, uint32_t framesInFlight)
	{
		switch (RendererAPI::Current())
		{
			//case RendererAPIType::Vulkan: return Ref<VulkanUniformBufferSet>::Create(size, framesInFlight);
			case RendererAPIType::OpenGL: return Ref<OpenGLUniformBufferSet>::Create(size, framesInFlight);
		}
		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}