#include "LKpch.h"
#include "VertexBuffer.h"

#include "LkEngine/Platform/OpenGL/OpenGLVertexBuffer.h"

#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(void* InBuffer, 
													const uint64_t InSize, 
													const EVertexBufferUsage InBufferUsage) 
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<OpenGLVertexBuffer>::Create(InBuffer, InSize, InBufferUsage);

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
		return nullptr;
	}

	TObjectPtr<LVertexBuffer> LVertexBuffer::Create(const uint64_t InSize, const EVertexBufferUsage InBufferUsage) 
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<OpenGLVertexBuffer>::Create(InSize, InBufferUsage);

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Renderer API");
		return nullptr;
	}

}
