#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "OpenGLImage.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification); 
		OpenGLTexture2D(const TextureSpecification& specification, Buffer imageData);
		~OpenGLTexture2D();

		void SetData(void* data, uint32_t size) override;

		void Invalidate() override;
		Ref<Image2D> GetImage() override;

		void Resize(const uint32_t width, const uint32_t height) override;

		Buffer GetImageBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const;

		ImageFormat GetFormat() const override { return m_Image->GetSpecification().Format; }
		//TextureType GetType() const override { return TextureType::Texture2D; }

		void Lock() override;
		void Unlock() override;
		void Load() override;
		void Unload() override;

		bool Loaded() const override { return m_Loaded; }
		RendererID GetRendererID() const override;
		RendererID& GetRendererID() override;

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		const std::string& GetName() const override { return m_Specification.Name; }
		const TextureSpecification& GetSpecification() const override { return m_Specification; }

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
		OpenGLTextureCube(const TextureSpecification& specification, std::vector<std::filesystem::path> facePaths);
		~OpenGLTextureCube();

		void Bind(uint32_t slot = 0) const override;

		ImageFormat GetFormat() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint32_t GetMipLevelCount() const override;

		RendererID GetRendererID() const override;
		RendererID& GetRendererID() override;

		const std::string& GetName() const override;
		const std::filesystem::path& GetPath() const override;

		TextureType GetType() const override { return TextureType::TextureCube; }
		const TextureSpecification& GetSpecification() const override { return m_Specification; }

	private:
		RendererID m_RendererID;
		Buffer m_LocalData;
		std::filesystem::path m_Filepath;
		TextureSpecification m_Specification;
		uint32_t m_Width, m_Height;

		bool m_MipsGenerated = false;
	};


}