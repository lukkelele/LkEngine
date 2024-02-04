#include "LKpch.h"
#include "OpenGLTextureArray.h"


namespace LkEngine {

	OpenGLTextureArray::OpenGLTextureArray(const TextureArraySpecification& specification)
		: m_Specification(specification)
	{
		auto [width, height]= GLUtils::ConvertDimensionsToWidthAndHeight(m_Specification.Dimension);
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_Width, m_Height, m_Specification.Layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GLUtils::ImageFormatToGLDataFormat(m_Specification.Format), m_Width, m_Height, m_Specification.Layers, 0, GLUtils::ImageFormatToGLDataFormat(m_Specification.Format), GL_UNSIGNED_BYTE, nullptr);
		GLUtils::GenerateTextureArrayImage(specification);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		//GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 10);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	OpenGLTextureArray::~OpenGLTextureArray()
	{
		// The TextureArray should not explicitly delete textures as they belong to TextureLibrary
		LK_CORE_WARN_TAG("OpenGLTextureArray", "Deleting TextureArrayTexture bound to {}", m_RendererID);
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureArray::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void OpenGLTextureArray::Unbind()
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	void OpenGLTextureArray::AddTextureToArray(Ref<Texture> texture)
	{
		glActiveTexture(GL_TEXTURE0 + m_Specification.TextureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		Buffer imageBuffer = texture->GetImageBuffer();
		LK_CORE_ASSERT(imageBuffer.Data, "Data is nullptr");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.Data);
		//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Textures.size(), m_Width, m_Height, 1, GLUtils::ImageFormatToGLDataFormat(texture->GetSpecification().Format), GL_UNSIGNED_BYTE, imageBuffer.Data);
		m_Textures.push_back(texture);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	bool OpenGLTextureArray::RemoveTextureFromArray(RendererID& rendererID)
	{
		for (auto& texture : m_Textures)
		{
			if (texture->GetRendererID() == rendererID)
			{
				// TODO: Pop reference to texture here
			}
		}
		return true;
	}

}
