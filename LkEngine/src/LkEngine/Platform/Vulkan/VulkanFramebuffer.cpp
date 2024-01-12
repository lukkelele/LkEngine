#include "LKpch.h"
#include "VulkanFramebuffer.h"


namespace LkEngine {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& framebufferSpecification)
		: m_Specification(framebufferSpecification)
		, m_Width(framebufferSpecification.Width)
		, m_Height(framebufferSpecification.Height)
		, m_ClearColor(framebufferSpecification.ClearColor)
	{
		TextureSpecification textureSpecification;
		textureSpecification.Width = framebufferSpecification.Width;
		textureSpecification.Height = framebufferSpecification.Height;

		LK_ASSERT(m_Width != 0 && m_Height != 0);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Bind() const
	{
	}

	void VulkanFramebuffer::Unbind() const
	{
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
	}

	void VulkanFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
	}

	

}