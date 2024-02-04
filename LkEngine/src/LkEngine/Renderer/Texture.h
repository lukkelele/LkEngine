#pragma once

#include "LkEngine/Core/Base.h"

#include "Image.h"



namespace LkEngine {

	enum class TextureUniformType : uint8_t
	{
		Diffuse = 0,
		Specular,
		Normal,
		Height,
		Emissive,
		DiffuseRoughness,
	};

	struct TextureSpecification
	{
		std::string Path = "";
		std::string Name = "";
		uint32_t Width = 1;
		uint32_t Height = 1;
		bool GenerateMips = true;

		ImageFormat Format = ImageFormat::RGBA;
		TextureWrap SamplerWrap = TextureWrap::Clamp;
		TextureFilter SamplerFilter = TextureFilter::Linear;

		TextureUniformType UniformType;

		bool Storage = false;
		bool StoreLocally = false;

		std::string DebugName;
	};

	class Texture : public RefCounted
	{
	public:
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

		static Ref<Texture> Create(const TextureSpecification& specification);
	};


	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual Ref<Image2D> GetImage() = 0;

		virtual Buffer GetImageBuffer() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Invalidate() = 0;

		virtual void Bind(unsigned int slot = 0) = 0;
		virtual void Unbind(unsigned int slot = 0) = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual bool IsLoaded() const = 0;

		virtual uint32_t GetMipLevelCount() const = 0;
		virtual TextureSpecification GetSpecification() const = 0;

		virtual void Load() = 0;
		virtual void Unload() = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual std::string GetName() const = 0;
		virtual uint32_t GetWidth() const = 0;  
		virtual uint32_t GetHeight() const = 0;

		virtual std::filesystem::path GetPath() const = 0;
		virtual std::filesystem::path & GetPath() = 0;

		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer imageData);
	};

}
