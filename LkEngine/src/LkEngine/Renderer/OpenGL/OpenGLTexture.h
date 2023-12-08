#pragma once

#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filePath);
		OpenGLTexture(const TextureSpecification& textureSpec);
		virtual ~OpenGLTexture();

		std::string GetName() const { return m_Name; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetPath() const { return m_FilePath; }
		RendererID GetRendererID() const;

		void Bind(unsigned int slot = 0);
		void Unbind();
		void Lock();
		void Unlock();
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }

		void Load();
		void Unload();

	protected:
		unsigned int m_RendererID;
		std::string m_Name;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		float m_ScalerX = 1.0f;
		float m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
	};


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification); 
		OpenGLTexture2D(const std::string& filePath); 
		~OpenGLTexture2D();

		RendererID GetRendererID() const;
		std::string GetName() const { return m_Name; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetPath() const { return m_FilePath; }
		bool IsLoaded() const { return m_Loaded; }
		void SetData(void* data, uint32_t size);
		Buffer GetWriteableBuffer() { return m_Image->GetBuffer(); }

		void Bind(unsigned int slot = 0);
		void Unbind();
		void Lock();
		void Unlock();

		void Load();
		void Unload();

	private:
		std::string m_Name;
		std::string m_FilePath;
		uint32_t m_Width, m_Height;
		bool m_Loaded = false;
		bool m_Locked = false;
		TextureSpecification m_Specification;
		s_ptr<Image> m_Image = nullptr;
	};


}