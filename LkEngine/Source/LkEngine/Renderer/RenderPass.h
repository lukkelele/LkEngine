#pragma once

#include "LkEngine/Core/Core.h"

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
		TObjectPtr<LPipeline> Pipeline{};

		glm::vec4 MarkerColor;

		std::string DebugName;
	};

	class LRenderPass : public LObject
	{
	public:
		virtual ~LRenderPass() = default;

		virtual RenderPassSpecification& GetSpecification() = 0;
		virtual const RenderPassSpecification& GetSpecification() const = 0;

		virtual TObjectPtr<LPipeline> GetPipeline() const = 0;
		virtual TObjectPtr<LFramebuffer> GetTargetFramebuffer() const = 0;

		virtual void SetInput(std::string_view name, TObjectPtr<LImage> image) = 0;
		virtual void SetInput(std::string_view name, TObjectPtr<LTexture2D> texture) = 0;
		virtual void SetInput(std::string_view name, TObjectPtr<LUniformBuffer> uniformBuffer) = 0;

		virtual TObjectPtr<LImage> GetOutput(uint32_t index) = 0;
		virtual TObjectPtr<LImage> GetDepthOutput() = 0;
		virtual uint32_t GetFirstSetIndex() const =0;

		virtual bool Validate() = 0;
		virtual void Bake() = 0;
		virtual bool Baked() const = 0;
		virtual void Prepare() = 0;

		virtual void Terminate() = 0;

		static TObjectPtr<LRenderPass> Create(const RenderPassSpecification& spec);

	private:
		LCLASS(LRenderPass)
	};


}