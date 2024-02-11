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

		virtual Buffer GetImageBuffer() = 0;

		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual TextureType GetType() const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;
		//virtual const RendererID& GetRendererID() const = 0;

		//virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const = 0;
		//virtual uint64_t GetHash() const = 0;
		//virtual glm::uvec2 GetSize() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;

#if 0
		virtual ~Texture() = default;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Invalidate() = 0;

		virtual Buffer GetImageBuffer() = 0;

		virtual void Bind(unsigned int slot = 0) = 0;
		virtual void Unbind(unsigned int slot = 0) = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool IsLoaded() const = 0; // { return m_Loaded; }
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual TextureSpecification GetSpecification() const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;
		virtual std::string GetName() const = 0;
		virtual uint32_t GetWidth() const = 0; 
		virtual uint32_t GetHeight() const = 0; 

		virtual std::filesystem::path GetPath() const = 0;
		virtual std::filesystem::path & GetPath() = 0;
#endif
	};


	class Texture2D : public Texture
	{
	public:
#if 0
		virtual ~Texture2D() = default;

		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer imageData);
		//static Ref<Texture2D> Create(const TextureSpecification& specification, const std::filesystem::path& filepath);

		//virtual void Resize(const glm::uvec2& size) = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual Ref<Image2D> GetImage() = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;

		virtual Buffer GetImageBuffer() = 0;
		virtual bool Loaded() const = 0;
		//virtual const std::filesystem::path& GetPath() const = 0;

		virtual TextureType GetType() const override { return TextureType::Texture2D; }

		static AssetType GetStaticType() { return AssetType::Texture; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
#endif

		virtual ~Texture2D() = default;

		virtual Ref<Image2D> GetImage() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

		virtual Buffer GetImageBuffer() = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;
		virtual void Invalidate() = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual bool IsLoaded() const = 0;

		virtual uint32_t GetMipLevelCount() const = 0;
		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual void Load() = 0;
		virtual void Unload() = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		//virtual std::string GetName() const = 0;
		virtual uint32_t GetWidth() const = 0;  
		virtual uint32_t GetHeight() const = 0;

		//virtual std::filesystem::path GetPath() const = 0;
		//virtual std::filesystem::path & GetPath() = 0;

		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer imageData);
	};


	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const TextureSpecification& specification, Buffer imageData = Buffer());

		virtual TextureType GetType() const override { return TextureType::TextureCube; }

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

	};

}
