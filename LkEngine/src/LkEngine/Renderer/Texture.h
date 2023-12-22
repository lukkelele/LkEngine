#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	struct TextureSpecification
	{
		std::string Path = "";
		std::string Name = "";
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	// TODO:
	// - UV Mapping
	// Setting the resolutions of textures should be easy
	// Should be easy to rotate etc

	class Texture
	{
	public:
		virtual ~Texture() = default;

		static s_ptr<Texture> Create(const TextureSpecification& specification);
		static s_ptr<Texture> Create(const std::string& path);

		virtual std::string GetName() const = 0;
		virtual uint32_t GetWidth() const = 0; 
		virtual uint32_t GetHeight() const = 0; 
		virtual const std::string& GetPath() const = 0;
		virtual Buffer GetWriteableBuffer() = 0;

		virtual void Bind(unsigned int slot = 0) = 0;
		virtual void Unbind() = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool IsLoaded() const = 0; // { return m_Loaded; }
		virtual RendererID GetRendererID() const = 0;

	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		static s_ptr<Texture2D> Create(const TextureSpecification& specification);
		static s_ptr<Texture2D> Create(const TextureSpecification& specification, Buffer imageData);
		static s_ptr<Texture2D> Create(const std::string& path);

		virtual std::string GetName() const = 0;
		virtual uint32_t GetWidth() const = 0;  
		virtual uint32_t GetHeight() const = 0;
		virtual const std::string& GetPath() const = 0; 
		virtual Buffer GetWriteableBuffer() = 0;

		virtual void Bind(unsigned int slot = 0) = 0;
		virtual void Unbind() = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual bool IsLoaded() const = 0;
		virtual RendererID GetRendererID() const = 0;

		virtual void Load() = 0;
		virtual void Unload() = 0;
	};

}
