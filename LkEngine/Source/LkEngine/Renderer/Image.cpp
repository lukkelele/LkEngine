#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Platform/OpenGL/OpenGLImage.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LImage> LImage::Create(const ImageSpecification& InImageSpecification, FBuffer InImageBuffer)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage>::Create(InImageSpecification, std::move(InImageBuffer));

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}

	TObjectPtr<LImage> LImage::Create(const ImageSpecification& InImageSpecification, void* InData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage>::Create(InImageSpecification, InData);

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}


	/*************************************************************************************
	 * LImage2D
	 ************************************************************************************/
	TObjectPtr<LImage2D> LImage2D::Create(const ImageSpecification& InImageSpecification, FBuffer InBuffer)
	{
		switch (LRendererAPI::Current())
		{
			//case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage2D>::Create(InImageSpecification, InBuffer);
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage2D>::Create(InImageSpecification, std::move(InBuffer));

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}

	TObjectPtr<LImage2D> LImage2D::Create(const ImageSpecification& InImageSpecification, void* InData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage2D>::Create(InImageSpecification, InData);

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}


}
