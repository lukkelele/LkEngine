#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/RenderPass.h"


namespace LkEngine {

    class OpenGLRenderPass : public RenderPass
    {
    public:
		OpenGLRenderPass(const RenderPassSpecification& spec);
		virtual ~OpenGLRenderPass();

		RenderPassSpecification& GetSpecification() override { return m_Specification; }
		const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

		Ref<Framebuffer> GetTargetFramebuffer() const;
		Ref<Pipeline> GetPipeline() const;

		void SetInput(std::string_view name, Ref<Image> image);
		void SetInput(std::string_view name, Ref<Texture2D> texture);
		void SetInput(std::string_view name, Ref<UniformBuffer> uniformBuffer);

		Ref<Image> GetOutput(uint32_t index);
		Ref<Image> GetDepthOutput();
		uint32_t GetFirstSetIndex() const;

		bool Validate() override;
		void Bake() override;
		bool Baked() const override;
		void Prepare() override;

		bool IsInvalidated(uint32_t set, uint32_t binding) const;

    private:
        RenderPassSpecification m_Specification;  
    };

}