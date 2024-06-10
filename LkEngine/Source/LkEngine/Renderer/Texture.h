#pragma once

#include "LkEngine/Core/Base.h"

#include "LkEngine/Asset/Asset.h"

#include "Image.h"


namespace LkEngine {

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		static Ref<Texture> Create(const TextureSpecification& specification);

		virtual void Bind(uint32_t slot = 0) const = 0;

		//virtual Buffer GetImageBuffer() = 0;

		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual TextureType GetType() const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual const TextureSpecification& GetSpecification() const = 0;
	};


	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Invalidate() = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual Ref<Image2D> GetImage() = 0;
		virtual Buffer GetImageBuffer() = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool Loaded() const = 0;

		virtual TextureType GetType() const override { return TextureType::Texture2D; }

		static AssetType GetStaticType() { return AssetType::Texture; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer imageData);
	};


	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const TextureSpecification& specification, std::vector<std::filesystem::path> facePaths);

		virtual TextureType GetType() const override { return TextureType::TextureCube; }

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

	};

}
