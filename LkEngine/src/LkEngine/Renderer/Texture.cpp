#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#ifdef LK_RENDERER_API_VULKAN
	#include "LkEngine/Renderer/Vulkan/VulkanTexture.h"
#elif defined(LK_RENDERER_API_OPENGL)
	#include "LkEngine/Renderer/OpenGL/OpenGLTexture.h"
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

	Buffer Texture::GetWriteableBuffer()
	{
		LK_ASSERT(m_Locked);
		return m_ImageData;
	}




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

	s_ptr<Texture2D> Texture2D::Create(const std::string& filePath)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanTexture>(filePath);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLTexture2D>(filePath);
	#endif
	}


#if 0

	Texture::Texture(const std::string& path)
		: m_RendererID(0)
		, m_FilePath(path)
	{
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		GL_CALL(glGenTextures(1, &m_RendererID));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // S: x
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // T: y

		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

		stbi_image_free(data);
	}

	Texture::Texture(const TextureSpecification& textureSpec)
		: m_RendererID(0)
	{
		m_InternalFormat = ImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = ImageFormatToGLDataFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture::~Texture()
	{
		GL_CALL(glDeleteTextures(1, &m_RendererID));
	}

	void Texture::Bind(unsigned int slot /*= 0*/) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	}

	void Texture::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void Texture::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	Buffer Texture::GetWriteableBuffer()
	{
		LK_ASSERT(m_Locked);
		return m_ImageData;
	}

	void Texture::Lock()
	{
		m_Locked = true;
	}

	void Texture::Unlock()
	{
		m_Locked = false;
		// Submit to renderer
	}

	s_ptr<Texture2D> Texture2D::Create(const TextureSpecification& textureSpec)
	{
		return std::make_shared<Texture2D>(textureSpec);
	}

	s_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		return std::make_shared<Texture2D>(path);
	}

#endif

}
