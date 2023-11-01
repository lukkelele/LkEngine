#include "LKpch.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include <glad/glad.h>

#ifdef LK_RENDERER_API_VULKAN
	#include "LkEngine/Renderer/Vulkan/VulkanVertexBuffer.h"
#elif defined(LK_RENDERER_API_OPENGL)
	#include "LkEngine/Renderer/OpenGL/OpenGLVertexBuffer.h"
#endif


namespace LkEngine {


	s_ptr<VertexBuffer> VertexBuffer::Create(const void* data, unsigned int size) 
	{ 
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanVertexBuffer>(data, size); 
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLVertexBuffer>(data, size); 
	#endif
	}

	s_ptr<VertexBuffer> VertexBuffer::Create(unsigned int size) 
	{ 
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanVertexBuffer>(size); 
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLVertexBuffer>(size); 
	#endif
	}

}
