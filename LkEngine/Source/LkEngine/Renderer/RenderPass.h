#pragma once

#include "LkEngine/Core/Base.h"

#include "UniformBuffer.h"
#include "Pipeline.h"


namespace LkEngine {

	/// @FIXME: Add E prefix
	enum class RenderPassResourceType : uint16_t
	{
		None = 0,
		UniformBuffer,
		UniformBufferSet,
		StorageBuffer,
		StorageBufferSet,
		Texture2D,
		TextureCube,
		Image2D
	};

	/// @FIXME: Add E prefix
	enum class RenderPassInputType : uint16_t
	{
		None = 0,
		UniformBuffer,
		StorageBuffer,
		ImageSampler1D,
		ImageSampler2D,
		ImageSampler3D, 
		StorageImage1D,
		StorageImage2D,
		StorageImage3D
	};

	struct RenderPassInput
	{
		RenderPassResourceType Type = RenderPassResourceType::None;
	};

	struct RenderPassInputDeclaration
	{
		RenderPassInputType Type = RenderPassInputType::None;
		uint32_t Set = 0;
		uint32_t Binding = 0;
		uint32_t Count = 0;
		std::string Name;
	};

	struct RenderPassSpecification
	{
		Ref<Pipeline> Pipeline;
		glm::vec4 MarkerColor;

		std::string DebugName;
	};

	class RenderPass : public RefCounted
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassSpecification& GetSpecification() = 0;
		virtual const RenderPassSpecification& GetSpecification() const = 0;

		virtual Ref<Pipeline> GetPipeline() const = 0;
		virtual Ref<Framebuffer> GetTargetFramebuffer() const = 0;

		virtual void SetInput(std::string_view name, Ref<Image> image) = 0;
		virtual void SetInput(std::string_view name, Ref<Texture2D> texture) = 0;
		virtual void SetInput(std::string_view name, Ref<UniformBuffer> uniformBuffer) = 0;

		virtual Ref<Image> GetOutput(uint32_t index) = 0;
		virtual Ref<Image> GetDepthOutput() = 0;
		virtual uint32_t GetFirstSetIndex() const =0;

		virtual bool Validate() = 0;
		virtual void Bake() = 0;
		virtual bool Baked() const = 0;
		virtual void Prepare() = 0;

		virtual void Terminate() = 0;

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	};


}