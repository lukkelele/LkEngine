#include "LKpch.h"
#include "OpenGLRenderPass.h"

#include "LkOpenGL.h"
#include "OpenGLPipeline.h"


namespace LkEngine {

	LOpenGLRenderPass::LOpenGLRenderPass(const FRenderPassSpecification& InSpecification)
		: Specification(InSpecification)
	{
		LOBJECT_REGISTER();
	}

	void LOpenGLRenderPass::SetInput(std::string_view Name, TObjectPtr<LImage> Image)
	{
	}

	void LOpenGLRenderPass::SetInput(std::string_view Name, TObjectPtr<LTexture2D> Texture)
	{
	}

	void LOpenGLRenderPass::SetInput(std::string_view Name, TObjectPtr<LUniformBuffer> UniformBuffer)
	{
	}

	TObjectPtr<LImage> LOpenGLRenderPass::GetOutput(const uint32_t Index)
	{
		return nullptr;
	}

	TObjectPtr<LImage> LOpenGLRenderPass::GetDepthOutput()
	{
		return nullptr;
	}

	uint32_t LOpenGLRenderPass::GetFirstSetIndex() const
	{
		return 1;
	}
	
	bool LOpenGLRenderPass::Validate() 
	{
		return true;
	}

	void LOpenGLRenderPass::Bake() 
	{
	}

	bool LOpenGLRenderPass::Baked() const 
	{
		return true;
	}

	void LOpenGLRenderPass::Prepare() 
	{
	}

	bool LOpenGLRenderPass::IsInvalidated(const uint32_t Set, const uint32_t Binding) const
	{
		return false;
	}

	TObjectPtr<LPipeline> LOpenGLRenderPass::GetPipeline() const
	{
		return Specification.Pipeline;
	}

	TObjectPtr<LFramebuffer> LOpenGLRenderPass::GetTargetFramebuffer() const
	{
		return nullptr;
	}

	void LOpenGLRenderPass::Terminate()
	{
	}

}
