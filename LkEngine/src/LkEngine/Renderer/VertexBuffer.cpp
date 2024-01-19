#include "LKpch.h"
#include "VertexBuffer.h"

#include "LkEngine/Platform/Vulkan/VulkanVertexBuffer.h"
#include "LkEngine/Platform/OpenGL/OpenGLVertexBuffer.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<VertexBuffer> VertexBuffer::Create(void* buffer, uint64_t size, VertexBufferUsage usage) 
	{ 
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(buffer, size, usage);
			case RendererAPIType::OpenGL: return Ref<OpenGLVertexBuffer>::Create(buffer, size, usage);
		}
		LK_CORE_ASSERT(false, "VertexBuffer::Create(data, size)    RendererAPI could not be determined");
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint64_t size, VertexBufferUsage usage) 
	{ 
		switch (RendererAPI::Current())
		{
			case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(size, usage);
			case RendererAPIType::OpenGL: return Ref<OpenGLVertexBuffer>::Create(size, usage);
		}
		LK_CORE_ASSERT(false, "VertexBuffer::Create(data, size)    RendererAPI could not be determined");
	}

}
