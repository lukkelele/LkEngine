#pragma once

#include <glad/glad.h>

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	class OpenGLImage : public Image
	{
	public:
		OpenGLImage(ImageSpecification spec, Buffer buffer);
		OpenGLImage(ImageSpecification spec, const void* data = nullptr);
		~OpenGLImage();

		void Invalidate();
		void Resize(uint32_t width, uint32_t height);
		void SetData(const void* data);

		RendererID& GetRendererID() { return m_RendererID; }
		RendererID GetRendererID() const { return m_RendererID; }

		Buffer GetBuffer() const { return m_ImageData; }
		Buffer& GetBuffer() { return m_ImageData; }	
		uint32_t GetWidth() const { return m_Specification.Width; }
		uint32_t GetHeight() const { return m_Specification.Height; }
		int64_t GetImageFormat(ImageFormat fmt);
		const ImageSpecification GetImageSpecification() const { return m_Specification; }

	private:
		void Release();
		void AllocateMemory(uint64_t size);

	private:
		RendererID m_RendererID = 0;
		ImageSpecification m_Specification;
		uint32_t m_Width, m_Height;
		std::string m_FilePath;
		Buffer m_ImageData;
		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};


}