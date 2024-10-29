#include "LKpch.h"
#include "OpenGLRenderPass.h"

#include "LkOpenGL.h"
#include "OpenGLPipeline.h"


namespace LkEngine {

	OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		LCLASS_REGISTER();
	}

	void OpenGLRenderPass::SetInput(std::string_view name, TObjectPtr<LImage> image)
	{
	}

	void OpenGLRenderPass::SetInput(std::string_view name, TObjectPtr<LTexture2D> texture)
	{
	}

	void OpenGLRenderPass::SetInput(std::string_view name, TObjectPtr<LUniformBuffer> uniformBuffer)
	{
	}

	TObjectPtr<LImage> OpenGLRenderPass::GetOutput(uint32_t index)
	{
		return nullptr;
	}

	TObjectPtr<LImage> OpenGLRenderPass::GetDepthOutput()
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

	TObjectPtr<LPipeline> OpenGLRenderPass::GetPipeline() const
	{
		return m_Specification.Pipeline;
	}

	TObjectPtr<LFramebuffer> OpenGLRenderPass::GetTargetFramebuffer() const
	{
		return nullptr;
	}

	void OpenGLRenderPass::Terminate()
	{
	}



}