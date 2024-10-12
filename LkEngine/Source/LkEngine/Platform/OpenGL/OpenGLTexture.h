#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "OpenGLImage.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	class OpenGLTexture2D : public LTexture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification, FBuffer InBuffer);
		OpenGLTexture2D(const TextureSpecification& specification); 
		~OpenGLTexture2D();

		virtual void SetData(void* data, uint32_t size) override;
		virtual void Invalidate() override;

		FORCEINLINE virtual TObjectPtr<LImage2D> GetImage() override { return m_Image; }
		FORCEINLINE virtual const TObjectPtr<LImage2D>& GetImage() const override { return m_Image; }

		virtual void Resize(const uint32_t width, const uint32_t height) override;

		virtual FBuffer GetImageBuffer() override 
		{ 
			return m_Image->GetBuffer(); 
		}

		virtual uint32_t GetMipLevelCount() const override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const;

		virtual ImageFormat GetFormat() const override { return m_Image->GetSpecification().Format; }

		virtual void Lock() override;
		virtual void Unlock() override;
		virtual void Load() override;
		virtual void Unload() override;

		virtual bool Loaded() const override { return m_Loaded; }
		virtual RendererID GetRendererID() const override;
		virtual RendererID& GetRendererID() override;

		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }

		//virtual const std::string& GetName() const override 
		FORCEINLINE virtual std::string_view GetName() const override { return m_Specification.Name; }
		FORCEINLINE virtual std::string_view GetFilename() const override { return FileName; }
		FORCEINLINE virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }
		FORCEINLINE virtual const std::filesystem::path& GetPath() const override { return m_FilePath; }

		FORCEINLINE int GetArrayIndex() const { return m_TextureArrayIndex; }
		FORCEINLINE void SetArrayIndex(const int ArrayIndex) 
		{ 
			m_TextureArrayIndex = ArrayIndex; 
		}

		uint64_t GetARBHandle() const;

	private:
		TObjectPtr<LImage2D> m_Image = nullptr;
		uint32_t m_Width{};
		uint32_t m_Height{};
		TextureSpecification m_Specification;

		std::filesystem::path m_FilePath;
		std::string FileName{};

		bool m_Loaded = false;
		bool m_Locked = false;

		int m_TextureArrayIndex = 0;
	};


	class OpenGLTextureCube : public LTextureCube
	{
	public:
		OpenGLTextureCube(const TextureSpecification& specification, 
						  std::vector<std::filesystem::path> facePaths);
		~OpenGLTextureCube();

		virtual void Bind(uint32_t slot = 0) const override;

		FORCEINLINE virtual ImageFormat GetFormat() const override { return m_Specification.Format; }
		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }

		/// TODO: Needs implementation
		virtual uint32_t GetMipLevelCount() const override { return 0; }

		FORCEINLINE virtual RendererID GetRendererID() const override { return m_RendererID; }
		FORCEINLINE virtual RendererID& GetRendererID() override { return m_RendererID; }

		FORCEINLINE virtual std::string_view GetName() const override { return m_Specification.Name; }
		FORCEINLINE virtual std::string_view GetFilename() const override { return FileName; }
		FORCEINLINE virtual const std::filesystem::path& GetPath() const override { return m_FilePath;  }

		FORCEINLINE virtual TextureType GetType() const override { return TextureType::TextureCube; }

		FORCEINLINE virtual const TextureSpecification& GetSpecification() const override 
		{ 
			return m_Specification; 
		}

	private:
		RendererID m_RendererID = 0;
		TextureSpecification m_Specification{};

		FBuffer DataBuffer{};
		uint32_t m_Width{};
		uint32_t m_Height{};

		std::filesystem::path m_FilePath{};
		std::string FileName{};

		bool m_MipsGenerated = false;
	};


}