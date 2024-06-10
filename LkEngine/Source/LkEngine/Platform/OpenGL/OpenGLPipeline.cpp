#include "LKpch.h"
#include "OpenGLPipeline.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLPipeline::OpenGLPipeline(const FPipelineSpecification& InPipelineSpecification)
		: PipelineSpecification(InPipelineSpecification)
	{
		PipelineSpecification.RenderAPI = ERendererAPI::OpenGL;
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::Invalidate()
	{
	}

	void OpenGLPipeline::SetLayout(const VertexBufferLayout& layout)
	{
		PipelineSpecification.Layout = layout;
	}

	void OpenGLPipeline::BindTextureArray(const RendererID& rendererID)
	{
		BoundTextureArrays.push_back(rendererID);
	}

}