#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	struct FramebufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width = 0;
		uint32_t Height = 0;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 0.0f;
		bool ClearColorOnLoad = true;
		bool ClearDepthOnLoad = true;

		FramebufferSpecification() = default;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		static s_ptr<Framebuffer> Create(const FramebufferSpecification& framebufferSpecification);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;
		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual s_ptr<Image> GetImage(uint32_t attachmentIndex = 0) const = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
	};


}