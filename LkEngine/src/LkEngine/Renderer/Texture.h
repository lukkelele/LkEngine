#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		static s_ptr<Texture> Create(const TextureSpecification& specification);
		static s_ptr<Texture> Create(const std::string& path);

		virtual uint32_t GetWidth() const = 0; 
		virtual uint32_t GetHeight() const = 0; 
		virtual const std::string& GetPath() const = 0;
		virtual Buffer GetWriteableBuffer() = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;
		virtual void Unbind() const = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual bool IsLoaded() const = 0; // { return m_Loaded; }

#if 0
	protected:
		unsigned int m_RendererID;
		//uint32_t m_Width, m_Height;
		//std::string m_FilePath;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
		//GLenum m_InternalFormat, m_DataFormat;
#endif
	};

	class Texture2D : public Texture
	{
	public:
		//Texture2D(const TextureSpecification& specification) : Texture(specification) {}
		//Texture2D(const std::string& path) : Texture(path) {}
		virtual ~Texture2D() = default;

		static s_ptr<Texture2D> Create(const TextureSpecification& specification);
		static s_ptr<Texture2D> Create(const std::string& path);

		virtual void Bind(unsigned int slot = 0) const = 0;
		virtual void Unbind() const = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual bool IsLoaded() const = 0;

		virtual uint32_t GetWidth() const = 0;  
		virtual uint32_t GetHeight() const = 0;
		virtual const std::string& GetPath() const = 0; 
		virtual Buffer GetWriteableBuffer() = 0;
	};

}
