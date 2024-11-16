#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

    /**
     * LOpenGLRenderPass
	 * 
	 * @note: This is not implemented yet.
     */
    class LOpenGLRenderPass : public LRenderPass
    {
    public:
		LOpenGLRenderPass(const FRenderPassSpecification& InSpecification);
		LOpenGLRenderPass() = delete;
		~LOpenGLRenderPass() = default;

		FORCEINLINE virtual FRenderPassSpecification& GetSpecification() override 
		{ 
			return Specification; 
		}

		FORCEINLINE virtual const FRenderPassSpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

		virtual TObjectPtr<LFramebuffer> GetTargetFramebuffer() const override;
		virtual TObjectPtr<LPipeline> GetPipeline() const override;

		virtual void SetInput(std::string_view Name, TObjectPtr<LImage> Image) override;
		virtual void SetInput(std::string_view Name, TObjectPtr<LTexture2D> Texture) override;
		virtual void SetInput(std::string_view Name, TObjectPtr<LUniformBuffer> UniformBuffer) override;

		virtual TObjectPtr<LImage> GetOutput(const uint32_t Index) override;
		virtual TObjectPtr<LImage> GetDepthOutput() override;
		virtual uint32_t GetFirstSetIndex() const override;

		virtual bool Validate() override;
		virtual void Bake() override;
		virtual bool Baked() const override;
		virtual void Prepare() override;
		virtual void Terminate() override;

		virtual bool IsInvalidated(const uint32_t Set, const uint32_t Binding) const override;

    private:
		FRenderPassSpecification Specification{};

		LCLASS(LOpenGLRenderPass)
    };

}
