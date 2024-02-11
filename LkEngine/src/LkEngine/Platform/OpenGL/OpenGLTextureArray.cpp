#include "LKpch.h"
#include "OpenGLTextureArray.h"


namespace LkEngine {

	OpenGLTextureArray::OpenGLTextureArray(const TextureArraySpecification& specification)
		: m_Specification(specification)
	{
		auto [width, height]= GLUtils::ConvertDimensionsToWidthAndHeight(m_Specification.Dimension);
		m_Width = width;
		m_Height = height;

#if 0
		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_Width, m_Height, 10 /* layers */, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		//GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 10);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
#endif

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID);
		glTextureStorage3D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height, m_Specification.Layers);

		// Since we're using immutable storage, mipmap generation should be reconsidered.
		// If mipmaps are needed, you must specify the correct number of levels in glTextureStorage3D and generate them after filling the texture.
		// glGenerateTextureMipmap is the DSA function for mipmap generation but decide if mipmaps are necessary for your texture array.
		glGenerateTextureMipmap(m_RendererID);
		
		// Set texture parameters using DSA functions
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Adjust if mipmaps are used
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAX_LEVEL, 10); // Only necessary if using mipmaps
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTextureArray::~OpenGLTextureArray()
	{
		// The TextureArray should not explicitly delete textures as they belong to TextureLibrary
		LK_CORE_WARN_TAG("OpenGLTextureArray", "Deleting TextureArrayTexture bound to {}", m_RendererID);
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureArray::Bind()
	{
		//glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		//glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
		glBindTextureUnit(m_Specification.TextureSlot, m_RendererID);
	}

	void OpenGLTextureArray::Unbind()
	{
		//glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		//glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glBindTextureUnit(m_Specification.TextureSlot, 0);
	}

	void OpenGLTextureArray::AddTextureToArray(Ref<Texture> texture)
	{
		//Buffer imageBuffer = texture.As<Texture2D>()->GetImageBuffer();
		Buffer imageBuffer = texture->GetImageBuffer();
		LK_CORE_ASSERT(imageBuffer.Data, "Texture data from \"{}\" is NULL", texture->GetName());
		glTextureSubImage3D(m_RendererID, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.Data);
		m_Textures.push_back(texture);

#if 0
		if (texture->GetType() == TextureType::Texture2D)
		{
			Ref<Texture2D> texture2D = texture.As<Texture2D>();
			Buffer imageBuffer = texture2D->GetImageBuffer();
			LK_CORE_ASSERT(imageBuffer.Data, "Texture data from \"{}\" is NULL", texture2D->GetName());
			glTextureSubImage3D(m_RendererID, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.Data);
			m_Textures.push_back(texture2D);
		}
#endif
	}

	bool OpenGLTextureArray::RemoveTextureFromArray(RendererID& rendererID)
	{
		for (auto& texture : m_Textures)
		{
			if (texture->GetRendererID() == rendererID)
			{
				// TODO: Pop reference texture here
			}
		}
		return true;
	}

}
