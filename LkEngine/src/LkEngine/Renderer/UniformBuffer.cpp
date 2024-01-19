#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Platform/Vulkan/VulkanUniformBuffer.h"
#include "LkEngine/Platform/OpenGL/OpenGLUniformBuffer.h"


namespace LkEngine {

	Ref<UniformBuffer> UniformBuffer::Create(uint64_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanUniformBuffer>::Create(size);
			case RendererAPIType::OpenGL: return Ref<OpenGLUniformBuffer>::Create(size);
		}
		LK_CORE_ASSERT(false, "UniformBuffer::Create(), RendererAPI could not be determined");
	}

}