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

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
		return nullptr;
	}

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(const uint32_t InSize, const EVertexBufferUsage BufferUsage) 
	{ 
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLVertexBuffer>::Create(InSize, BufferUsage);

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
		return nullptr;
	}

}
