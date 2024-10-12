#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Asset/Asset.h"

#include "Image.h"


namespace LkEngine {

	class LTexture : public LAsset
	{
	public:
		virtual ~LTexture() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual TextureType GetType() const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual const TextureSpecification& GetSpecification() const = 0;

		//virtual const std::string& GetName() const = 0;
		virtual std::string_view GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual std::string_view GetFilename() const = 0;

		/** Factory function. */
		static TObjectPtr<LTexture> Create(const TextureSpecification& specification);

	private:
		LCLASS(LTexture)
	};


	class LTexture2D : public LTexture
	{
	public:
		virtual ~LTexture2D() = default;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Invalidate() = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual TObjectPtr<LImage2D> GetImage() = 0;
		virtual const TObjectPtr<LImage2D>& GetImage() const = 0;
		virtual FBuffer GetImageBuffer() = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool Loaded() const = 0;

		virtual TextureType GetType() const override { return TextureType::Texture2D; }
		static EAssetType GetStaticType() { return EAssetType::Texture; }

		static TObjectPtr<LTexture2D> Create(const TextureSpecification& specification);
		static TObjectPtr<LTexture2D> Create(const TextureSpecification& specification, FBuffer imageData);

	private:
		LASSET(LTexture2D)
	};


	class LTextureCube : public LTexture
	{
	public:
		static TObjectPtr<LTextureCube> Create(const TextureSpecification& specification, 
											   const std::vector<std::filesystem::path>& InFacePaths);

		FORCEINLINE virtual TextureType GetType() const override { return TextureType::TextureCube; }

		FORCEINLINE static EAssetType GetStaticType() { return EAssetType::EnvMap; }

	private:
		LASSET(LTextureCube)
	};

}
