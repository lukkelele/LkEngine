#include "LKpch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLIndexBuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanIndexBuffer.h"


namespace LkEngine {

	Ref<IndexBuffer> IndexBuffer::Create(void* buffer, uint64_t size)
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLIndexBuffer>::Create(buffer, size);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(buffer, size);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint64_t size)
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLIndexBuffer>::Create(size);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(size);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
	}

}
