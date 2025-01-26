#include "LKpch.h"
#include "UniformBufferSet.h"

#include "LkEngine/Renderer/RendererAPI.h"

#if defined(LK_ENGINE_OPENGL)
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLUniformBufferSet.h"
#endif


namespace LkEngine {

	TObjectPtr<LUniformBufferSet> LUniformBufferSet::Create(const uint32_t InSize, const uint32_t InFramesInFlight)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLUniformBufferSet>::Create(InSize, InFramesInFlight);

			case ERendererAPI::Vulkan:
			default: break;
		}

		LK_CORE_VERIFY(false);
		return nullptr;
	}

}
