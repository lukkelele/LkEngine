#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Platform/OpenGL/OpenGLTexture.h"
//#include "LkEngine/Platform/Vulkan/VulkanTexture.h"


namespace LkEngine {

	Ref<Texture> Texture::Create(const TextureSpecification& textureSpec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(textureSpec);
			//case RendererAPIType::OpenGL: return Ref<OpenGLTexture>::Create(textureSpec);
			//case RendererAPIType::Vulkan: return Ref<VulkanTexture>::Create(textureSpec);
		}
		LK_CORE_ASSERT(false, "Coult not determine render API");
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer imageData)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(specification, imageData);
			//case RendererAPIType::Vulkan: return Ref<VulkanTexture2D>::Create(specification, imageData);
		}
		LK_CORE_ASSERT(false, "Coult not determine render API");
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(specification);
            //case RendererAPIType::Vulkan: return Ref<VulkanTexture>::Create(specification);
		}
		LK_CORE_ASSERT(false, "Coult not determine render API");
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification, std::vector<std::filesystem::path> facePaths)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: return Ref<OpenGLTextureCube>::Create(specification, facePaths);
			//case RendererAPIType::Vulkan: return Ref<VulkanTextureCube>::Create(specification, imageData);
		}
		LK_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


}
