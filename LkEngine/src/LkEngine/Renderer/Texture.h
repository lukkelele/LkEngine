#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Buffer.h"
#include <glad/glad.h>


namespace LkEngine {

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	// TODO:
	// - The texture class needs some of its attributes passed to a base Texture class that
	//   will be inherited by Texture (the 3D texture class) and Texture2D
	class Texture
	{
	public:
		Texture(const std::string& path);
		Texture(const TextureSpecification& textureSpec);
		virtual ~Texture();

		virtual void Bind(unsigned int slot = 0) const;
		virtual void Unbind() const;
		void Lock();
		void Unlock();
		virtual int GetWidth() const { return m_Width; }
		virtual int GetHeight() const { return m_Height; }
		virtual const std::string& GetPath() const { return m_Path; }
		virtual bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer();

	protected:
		unsigned int m_RendererID;
		uint32_t m_Width, m_Height;
		Buffer m_ImageData;
		std::string m_Path;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		GLenum m_InternalFormat, m_DataFormat;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const TextureSpecification& specification) 
			: Texture(specification) {}

		Texture2D(const std::string& path) 
			: Texture(path) {}

		static s_ptr<Texture2D> Create(const TextureSpecification& specification);
		static s_ptr<Texture2D> Create(const std::string& path);
	};

}
