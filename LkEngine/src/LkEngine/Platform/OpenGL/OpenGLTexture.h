#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "OpenGLImage.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


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

		void Resize(const uint32_t width, const uint32_t height);
		void Invalidate();

		Buffer GetImageBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;

		const TextureSpecification& GetSpecification() const { return m_Specification; }
		uint64_t GetARBHandle() const; 

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const;

		void Lock();
		void Unlock();
		void Load();
		void Unload();

		RendererID GetRendererID() const override;
		RendererID& GetRendererID() override;

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		const std::filesystem::path& GetPath() const override { return m_FilePath; }

		int GetArrayIndex() const { return m_TextureArrayIndex; }
		void SetArrayIndex(int idx) { m_TextureArrayIndex = idx; }

		TextureType GetType() const override { return TextureType::Texture2D; }
		ImageFormat GetFormat() const override { return m_Specification.Format; }

		const std::string& GetName() const { return m_Specification.Name; }
		//glm::uvec2 GetSize() const override;

	protected:
		Ref<Image> m_Image;
		uint32_t m_Width, m_Height;

		TextureSpecification m_Specification;
		std::filesystem::path m_FilePath;

		float m_ScalerX = 1.0f, m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;

		int m_TextureArrayIndex = 0;
	};


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification); 
		OpenGLTexture2D(const TextureSpecification& specification, Buffer imageData);
		~OpenGLTexture2D() override;

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Invalidate();
		Ref<Image2D> GetImage();

		void Resize(const uint32_t width, const uint32_t height) override;

		Buffer GetImageBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;
		const TextureSpecification& GetSpecification() const override { return m_Specification; }

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const;

		TextureType GetType() const override { return TextureType::Texture2D; }
		ImageFormat GetFormat() const override { return m_Specification.Format; }

		void Lock() override;
		void Unlock() override;
		void Load();
		void Unload();

		//bool Loaded() const override { return m_Loaded; }
		RendererID GetRendererID() const override;
		RendererID& GetRendererID() override;

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		const std::string& GetName() const override { return m_Specification.Name; }

		const std::filesystem::path& GetPath() const override { return m_FilePath; }

		int GetArrayIndex() const { return m_TextureArrayIndex; }
		void SetArrayIndex(int idx) { m_TextureArrayIndex = idx; }
		uint64_t GetARBHandle() const; 

	private:
		Ref<Image2D> m_Image = nullptr;
		uint32_t m_Width, m_Height;
		TextureSpecification m_Specification;
		std::filesystem::path m_FilePath;

		bool m_Loaded = false;
		bool m_Locked = false;

		int m_TextureArrayIndex = 0;
	};


	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(const TextureSpecification& specification, Buffer data);
		~OpenGLTextureCube();

		void Bind(uint32_t slot = 0) const;

		ImageFormat GetFormat() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetMipLevelCount() const;
		//virtual uint64_t GetHash() const = 0;

		Buffer GetImageBuffer() override { return Buffer(); }

		RendererID GetRendererID() const override;
		RendererID& GetRendererID() override;

		const std::string& GetName() const;
		const std::filesystem::path& GetPath() const;

		TextureType GetType() const override { return TextureType::TextureCube; }

	private:
		RendererID m_RendererID;
		TextureSpecification m_Specification;
		std::filesystem::path m_Filepath;

		Buffer m_LocalData;
		bool m_MipsGenerated = false;

	};


}