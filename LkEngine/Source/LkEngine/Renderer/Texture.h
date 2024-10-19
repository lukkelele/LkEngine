#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Asset/Asset.h"

#include "Image.h"


namespace LkEngine {

	class LTexture : public LAsset
	{
	public:
		virtual ~LTexture() = default;

		virtual void Bind(const uint32_t Slot = 0) const = 0;

		virtual EImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual ETextureType GetType() const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual const FTextureSpecification& GetSpecification() const = 0;

		virtual std::string_view GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual std::string_view GetFilename() const = 0;

		/** Factory function. */
		static TObjectPtr<LTexture> Create(const FTextureSpecification& Specification);

	private:
		LCLASS(LTexture)
	};


	class LTexture2D : public LTexture
	{
	public:
		virtual ~LTexture2D() = default;

		virtual void SetData(void* InData, const uint32_t InSize) = 0;
		virtual void Invalidate() = 0;
		virtual void Resize(const uint32_t NewWidth, const uint32_t NewWeight) = 0;

		virtual TObjectPtr<LImage2D> GetImage() = 0;
		virtual const TObjectPtr<LImage2D>& GetImage() const = 0;
		virtual FBuffer GetImageBuffer() = 0;

		virtual void Bind(const uint32_t Slot = 0) const = 0;
		virtual void Unbind(const uint32_t Slot = 0) const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool Loaded() const = 0;

		FORCEINLINE virtual ETextureType GetType() const override 
		{ 
			return ETextureType::Texture2D; 
		}

		FORCEINLINE static EAssetType GetStaticType() { return EAssetType::Texture; }

		static TObjectPtr<LTexture2D> Create(const FTextureSpecification& specification);
		static TObjectPtr<LTexture2D> Create(const FTextureSpecification& specification, FBuffer imageData);

	private:
		LASSET(LTexture2D)
	};


	class LTextureCube : public LTexture
	{
	public:
		static TObjectPtr<LTextureCube> Create(const FTextureSpecification& Specification, 
											   const std::vector<std::filesystem::path>& InFacePaths);

		FORCEINLINE virtual ETextureType GetType() const override 
		{ 
			return ETextureType::TextureCube; 
		}

		FORCEINLINE static EAssetType GetStaticType() { return EAssetType::EnvMap; }

	private:
		LASSET(LTextureCube)
	};

}
