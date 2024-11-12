#include "LKpch.h"
#include "OpenGLPipeline.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLPipeline::LOpenGLPipeline(const FPipelineSpecification& InPipelineSpecification)
		: PipelineSpecification(InPipelineSpecification)
	{
		PipelineSpecification.RenderAPI = ERendererAPI::OpenGL;
	}

	void LOpenGLPipeline::Invalidate()
	{
	}

	void LOpenGLPipeline::SetLayout(const VertexBufferLayout& layout)
	{
		PipelineSpecification.Layout = layout;
	}

	void LOpenGLPipeline::BindTextureArray(const LRendererID TextureArrayID)
	{
		BoundTextureArrays.push_back(TextureArrayID);
	}

}