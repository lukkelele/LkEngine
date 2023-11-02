#pragma once

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {


	class OpenGLImage : public Image
	{
	public:
		//OpenGLImage(std::string_view stringView);
		OpenGLImage(ImageSpecification spec, Buffer buffer);
		OpenGLImage(ImageSpecification spec, const void* data = nullptr);
		~OpenGLImage();

		void SetData(const void* data);
		void Resize(uint32_t width, uint32_t height);

		RendererID GetRendererID() const { return m_RendererID; }
		Buffer GetBuffer() const { return m_ImageData; }
		Buffer& GetBuffer() { return m_ImageData; }	
		uint32_t GetWidth() const { return m_Specification.Width; }
		uint32_t GetHeight() const { return m_Specification.Height; }
		int64_t GetImageFormat(ImageFormat fmt);
		const ImageSpecification GetImageSpecification() const { return m_Specification; }

	private:
		void AllocateMemory(uint64_t size);
		void ReleaseMemory();

	private:
		RendererID m_RendererID = 0;
		ImageSpecification m_Specification;
		uint32_t m_Width, m_Height;
		std::string m_FilePath;
		Buffer m_ImageData;
	};


}