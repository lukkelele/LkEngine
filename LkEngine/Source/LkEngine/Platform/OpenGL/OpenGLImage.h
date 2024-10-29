#pragma once

#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	/**
	 * LOpenGLImage
	 *
	 *  Image implementation for OpenGL.
	 */
	class LOpenGLImage : public LImage
	{
	public:
		LOpenGLImage(const FImageSpecification& InSpecification, FBuffer&& InBuffer);
		LOpenGLImage(const FImageSpecification& InSpecification, void* InData);
		LOpenGLImage() = delete;
		~LOpenGLImage();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Resize(const uint32_t NewWidth, const uint32_t NewHeight) override;
		virtual void SetData(const void* data) override;

		virtual LRendererID& GetRendererID() override { return m_RendererID; }
		virtual LRendererID GetRendererID() const override { return m_RendererID; }

		virtual FBuffer GetBuffer() const override { return m_ImageData; }
		virtual FBuffer& GetBuffer() override { return m_ImageData; }	
		virtual uint32_t GetWidth() const override { return Specification.Width; }
		virtual uint32_t GetHeight() const override { return Specification.Height; }

		const FImageSpecification& GetSpecification() const { return Specification; }

	private:
		void Release() override;
		void AllocateMemory(uint64_t size) override;

	private:
		LRendererID m_RendererID = 0;
		FBuffer m_ImageData{};

		FImageSpecification Specification{};
		std::string m_FilePath{};

		glm::vec4 m_TintColor = Color::RGBA::Transparent;

		LCLASS(LOpenGLImage);
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
		LOpenGLImage2D(const FImageSpecification& InSpecification, FBuffer&& InBuffer);
		LOpenGLImage2D(const FImageSpecification& InSpecification, void* InData);
		LOpenGLImage2D() = delete;
		~LOpenGLImage2D();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Resize(const uint32_t NewWidth, const uint32_t NewHeight) override;
		virtual void SetData(const void* InData) override;

		virtual LRendererID& GetRendererID() override { return m_RendererID; }
		virtual LRendererID GetRendererID() const override { return m_RendererID; }

		virtual FBuffer GetBuffer() const override { return m_ImageData; }
		virtual FBuffer& GetBuffer() override { return m_ImageData; }	
		virtual uint32_t GetWidth() const override { return Specification.Width; }
		virtual uint32_t GetHeight() const override { return Specification.Height; }

		virtual const FImageSpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

	private:
		virtual void Release() override;
		virtual void AllocateMemory(uint64_t size) override;

	private:
		LRendererID m_RendererID = 0;
		FImageSpecification Specification{};

		FBuffer m_ImageData{};
		std::filesystem::path m_FilePath{};

		glm::vec4 m_TintColor = Color::RGBA::Transparent;

		LCLASS(LOpenGLImage2D);
	};

	namespace GLUtils 
	{
		void ApplyTextureFilter(ETextureFilter TextureFilter, bool bUseMipmap = true);
		void ApplyTextureWrap(ETextureWrap TextureWrap);
	}


}