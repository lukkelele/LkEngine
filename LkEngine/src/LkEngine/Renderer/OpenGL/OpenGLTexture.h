#pragma once

#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filePath);
		OpenGLTexture(const TextureSpecification& textureSpec);
		virtual ~OpenGLTexture();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
		void Lock();
		void Unlock();
		const std::string& GetPath() const { return m_FilePath; }
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }

		void Load();
		void Unload();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

	protected:
		unsigned int m_RendererID;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		float m_ScalerX, m_ScalerY;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
	};





	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification); // : Texture2D(specification) {}
		OpenGLTexture2D(const std::string& filePath); // : Texture2D(filePath) {}
		~OpenGLTexture2D();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
		void Lock();
		void Unlock();
		const std::string& GetPath() const { return m_FilePath; }
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }

		void Load();
		void Unload();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

	private:
		//unsigned int m_RendererID;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
	};


}