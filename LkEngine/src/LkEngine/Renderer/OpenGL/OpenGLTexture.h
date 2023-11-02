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
	};


}