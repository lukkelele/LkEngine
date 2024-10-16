#include "LKpch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLIndexBuffer.h"


namespace LkEngine {

	TObjectPtr<LIndexBuffer> LIndexBuffer::Create(void* InBuffer, const uint64_t InSize)
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLIndexBuffer>::Create(InBuffer, InSize);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LIndexBuffer> LIndexBuffer::Create(const uint64_t InSize)
	{ 
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLIndexBuffer>::Create(InSize);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}
