#include "LKpch.h"
#include "OpenGLTextureArray.h"


namespace LkEngine {

	OpenGLTextureArray::OpenGLTextureArray(const FTextureArraySpecification& InSpecification)
		: Specification(InSpecification)
	{
		auto [Width, Height] = GLUtils::ConvertDimensionsToWidthAndHeight(Specification.TextureArrayDimension);
		m_Width = Width;
		m_Height = Height;

		LK_OpenGL(glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID));
		LK_OpenGL(glTextureStorage3D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height, Specification.Layers));

		// Since we're using immutable storage, mipmap generation should be reconsidered.
		// If mipmaps are needed, you must specify the correct number of levels in glTextureStorage3D and generate them after filling the Texture.
		if (Specification.Mipmaps > 1)
		{
			LK_OpenGL(glGenerateTextureMipmap(m_RendererID));
			LK_OpenGL(glTextureParameteri(m_RendererID, GL_TEXTURE_MAX_LEVEL, 10)); 
		}
		
		LK_OpenGL(glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); 
		LK_OpenGL(glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		LK_OpenGL(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
		LK_OpenGL(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	OpenGLTextureArray::~OpenGLTextureArray()
	{
		// The TextureArray should not explicitly delete Textures as they belong to TextureLibrary
		LK_CORE_WARN_TAG("OpenGLTextureArray", "Deleting TextureArrayTexture bound to {}", m_RendererID);
		LK_OpenGL(glDeleteTextures(1, &m_RendererID));
	}

	void OpenGLTextureArray::Bind()
	{
		if (m_RendererID > 0)
		{
			LK_OpenGL(glBindTextureUnit(Specification.TextureSlot, m_RendererID));
		}
	}

	void OpenGLTextureArray::Unbind()
	{
		LK_OpenGL(glBindTextureUnit(Specification.TextureSlot, 0));
		if (m_RendererID > 0)
		{
			//LK_OpenGL(glBindTextureUnit(Specification.TextureSlot, 0));
		}
	}

	void OpenGLTextureArray::AddTextureToArray(const TObjectPtr<LTexture> Texture)
	{
		if (Texture->GetType() == ETextureType::Texture2D)
		{
			TObjectPtr<LTexture2D> Texture2D = Texture.As<LTexture2D>();
			FBuffer ImageBuffer = Texture2D->GetImageBuffer();
			LK_CORE_ASSERT(ImageBuffer.Data, "Texture data from \"{}\" is NULL", Texture2D->GetName());

			LK_OpenGL(glTextureSubImage3D(m_RendererID, 
					                      0, 
					                      0, 
					                      0, 
					                      Textures.size(), 
					                      m_Width, 
					                      m_Height, 
					                      1, 
					                      GL_RGBA, 
					                      GL_UNSIGNED_BYTE, 
					                      ImageBuffer.Data));

			/* Add texture to array and populate caches. */
			Textures.push_back(Texture2D);
			IndexCache[Texture2D->GetRendererID()] = static_cast<int>(Textures.size()) - 1;
			TextureIDSet.insert(Texture2D->GetRendererID());
		}
		else
		{
			LK_CORE_WARN_TAG("OpenGLTextureArray", "Cannot add texture \"{}\" to array", Texture->ClassName());
		}
	}

	bool OpenGLTextureArray::RemoveTextureFromArray(const RendererID TextureID)
	{
		auto FindTextureInArray = [TextureID](const TObjectPtr<LTexture>& Texture) -> bool
		{
			return (Texture->GetRendererID() == TextureID);
		};

		auto Iter = std::remove_if(Textures.begin(), Textures.end(), FindTextureInArray);
		if (Iter == Textures.end())
		{
			return false;
		}

		/* Remove cached texture index. */
		LK_CORE_ASSERT(IndexCache.contains(TextureID));
		const std::size_t Erased = IndexCache.erase(TextureID);
		LK_CORE_TRACE_TAG("OpenGLTextureArray", "Removed {} indices from the texture index cache", Erased);
		TextureIDSet.erase(TextureID);

		return true;
	}

}
