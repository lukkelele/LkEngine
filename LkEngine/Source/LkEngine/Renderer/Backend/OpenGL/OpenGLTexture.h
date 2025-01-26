#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "OpenGLImage.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	class LOpenGLTexture2D : public LTexture2D
	{
	public:
		LOpenGLTexture2D(const FTextureSpecification& InSpecification, FBuffer InBuffer);
		LOpenGLTexture2D(const FTextureSpecification& InSpecification); 
		LOpenGLTexture2D() = delete;
		~LOpenGLTexture2D();

		virtual void SetData(void* InData, const uint32_t InSize) override;
		virtual void Invalidate() override;

		FORCEINLINE virtual TObjectPtr<LImage2D> GetImage() override { return Image; }
		FORCEINLINE virtual const TObjectPtr<LImage2D>& GetImage() const override { return Image; }

		virtual void Resize(const uint32_t InWidth, const uint32_t InHeight) override;

		virtual FBuffer GetImageBuffer() override { return Image->GetBuffer(); }

		virtual uint32_t GetMipLevelCount() const override;

		virtual void Bind(const uint32_t Slot = 0) const override;
		virtual void Unbind(const uint32_t Slot = 0) const;

		FORCEINLINE virtual EImageFormat GetFormat() const override 
		{ 
			LK_CORE_ASSERT(Image);
			return Image->GetSpecification().Format; 
		}

		virtual void Lock() override;
		virtual void Unlock() override;
		virtual void Load() override;
		virtual void Unload() override;

		virtual bool Loaded() const override { return bLoaded; }

		FORCEINLINE virtual LRendererID GetRendererID() const override { return Image->GetRendererID(); }
		FORCEINLINE virtual LRendererID& GetRendererID() override { return Image->GetRendererID(); }

		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }

		FORCEINLINE virtual std::string_view GetName() const override { return Specification.Name; }
		FORCEINLINE virtual std::string_view GetFilename() const override { return FileName; }
		FORCEINLINE virtual const std::filesystem::path& GetPath() const override { return m_FilePath; }

		FORCEINLINE virtual const FTextureSpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

		int GetArrayTextureIndex() const { return ArrayTextureIndex; }
		void SetArrayIndex(const int ArrayIndex) 
		{ 
			ArrayTextureIndex = ArrayIndex; 
		}

		/* TODO: Remove */
		uint64_t GetARBHandle() const;

	private:
		TObjectPtr<LImage2D> Image = nullptr;
		uint32_t m_Width{};
		uint32_t m_Height{};
		FTextureSpecification Specification;

		std::filesystem::path m_FilePath;
		std::string FileName{};

		bool bLoaded = false;
		bool bLocked = false;

		int ArrayTextureIndex = 0;
	};


	class LOpenGLTextureCube : public LTextureCube
	{
	public:
		LOpenGLTextureCube(const FTextureSpecification& InSpecification, std::vector<std::filesystem::path> InFacePaths);
		LOpenGLTextureCube() = delete;
		~LOpenGLTextureCube();

		virtual void Bind(const uint32_t Slot = 0) const override;

		FORCEINLINE virtual EImageFormat GetFormat() const override 
		{ 
			return Specification.Format; 
		}

		FORCEINLINE virtual uint32_t GetWidth() const override { return m_Width; }
		FORCEINLINE virtual uint32_t GetHeight() const override { return m_Height; }

		/// TODO: Needs implementation
		virtual uint32_t GetMipLevelCount() const override { return 0; }

		FORCEINLINE virtual LRendererID GetRendererID() const override { return m_RendererID; }
		FORCEINLINE virtual LRendererID& GetRendererID() override { return m_RendererID; }

		FORCEINLINE virtual std::string_view GetName() const override { return Specification.Name; }
		FORCEINLINE virtual std::string_view GetFilename() const override { return FileName; }
		FORCEINLINE virtual const std::filesystem::path& GetPath() const override { return m_FilePath;  }

		FORCEINLINE virtual ETextureType GetType() const override { return ETextureType::TextureCube; }

		FORCEINLINE virtual const FTextureSpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

	private:
		LRendererID m_RendererID = 0;
		FTextureSpecification Specification{};

		FBuffer DataBuffer{};
		uint32_t m_Width{};
		uint32_t m_Height{};

		std::filesystem::path m_FilePath{};
		std::string FileName{};

		bool m_MipsGenerated = false;
	};


}
