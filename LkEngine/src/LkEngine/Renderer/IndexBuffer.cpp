#include "LKpch.h"
#include "IndexBuffer.h"
#include "Renderer.h"

#ifdef LK_RENDERER_API_OPENGL
	#include "LkEngine/Platform/OpenGL/OpenGLIndexBuffer.h"
#elif defined(LK_RENDERER_API_VULKAN)
	#include "LkEngine/Platform/Vulkan/VulkanIndexBuffer.h"
#endif

namespace LkEngine {

	s_ptr<IndexBuffer> IndexBuffer::Create(const void* data, unsigned int count)
	{ 
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return std::make_shared<OpenGLIndexBuffer>(data, count);
			case RendererAPIType::Vulkan: return nullptr;
			case RendererAPIType::None:   return nullptr;
		}
	}


}
