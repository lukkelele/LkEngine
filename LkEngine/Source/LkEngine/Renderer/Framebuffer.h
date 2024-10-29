#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	class LFramebuffer;

	enum class EFramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		RED_INTEGER,

		DEPTH24STENCIL8, /* Depth/Stencil */

		Depth = DEPTH24STENCIL8
	};

	enum class EFramebufferBlendMode
	{
		None = 0,
		OneZero,
		SrcAlphaOneMinusSrcAlpha,
		Additive,
		Zero_SrcColor
	};

	enum class EAttachmentLoadOp
	{
		Inherit = 0, 
		Clear   = 1, 
		Load    = 2
	};

	struct FFramebufferTextureSpecification
	{
		FFramebufferTextureSpecification() = default;
		FFramebufferTextureSpecification(EImageFormat InImageFormat) 
			: ImageFormat(InImageFormat) {}

		EImageFormat ImageFormat{};
		bool bBlend = true;
		EFramebufferBlendMode BlendMode = EFramebufferBlendMode::SrcAlphaOneMinusSrcAlpha;
		EAttachmentLoadOp LoadOp = EAttachmentLoadOp::Inherit;
	};

	struct FFramebufferAttachmentSpecification
	{
		FFramebufferAttachmentSpecification() = default;
		FFramebufferAttachmentSpecification(const std::initializer_list<FFramebufferTextureSpecification>& InAttachments)
			: Attachments(InAttachments) {}

		std::vector<FFramebufferTextureSpecification> Attachments{};
	};

	struct FFramebufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width = 0;
		uint32_t Height = 0;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 0.0f;
		bool ClearColorOnLoad = true;
		bool ClearDepthOnLoad = true;

		uint32_t Samples = 1;

		FFramebufferAttachmentSpecification Attachments{};
		EFramebufferBlendMode BlendMode = EFramebufferBlendMode::None;

		bool SwapChainTarget = false; // SwapChainTarget
		bool Transfer = false; // Transfer operation flag
		bool Blend = true;

		TObjectPtr<LFramebuffer> ExistingFramebuffer;

		// Note: these are used to attach multi-layered color/depth images 
		TObjectPtr<LImage> ExistingImage;
		std::vector<uint32_t> ExistingImageLayers;
		
		std::map<uint32_t, TObjectPtr<LImage>> ExistingImages;

		std::string DebugName;

		FFramebufferSpecification() = default;
	};

	using FResizeCallback = std::function<void(TObjectPtr<LFramebuffer>)>;

	class LFramebuffer : public LObject
	{
	public:
		virtual ~LFramebuffer() = default;

		virtual void Invalidate() = 0;
		virtual void Resize(const uint32_t NewWidth, const uint32_t NewHeight, const bool bForceRecreate = false) = 0;
		virtual void AddResizeCallback(const FResizeCallback& Func) = 0;

		virtual TObjectPtr<LImage> GetImage(const uint32_t AttachmentIndex = 0) const = 0;
		virtual TObjectPtr<LImage> GetDepthImage() const = 0;
		virtual size_t GetColorAttachmentCount() const = 0;
		virtual bool HasDepthAttachment() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void BindTexture(const uint32_t AttachmentIndex = 0, const uint32_t TextureSlot = 0) const = 0;

		virtual void Clear() = 0;
		virtual int ReadPixel(const uint32_t AttachmentIndex, int PosX, int PosY) = 0;
		virtual void ClearAttachment(const uint32_t AttachmentIndex, int value) = 0;
		virtual LRendererID GetColorAttachmentRendererID(const uint32_t Index = 0) const = 0;

		virtual LRendererID GetRendererID() const = 0;
		virtual LRendererID& GetRendererID() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint64_t GetSize() const = 0;
		virtual const FFramebufferSpecification& GetSpecification() const = 0;

		static TObjectPtr<LFramebuffer> Create(const FFramebufferSpecification& InSpecification);

		static void TargetSwapChain();

	private:
		LCLASS(LFramebuffer)
	};


}