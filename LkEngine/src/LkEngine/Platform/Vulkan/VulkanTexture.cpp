#include "LKpch.h"
#include "VulkanTexture.h"

#include "LkVulkan.h"

#include "LkEngine/IO/File.h"


namespace LkEngine {

	VulkanTexture::VulkanTexture(const TextureSpecification& specification)
	{
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		if (specification.Path != "")
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);

			m_Image = Image::Create(imageSpec, data);
			stbi_image_free(data);
		}
		else
		{
			m_Image = Image::Create(imageSpec, nullptr);
		}
	}

	VulkanTexture::VulkanTexture(const TextureSpecification& specification, Buffer imageData)
	{
		ImageSpecification imageSpec;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		m_Image = Image::Create(imageSpec, imageData);
#if 0
		if (specification.Path != "")
		{
			stbi_set_flip_vertically_on_load(1);
			int width, height, channels;
			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);

			m_Image = Image::Create(imageSpec, data);
			stbi_image_free(data);
		}
		else
		{
			m_Image = Image::Create(imageSpec, nullptr);
		}
#endif
	}


	VulkanTexture::VulkanTexture(const std::string& filePath)
	{
		LOG_TRACE("VulkanTexture created: {}", filePath);
		m_FilePath = filePath;
		m_Name = File::ExtractFilenameWithoutExtension(filePath);

		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;
		imageSpec.Path = filePath;
		m_Image = Image::Create(imageSpec, data);
	}

	VulkanTexture::VulkanTexture(const VulkanTexture& texture)
		: VulkanTexture(texture.GetSpecification())
	{
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	void VulkanTexture::Bind(unsigned int slot /*= 0*/) 
	{
		//m_Loaded = true;
	}

	void VulkanTexture::Unbind(unsigned slot)
	{
		//m_Loaded = false;
	}

	RendererID VulkanTexture::GetRendererID() const
	{
		return m_Image->GetRendererID();
	}

	RendererID& VulkanTexture::GetRendererID() 
	{
		return m_Image->GetRendererID();
	}

	void VulkanTexture::SetData(void* data, uint32_t size)
	{
	}

	void VulkanTexture::Lock()
	{
		m_Locked = true;
	}

	void VulkanTexture::Unlock()
	{
		m_Locked = false;
		// Submit to renderer
	}

	void VulkanTexture::Load()
	{
		m_Loaded = true;
	}

	void VulkanTexture::Unload()
	{
		m_Loaded = false;
	}

	s_ptr<Image> VulkanTexture::GetImage()
	{
		return m_Image;
	}

	//=====================================================
	// VulkanTexture 2D
	//=====================================================
	VulkanTexture2D::VulkanTexture2D(const std::string& filePath)
	{
		LOG_TRACE("VulkanTexture2D created: {}", filePath);
		m_FilePath = filePath;
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		ImageSpecification imageSpec;
		imageSpec.Width = width;
		imageSpec.Height = height;
		m_Width = width;
		m_Height = height;
		m_Name = File::ExtractFilenameWithoutExtension(filePath);

		m_Image = Image::Create(imageSpec, data);
		stbi_image_free(data);
	}

	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& specification, Buffer imageData)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.Path = specification.Path;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.Filter;
		imageSpec.Wrap = specification.Wrap;

		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		m_Image = Image::Create(imageSpec, imageData);
	}

	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		ImageSpecification imageSpec;
		imageSpec.Name = specification.Name;
		imageSpec.Width = specification.Width;
		imageSpec.Height = specification.Height;
		imageSpec.Path = specification.Path;
		imageSpec.Format = specification.Format;
		imageSpec.Filter = specification.Filter;
		imageSpec.Wrap = specification.Wrap;

		m_Name = specification.Name;
		if (specification.Name.empty())
			m_Name = specification.Path;

		// Try to read data from path
		if (specification.Path.empty() == false)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);

			stbi_uc* data = stbi_load(specification.Path.c_str(), &width, &height, &channels, 4);
            uint32_t memorySize = Image::GetMemorySize(specification.Format, specification.Width, specification.Height);

			imageSpec.Width = width;
			imageSpec.Height = height;
			m_Image = Image::Create(imageSpec, data);
		}
		else
			m_Image = Image::Create(imageSpec, nullptr);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
	}

	void VulkanTexture2D::Bind(unsigned int slot /*= 0*/)
	{
	}

	void VulkanTexture2D::Unbind(unsigned slot)
	{
	}

	RendererID VulkanTexture2D::GetRendererID() const
	{
		return m_Image->GetRendererID();
	}

	RendererID& VulkanTexture2D::GetRendererID() 
	{
		return m_Image->GetRendererID();
	}

	// TODO: Remove this ?
	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
	}

	void VulkanTexture2D::Lock()
	{
		m_Locked = true;
	}

	void VulkanTexture2D::Unlock()
	{
		m_Locked = false;
		// Submit to renderer
	}

	void VulkanTexture2D::Load()
	{
		m_Loaded = true;
	}

	void VulkanTexture2D::Unload()
	{
		m_Loaded = false;
	}

	s_ptr<Image> VulkanTexture2D::GetImage()
	{
		return m_Image;
	}

}