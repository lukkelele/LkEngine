#pragma once

#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

    class VulkanTexture : public Texture
    {
    public:
		VulkanTexture(const TextureSpecification& textureSpec);
		VulkanTexture(const TextureSpecification& textureSpec, Buffer imageData);
		VulkanTexture(const std::string& filePath);
		VulkanTexture(const VulkanTexture& texture);
		virtual ~VulkanTexture();

		RendererID GetRendererID() const;
		RendererID& GetRendererID();
		std::string GetName() const { return m_Name; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetPath() const { return m_FilePath; }
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }
		TextureSpecification GetSpecification() const { return m_Specification; }
		s_ptr<Image> GetImage();

		void Bind(unsigned int slot = 0);
		void Unbind(unsigned int slot = 0);
		void Lock();
		void Unlock();
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);

		void Load();
		void Unload();

	protected:
		std::string m_Name;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		TextureSpecification m_Specification;

		float m_ScalerX = 1.0f;
		float m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;
		s_ptr<Image> m_Image = nullptr;
    };



	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const TextureSpecification& specification); 
		VulkanTexture2D(const TextureSpecification& textureSpec, Buffer imageData);
		VulkanTexture2D(const std::string& filePath); 
		~VulkanTexture2D();

		RendererID GetRendererID() const;
		RendererID& GetRendererID();
		std::string GetName() const { return m_Name; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetPath() const { return m_FilePath; }
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }
		TextureSpecification GetSpecification() const { return m_Specification; }
		s_ptr<Image> GetImage();

		void Bind(unsigned int slot = 0);
		void Unbind(unsigned int slot = 0);
		void Lock();
		void Unlock();

		void Load();
		void Unload();

	private:
		std::string m_Name;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
	};

}