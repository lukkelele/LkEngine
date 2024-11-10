#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Platform/OpenGL/OpenGLUniformBuffer.h"

#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

	TObjectPtr<LUniformBuffer> LUniformBuffer::Create(const uint64_t Size)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLUniformBuffer>::Create(Size);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}