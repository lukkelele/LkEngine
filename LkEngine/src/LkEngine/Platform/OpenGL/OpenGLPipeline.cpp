#include "LKpch.h"
#include "OpenGLPipeline.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& specification)
		: m_Specification(specification)
	{
		m_Specification.RenderAPI = RendererAPIType::OpenGL;
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::Invalidate()
	{
	}

	void OpenGLPipeline::SetLayout(const VertexBufferLayout& layout)
	{
		m_Specification.Layout = layout;
	}

}