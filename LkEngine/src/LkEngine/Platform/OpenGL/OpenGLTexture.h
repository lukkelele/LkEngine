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
		virtual ~OpenGLTexture() override;

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Resize(uint32_t width, uint32_t height) override;
		void Invalidate() override;

		Ref<Image> GetImage() override;
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
		std::string GetName() const override { return m_Name; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		const std::string& GetPath() const override { return m_FilePath; }

	protected:
		std::string m_Name;
		std::string m_FilePath;

		uint32_t m_Width, m_Height;
		float m_ScalerX = 1.0f;
		float m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		Ref<Image> m_Image = nullptr;

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

		Ref<Image> GetImage() override;
		Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		void Invalidate() override;
		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetMipLevelCount() const override;
		uint64_t GetARBHandle() const; 
		TextureSpecification GetSpecification() const override { return m_Specification; }

		void Bind(unsigned int slot = 0) override;
		void Unbind(unsigned int slot = 0) override;
		void Lock();
		void Unlock();
		void Load();
		void Unload();

		const std::string& GetPath() const override { return m_FilePath; }

		RendererID GetRendererID() const;
		RendererID& GetRendererID();
		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }

		void SetInTextureArray(bool inTextureArray) { m_InTextureArray = inTextureArray; }

	private:
		std::string m_FilePath;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		Ref<Image> m_Image = nullptr;

		bool m_InTextureArray = false;
	};

}