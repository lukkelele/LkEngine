#include "LKpch.h"
#include "VertexBuffer.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLVertexBuffer.h"

#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(void* InBuffer, const uint32_t InSize, const EVertexBufferUsage BufferUsage) 
	{ 
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLVertexBuffer>::Create(InBuffer, InSize, BufferUsage);

			case ERendererAPI::Vulkan:
			default: break;
		}

		LK_CORE_VERIFY(false);
		return nullptr;
	}

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(const void* InBuffer, const uint32_t InSize, const EVertexBufferUsage BufferUsage) 
	{ 
		/**
		 * This function serves as a way to allow the creation of a LVertexBuffer 
		 * with a const data structure such as an constexpr float array present in 'GeometryPool.h'
		 * without having to cast to void* beforehand.
		 */
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLVertexBuffer>::Create(const_cast<void*>(InBuffer), InSize, BufferUsage);

			case ERendererAPI::Vulkan:
			default: break;
		}

		LK_CORE_VERIFY(false);
		return nullptr;
	}

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(const uint32_t InSize, const EVertexBufferUsage BufferUsage) 
	{ 
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLVertexBuffer>::Create(InSize, BufferUsage);

			case ERendererAPI::Vulkan:
			default: break;
		}

		LK_CORE_VERIFY(false);
		return nullptr;
	}

}
