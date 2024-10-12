#pragma once

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	/**
	 * LOpenGLImage
	 *
	 *  Image implementation for Open GL.
	 */
	class LOpenGLImage : public LImage
	{
	public:
		LOpenGLImage(const ImageSpecification& InSpecification, FBuffer&& InBuffer);
		LOpenGLImage(const ImageSpecification& InSpecification, void* InData);
		LOpenGLImage() = delete;
		~LOpenGLImage();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void SetData(const void* data) override;

		virtual RendererID& GetRendererID() override { return m_RendererID; }
		virtual RendererID GetRendererID() const override { return m_RendererID; }

		virtual FBuffer GetBuffer() const override { return m_ImageData; }
		virtual FBuffer& GetBuffer() override { return m_ImageData; }	
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		const ImageSpecification& GetSpecification() const { return m_Specification; }

	private:
		void Release() override;
		void AllocateMemory(uint64_t size) override;

	private:
		RendererID m_RendererID = 0;
		FBuffer m_ImageData{};

		ImageSpecification m_Specification;
		std::string m_FilePath;

		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};

	/********************************************************************************
	 ********************************************************************************/

	/**
	 * LOpenGLImage
	 *
	 *  Image implementation for Open GL.
	 */
	class LOpenGLImage2D : public LImage2D
	{
	public:
		LOpenGLImage2D(const ImageSpecification& InSpecification, FBuffer&& InBuffer);
		LOpenGLImage2D(const ImageSpecification& InSpecification, void* InData);
		LOpenGLImage2D() = delete;
		~LOpenGLImage2D();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void SetData(const void* data) override;

		virtual RendererID& GetRendererID() override { return m_RendererID; }
		virtual RendererID GetRendererID() const override { return m_RendererID; }

		virtual FBuffer GetBuffer() const override { return m_ImageData; }
		virtual FBuffer& GetBuffer() override { return m_ImageData; }	
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		const ImageSpecification& GetSpecification() const { return m_Specification; }

	private:
		virtual void Release() override;
		virtual void AllocateMemory(uint64_t size) override;

	private:
		RendererID m_RendererID = 0;

		ImageSpecification m_Specification{};
		std::filesystem::path m_FilePath{};

		FBuffer m_ImageData{};

		glm::vec4 m_TintColor = Color::RGBA::Transparent;
	};

	namespace GLUtils 
	{
		void ApplyTextureFilter(TextureFilter filter, bool mipmap = true);
		void ApplyTextureWrap(TextureWrap wrap);
	}


}