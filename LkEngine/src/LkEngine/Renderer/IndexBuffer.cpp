#include "LKpch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "LkEngine/Platform/Vulkan/VulkanIndexBuffer.h"


namespace LkEngine {

	Ref<IndexBuffer> IndexBuffer::Create(void* buffer, uint64_t size)
	{ 
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(buffer, size);
			case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::Create(buffer, size);
			case RendererAPIType::None:   return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint64_t size)
	{ 
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(size);
			case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::Create(size);
			case RendererAPIType::None:   return nullptr;
		}
	}

}
