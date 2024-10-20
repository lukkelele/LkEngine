#include "LKpch.h"
#include "UniformBufferSet.h"

#include "LkEngine/Renderer/RendererAPI.h"
#include "LkEngine/Platform/OpenGL/OpenGLUniformBufferSet.h"


namespace LkEngine {

	TObjectPtr<LUniformBufferSet> LUniformBufferSet::Create(const uint32_t size, 
															const uint32_t framesInFlight)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLUniformBufferSet>::Create(size, framesInFlight);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}