#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Platform/OpenGL/OpenGLImage.h"

#include "LkEngine/Renderer/RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LImage> LImage::Create(const FImageSpecification& ImageSpecification, FBuffer InImageBuffer)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage>::Create(ImageSpecification, std::move(InImageBuffer));

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}

	TObjectPtr<LImage> LImage::Create(const FImageSpecification& ImageSpecification, void* InData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage>::Create(ImageSpecification, InData);

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}


	/*************************************************************************************
	 * LImage2D
	 ************************************************************************************/
	TObjectPtr<LImage2D> LImage2D::Create(const FImageSpecification& ImageSpecification, FBuffer InBuffer)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage2D>::Create(ImageSpecification, std::move(InBuffer));

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}

	TObjectPtr<LImage2D> LImage2D::Create(const FImageSpecification& ImageSpecification, void* InData)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLImage2D>::Create(ImageSpecification, InData);

			default: break;
		}

		LK_CORE_ASSERT(false, "No RenderAPI detected");
		return nullptr;
	}


}
