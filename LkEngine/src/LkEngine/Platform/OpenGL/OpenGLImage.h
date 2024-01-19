#pragma once

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	class OpenGLImage : public Image
	{
	public:
		OpenGLImage(ImageSpecification spec, Buffer buffer);
		OpenGLImage(ImageSpecification spec, void* data = nullptr);
		~OpenGLImage();

		void Invalidate() override;
		void RT_Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;
		void SetData(const void* data) override;

		RendererID& GetRendererID() override { return m_RendererID; }
		RendererID GetRendererID() const override { return m_RendererID; }

		Buffer GetBuffer() const override { return m_ImageData; }
		Buffer& GetBuffer() override { return m_ImageData; }	
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }

		int64_t GetImageFormat(ImageFormat fmt) override;

		const ImageSpecification GetSpecification() const { return m_Specification; }
		ImageSpecification& GetSpecification() { return m_Specification; }

		uint64_t GetARBHandle() const { return m_HandleARB; } // TODO: Remove

	private:
		void Release() override;
		void AllocateMemory(uint64_t size) override;

	private:
		RendererID m_RendererID = 0;
		uint64_t m_HandleARB = 0;

		ImageSpecification m_Specification;
		std::string m_FilePath;

		Buffer m_ImageData;

		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};


}