#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	class Framebuffer;

	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	enum class FramebufferBlendMode
	{
		None = 0,
		OneZero,
		SrcAlphaOneMinusSrcAlpha,
		Additive,
		Zero_SrcColor
	};

	enum class AttachmentLoadOp
	{
		Inherit = 0, Clear = 1, Load = 2
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format) : Format(format) {}

		ImageFormat Format;
		bool Blend = true;
		FramebufferBlendMode BlendMode = FramebufferBlendMode::SrcAlphaOneMinusSrcAlpha;
		AttachmentLoadOp LoadOp = AttachmentLoadOp::Inherit;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width = 0;
		uint32_t Height = 0;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 0.0f;
		bool ClearColorOnLoad = true;
		bool ClearDepthOnLoad = true;

		uint32_t Samples = 1;

		FramebufferAttachmentSpecification Attachments;
		FramebufferBlendMode BlendMode = FramebufferBlendMode::None;

		bool SwapChainTarget = false; // SwapChainTarget
		bool Transfer = false; // Transfer operation flag
		bool Blend = true;

		Ref<Framebuffer> ExistingFramebuffer;

		// Note: these are used to attach multi-layered color/depth images 
		Ref<Image> ExistingImage;
		std::vector<uint32_t> ExistingImageLayers;
		
		std::map<uint32_t, Ref<Image>> ExistingImages;

		std::string DebugName;

		FramebufferSpecification() = default;
	};

	class Framebuffer : public RefCounted
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;
		virtual void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) = 0;

		virtual Ref<Image> GetImage(uint32_t attachmentIndex = 0) const = 0;
		virtual Ref<Image> GetDepthImage() const = 0;
		virtual size_t GetColorAttachmentCount() const = 0;
		virtual bool HasDepthAttachment() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const = 0;

		virtual void Clear() = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual RendererID GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& framebufferSpecification);

		static void TargetSwapChain(); /// @FIXME: REMOVE THIS 
	};


}