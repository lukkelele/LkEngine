#include "LKpch.h"
#include "OpenGLTextureArray.h"


namespace LkEngine {

	LOpenGLArrayTexture::LOpenGLArrayTexture(const FArrayTextureSpecification& InSpecification)
		: Specification(InSpecification)
	{
		LCLASS_REGISTER();

		auto [Width, Height] = LOpenGL::ConvertDimensionsToWidthAndHeight(Specification.TextureArrayDimension);
		m_Width = Width;
		m_Height = Height;

		LK_OpenGL_Verify(glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID));
		LK_OpenGL_Verify(glTextureStorage3D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height, Specification.Layers));

		// Since we're using immutable storage, mipmap generation should be reconsidered.
		// If mipmaps are needed, you must specify the correct number of levels in glTextureStorage3D and generate them after filling the Texture.
		if (Specification.Mipmaps > 1)
		{
			LK_OpenGL_Verify(glGenerateTextureMipmap(m_RendererID));
			LK_OpenGL_Verify(glTextureParameteri(m_RendererID, GL_TEXTURE_MAX_LEVEL, 10)); 
		}
		
		LK_OpenGL_Verify(glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); 
		LK_OpenGL_Verify(glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		LK_OpenGL_Verify(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
		LK_OpenGL_Verify(glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	LOpenGLArrayTexture::~LOpenGLArrayTexture()
	{
		/* The TextureArray should not explicitly delete textures as 
		 * they belong to the TextureLibrary. */
		LK_CORE_DEBUG_TAG("OpenGLTextureArray", "Deleting TextureArrayTexture bound to {}", m_RendererID);
		LK_OpenGL_Verify(glDeleteTextures(1, &m_RendererID));
	}

	void LOpenGLArrayTexture::Bind()
	{
		if (m_RendererID > 0)
		{
			LK_OpenGL_Verify(glBindTextureUnit(Specification.TextureSlot, m_RendererID));
		}
	}

	void LOpenGLArrayTexture::Unbind()
	{
		LK_OpenGL_Verify(glBindTextureUnit(Specification.TextureSlot, 0));
		if (m_RendererID > 0)
		{
			//LK_OpenGL(glBindTextureUnit(Specification.TextureSlot, 0));
		}
	}

	void LOpenGLArrayTexture::AddTextureToArray(const TObjectPtr<LTexture> Texture)
	{
		if (Texture->GetType() == ETextureType::Texture2D)
		{
			TObjectPtr<LTexture2D> Texture2D = Texture.As<LTexture2D>();
			FBuffer ImageBuffer = Texture2D->GetImageBuffer();
			LK_CORE_ASSERT(ImageBuffer.Data, "Texture data from \"{}\" is NULL", Texture2D->GetName());

			LK_OpenGL_Verify(glTextureSubImage3D(m_RendererID, 
					                      0, 
					                      0, 
					                      0, 
					                      static_cast<int>(Textures.size()), 
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

	bool LOpenGLArrayTexture::RemoveTextureFromArray(const LRendererID TextureID)
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
