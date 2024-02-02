#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "LkOpenGL.h"
#include "OpenGLImage.h"


namespace LkEngine {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const TextureSpecification& textureSpec, Buffer imageData);
		OpenGLTexture(const TextureSpecification& textureSpec);
		OpenGLTexture(const OpenGLTexture& texture);
		~OpenGLTexture();

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Resize(uint32_t width, uint32_t height) override;
		void Invalidate() override;

		Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;

		TextureSpecification GetSpecification() const override { return m_Specification; }
		uint64_t GetARBHandle() const; 

		void Bind(unsigned int slot = 0) override;
		void Unbind(unsigned int slot = 0) override;

		void Lock();
		void Unlock();
		void Load();
		void Unload();

		void SetInTextureArray(bool inTextureArray) { m_InTextureArray = inTextureArray; }

		RendererID GetRendererID() const override;
		RendererID& GetRendererID();
		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		FilePath GetPath() const { return m_FilePath; }
		FilePath& GetPath() { return m_FilePath; }

	protected:
		Ref<Image> m_Image;
		uint32_t m_Width, m_Height;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		float m_ScalerX = 1.0f, m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;

		bool m_InTextureArray = false;
	};


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& textureSpec, Buffer imageData);
		OpenGLTexture2D(const TextureSpecification& specification); 
		~OpenGLTexture2D() override;

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Invalidate() override;
		void Resize(uint32_t width, uint32_t height) override;
		Ref<Image2D> GetImage() override;

		Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;
		uint64_t GetARBHandle() const; 
		TextureSpecification GetSpecification() const override { return m_Specification; }

		void Bind(unsigned int slot = 0) override;
		void Unbind(unsigned int slot = 0) override;

		void Lock();
		void Unlock();
		void Load();
		void Unload();

		RendererID GetRendererID() const;
		RendererID& GetRendererID();

		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }

		FilePath GetPath() const { return m_FilePath; }
		FilePath& GetPath() { return m_FilePath; }

	private:
		Ref<Image2D> m_Image = nullptr;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		bool m_Loaded = false;
		bool m_Locked = false;

		bool m_InTextureArray = false;
	};

}