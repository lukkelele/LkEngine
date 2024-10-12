#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Platform/OpenGL/OpenGLUniformBuffer.h"


namespace LkEngine {

	TObjectPtr<LUniformBuffer> LUniformBuffer::Create(uint64_t size)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLUniformBuffer>::Create(size);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}