#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLTexture.h"


namespace LkEngine {

	TObjectPtr<LTexture> LTexture::Create(const FTextureSpecification& textureSpec)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(textureSpec);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const FTextureSpecification& Specification, FBuffer BufferData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(Specification, BufferData);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const FTextureSpecification& Specification)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTexture2D>::Create(Specification);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTextureCube> LTextureCube::Create(const FTextureSpecification& specification, 
												  const std::vector<std::filesystem::path>& InFacePaths)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<OpenGLTextureCube>::Create(specification, InFacePaths);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}


}
