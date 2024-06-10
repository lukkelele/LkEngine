#include "LKpch.h"
#include "VertexBuffer.h"

#include "LkEngine/Platform/OpenGL/OpenGLVertexBuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanVertexBuffer.h"

#include "RendererAPI.h"


namespace LkEngine {

	Ref<VertexBuffer> VertexBuffer::Create(void* buffer, uint64_t size, VertexBufferUsage usage) 
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return Ref<OpenGLVertexBuffer>::Create(buffer, size, usage);

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(buffer, size, usage);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint64_t size, VertexBufferUsage usage) 
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return Ref<OpenGLVertexBuffer>::Create(size, usage);

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(size, usage);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
	}

}
