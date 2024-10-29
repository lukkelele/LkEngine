#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

	///------------------------------------------------
	/// @FIXME: Not implemented.
	///------------------------------------------------

    /**
     * LOpenGLRenderPass
     */
    class OpenGLRenderPass : public LRenderPass
    {
    public:
		OpenGLRenderPass(const RenderPassSpecification& InSpecification);
		OpenGLRenderPass() = delete;
		~OpenGLRenderPass() = default;

		FORCEINLINE virtual RenderPassSpecification& GetSpecification() override 
		{ 
			return m_Specification; 
		}

		FORCEINLINE virtual const RenderPassSpecification& GetSpecification() const override 
		{ 
			return m_Specification; 
		}

		TObjectPtr<LFramebuffer> GetTargetFramebuffer() const;
		TObjectPtr<LPipeline> GetPipeline() const;

		void SetInput(std::string_view name, TObjectPtr<LImage> image);
		void SetInput(std::string_view name, TObjectPtr<LTexture2D> texture);
		void SetInput(std::string_view name, TObjectPtr<LUniformBuffer> uniformBuffer);

		TObjectPtr<LImage> GetOutput(uint32_t index);
		TObjectPtr<LImage> GetDepthOutput();
		uint32_t GetFirstSetIndex() const;

		virtual bool Validate() override;
		virtual void Bake() override;
		virtual bool Baked() const override;
		virtual void Prepare() override;
		virtual void Terminate() override;

		bool IsInvalidated(uint32_t Set, uint32_t Binding) const;

    private:
		RenderPassSpecification m_Specification{};

		LCLASS(OpenGLRenderPass)
    };

}