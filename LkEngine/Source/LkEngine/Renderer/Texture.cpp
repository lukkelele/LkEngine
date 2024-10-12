#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Platform/OpenGL/OpenGLTexture.h"


namespace LkEngine {

	TObjectPtr<LTexture> LTexture::Create(const TextureSpecification& textureSpec)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(textureSpec);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const TextureSpecification& specification, FBuffer BufferData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(specification, BufferData);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const TextureSpecification& specification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(specification);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTextureCube> LTextureCube::Create(const TextureSpecification& specification, 
												  const std::vector<std::filesystem::path>& facePaths)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTextureCube>::Create(specification, facePaths);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}


}
