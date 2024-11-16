#pragma once

#include "LkEngine/Core/Core.h"

#include "UniformBuffer.h"
#include "Pipeline.h"


namespace LkEngine {

	enum class ERenderPassResourceType : uint16_t
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

	enum class ERenderPassInputType : uint16_t
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

	struct FRenderPassInput
	{
		ERenderPassResourceType Type = ERenderPassResourceType::None;
	};

	struct FRenderPassInputDeclaration
	{
		ERenderPassInputType Type = ERenderPassInputType::None;
		uint32_t Set = 0;
		uint32_t Binding = 0;
		uint32_t Count = 0;
		std::string Name{};
	};

	struct FRenderPassSpecification
	{
		TObjectPtr<LPipeline> Pipeline{};

		glm::vec4 MarkerColor;

		std::string DebugName{};
	};

	class LRenderPass : public LObject
	{
	public:
		virtual ~LRenderPass() = default;

		virtual FRenderPassSpecification& GetSpecification() = 0;
		virtual const FRenderPassSpecification& GetSpecification() const = 0;

		virtual TObjectPtr<LPipeline> GetPipeline() const = 0;
		virtual TObjectPtr<LFramebuffer> GetTargetFramebuffer() const = 0;

		virtual void SetInput(std::string_view Name, TObjectPtr<LImage> Image) = 0;
		virtual void SetInput(std::string_view Name, TObjectPtr<LTexture2D> Texture) = 0;
		virtual void SetInput(std::string_view Name, TObjectPtr<LUniformBuffer> UniformBuffer) = 0;

		virtual TObjectPtr<LImage> GetOutput(const uint32_t Index) = 0;
		virtual TObjectPtr<LImage> GetDepthOutput() = 0;
		virtual uint32_t GetFirstSetIndex() const =0;

		virtual bool Validate() = 0;
		virtual bool IsInvalidated(const uint32_t Set, const uint32_t Binding) const = 0;

		virtual void Bake() = 0;
		virtual bool Baked() const = 0;
		virtual void Prepare() = 0;

		virtual void Terminate() = 0;

		static TObjectPtr<LRenderPass> Create(const FRenderPassSpecification& Specification);

	private:
		LCLASS(LRenderPass)
	};

}
