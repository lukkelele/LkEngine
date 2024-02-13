#pragma once

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	//-------------------------------------------------------------------------------
	// OpenGLImage
	//-------------------------------------------------------------------------------
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

		const ImageSpecification& GetSpecification() const { return m_Specification; }
		//ImageSpecification& GetSpecification() { return m_Specification; }

	private:
		void Release() override;
		void AllocateMemory(uint64_t size) override;

	private:
		RendererID m_RendererID = 0;

		ImageSpecification m_Specification;
		std::string m_FilePath;

		Buffer m_ImageData;

		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};


	//-------------------------------------------------------------------------------
	// OpenGLImage2D
	//-------------------------------------------------------------------------------
	class OpenGLImage2D : public Image2D
	{
	public:
		OpenGLImage2D(const ImageSpecification spec, Buffer buffer);
		OpenGLImage2D(const ImageSpecification spec, void* data = nullptr);
		~OpenGLImage2D();

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

		const ImageSpecification& GetSpecification() const { return m_Specification; }
		//const ImageSpecification GetSpecification() const { return m_Specification; }
		//ImageSpecification& GetSpecification() { return m_Specification; }

	private:
		void Release() override;
		void AllocateMemory(uint64_t size) override;

	private:
		RendererID m_RendererID = 0;

		ImageSpecification m_Specification;
		std::filesystem::path m_FilePath;

		Buffer m_ImageData;

		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};

	namespace GLUtils {

		void ApplyTextureFilter(TextureFilter filter, bool mipmap = true);
		void ApplyTextureWrap(TextureWrap wrap);

	}


}