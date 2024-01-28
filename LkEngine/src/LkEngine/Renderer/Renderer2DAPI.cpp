#include "LKpch.h"
#include "Renderer2DAPI.h"

#include "RendererAPI.h"

#include "LkEngine/Platform/OpenGL/OpenGLRenderer2D.h"
//#include "LkEngine/Platform/Vulkan/VulkanRenderer2D.h"


namespace LkEngine {

	Ref<Renderer2DAPI> Renderer2DAPI::Create(const Renderer2DSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLRenderer2D>::Create(specification);
			//case RendererAPIType::Vulkan: return Ref<VulkanRenderer2D>::Create(specification);
		}
		LK_CORE_ASSERT(false, "No render API detected");
	}

}