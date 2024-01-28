#include "LKpch.h"
#include "OpenGLRenderPass.h"

#include "LkOpenGL.h"
#include "OpenGLPipeline.h"


namespace LkEngine {

	OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& specification)
		: m_Specification(specification)
	{
		LK_CORE_VERIFY(m_Specification.Pipeline != nullptr, "OpenGLRenderPass::m_Specification.Pipeline is nullptr!");
	}

	OpenGLRenderPass::~OpenGLRenderPass()
	{
	}


	void OpenGLRenderPass::SetInput(std::string_view name, Ref<Image> image)
	{

	}

	void OpenGLRenderPass::SetInput(std::string_view name, Ref<Texture2D> texture)
	{

	}

	void OpenGLRenderPass::SetInput(std::string_view name, Ref<UniformBuffer> uniformBuffer)
	{
	}


	Ref<Image> OpenGLRenderPass::GetOutput(uint32_t index)
	{
		return nullptr;
	}

	Ref<Image> OpenGLRenderPass::GetDepthOutput()
	{
		return nullptr;
	}

	uint32_t OpenGLRenderPass::GetFirstSetIndex() const
	{
		return 1;
	}
	
	bool OpenGLRenderPass::Validate() 
	{
		return true;
	}

	void OpenGLRenderPass::Bake() 
	{
	}

	bool OpenGLRenderPass::Baked() const 
	{
		return true;
	}

	void OpenGLRenderPass::Prepare() 
	{
	}

	bool OpenGLRenderPass::IsInvalidated(uint32_t set, uint32_t binding) const
	{
		return false;
	}

	Ref<Pipeline> OpenGLRenderPass::GetPipeline() const
	{
		return m_Specification.Pipeline;
	}

	Ref<Framebuffer> OpenGLRenderPass::GetTargetFramebuffer() const
	{
		return nullptr;
	}



}