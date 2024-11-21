#include "LKpch.h"
#include "OpenGLPipeline.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLPipeline::LOpenGLPipeline(const FPipelineSpecification& InPipelineSpecification)
		: PipelineSpecification(InPipelineSpecification)
	{
		LCLASS_REGISTER();

		PipelineSpecification.RenderAPI = ERendererAPI::OpenGL;
	}

	void LOpenGLPipeline::Invalidate()
	{
		/* TODO: */
	}

	void LOpenGLPipeline::SetLayout(const FVertexBufferLayout& InLayout)
	{
		PipelineSpecification.Layout = InLayout;
	}

	void LOpenGLPipeline::BindTextureArray(const LRendererID TextureArrayID)
	{
		BoundTextureArrays.emplace_back(TextureArrayID);
	}

}
