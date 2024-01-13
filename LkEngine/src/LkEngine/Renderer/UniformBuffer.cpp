#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Platform/Vulkan/VulkanUniformBuffer.h"
#include "LkEngine/Platform/OpenGL/OpenGLUniformBuffer.h"


namespace LkEngine {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLUniformBuffer>::Create(size, binding);
			case RendererAPIType::Vulkan: return Ref<VulkanUniformBuffer>::Create(size, binding);
		}
		LK_CORE_ASSERT(false, "UniformBuffer::Create(), RendererAPI could not be determined");
	}

}