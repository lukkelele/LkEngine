#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Platform/OpenGL/OpenGLTexture.h"
//#include "LkEngine/Platform/Vulkan/VulkanTexture.h"


namespace LkEngine {

	Ref<Texture> Texture::Create(const TextureSpecification& textureSpec)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLTexture2D>::Create(textureSpec);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanTexture2D>::Create(specification, imageData);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer imageData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLTexture2D>::Create(specification, imageData);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanTexture2D>::Create(specification, imageData);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLTexture2D>::Create(specification);
			}

			/// @FIXME
            //case RendererAPIType::Vulkan: return Ref<VulkanTexture>::Create(specification);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification, std::vector<std::filesystem::path> facePaths)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return Ref<OpenGLTextureCube>::Create(specification, facePaths);
			}

			/// @FIXME
			//case RendererAPIType::Vulkan: return Ref<VulkanTextureCube>::Create(specification, imageData);

			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}


}
