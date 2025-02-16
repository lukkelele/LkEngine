#include "LKpch.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLTexture.h"


namespace LkEngine {

	TObjectPtr<LTexture> LTexture::Create(const FTextureSpecification& Specification)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLTexture2D>::Create(Specification);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: 
				break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const FTextureSpecification& Specification, FBuffer BufferData)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLTexture2D>::Create(Specification, BufferData);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: 
				break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTexture2D> LTexture2D::Create(const FTextureSpecification& Specification)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLTexture2D>::Create(Specification);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: 
				break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LTextureCube> LTextureCube::Create(const FTextureSpecification& Specification, 
												  const std::vector<std::filesystem::path>& InFacePaths)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: 
			{
				return TObjectPtr<LOpenGLTextureCube>::Create(Specification, InFacePaths);
			}

			case ERendererAPI::Vulkan:
			case ERendererAPI::None: 
				break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}


}
