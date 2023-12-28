#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#ifdef LK_RENDERER_API_VULKAN
	#include "LkEngine/Platform/Vulkan/VulkanTexture.h"
#elif defined(LK_RENDERER_API_OPENGL)
	#include "LkEngine/Platform/OpenGL/OpenGLTexture.h"
#endif

namespace LkEngine {

	s_ptr<Texture> Texture::Create(const TextureSpecification& textureSpec)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(textureSpec);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture>(textureSpec);
	#endif
	}

	s_ptr<Texture> Texture::Create(const std::string& filePath)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(filePath);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture>(filePath);
	#endif
	}

#if 0
	Buffer Texture::GetWriteableBuffer()
	{
		LK_ASSERT(m_Locked);
		return m_Image->GetBuffer();// m_ImageData;
	}
#endif


	//================================================================================
	// Texture 2D
	//================================================================================

	s_ptr<Texture2D> Texture2D::Create(const TextureSpecification& textureSpec)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(textureSpec);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture2D>(textureSpec);
	#endif
	}

	s_ptr<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer imageData)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(specification);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture2D>(specification, imageData);
	#endif
	}

	s_ptr<Texture2D> Texture2D::Create(const std::string& filePath)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(filePath);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture2D>(filePath);
	#endif
	}

}
