#include "LKpch.h"
#include "LkEngine/Renderer/UniformBuffer.h"

#include "LkEngine/Renderer/RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLUniformBuffer.h"
//#include "LkEngine/Platform/Vulkan/VulkanUniformBuffer.h"


namespace LkEngine {

	Ref<UniformBuffer> UniformBuffer::Create(uint64_t size)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLUniformBuffer>::Create(size);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanUniformBuffer>::Create(size);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

}