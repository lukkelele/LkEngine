#pragma once

#include "LkEngine/Renderer/Texture.h"

#include "LkOpenGL.h"
#include "OpenGLImage.h"


namespace LkEngine {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const TextureSpecification& textureSpec, Buffer imageData);
		OpenGLTexture(const TextureSpecification& textureSpec);
		OpenGLTexture(const OpenGLTexture& texture);
		~OpenGLTexture();

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Resize(uint32_t width, uint32_t height) override;
		void Invalidate() override;

		Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;

		TextureSpecification GetSpecification() const override { return m_Specification; }
		uint64_t GetARBHandle() const; 

		void Bind(unsigned int slot = 0) override;
		void Unbind(unsigned int slot = 0) override;

		void Lock();
		void Unlock();
		void Load();
		void Unload();

		void SetInTextureArray(bool inTextureArray) { m_InTextureArray = inTextureArray; }

		RendererID GetRendererID() const override;
		RendererID& GetRendererID();
		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		FilePath GetPath() const { return m_FilePath; }
		FilePath& GetPath() { return m_FilePath; }

	protected:
		Ref<Image> m_Image;
		uint32_t m_Width, m_Height;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		float m_ScalerX = 1.0f, m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;

		bool m_InTextureArray = false;
	};


	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& textureSpec, Buffer imageData);
		OpenGLTexture2D(const TextureSpecification& specification); 
		~OpenGLTexture2D() override;

		void SetData(void* data, uint32_t size);
		bool IsLoaded() const { return m_Loaded; }

		void Invalidate() override;
		void Resize(uint32_t width, uint32_t height) override;
		Ref<Image2D> GetImage() override;

		Buffer GetWriteableBuffer() override { return m_Image->GetBuffer(); }
		uint32_t GetMipLevelCount() const override;
		uint64_t GetARBHandle() const; 
		TextureSpecification GetSpecification() const override { return m_Specification; }

		void Bind(unsigned int slot = 0) override;
		void Unbind(unsigned int slot = 0) override;

		void Lock();
		void Unlock();
		void Load();
		void Unload();

		RendererID GetRendererID() const;
		RendererID& GetRendererID();

		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }

		FilePath GetPath() const { return m_FilePath; }
		FilePath& GetPath() { return m_FilePath; }

	private:
		Ref<Image2D> m_Image = nullptr;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		bool m_Loaded = false;
		bool m_Locked = false;

		bool m_InTextureArray = false;
	};


	enum TextureArrayDimension : uint8_t
	{
		Dimension_200x200 = 0,
		Dimension_400x400,
		Dimension_800x800,
		Dimension_1024x1024,
		Dimension_2048x2048,
		Dimension_4096x4096,
	};

	struct TextureArraySpecification
	{
		TextureArrayDimension Dimension = Dimension_1024x1024;
		int TextureSlot = 0;
		TextureArraySpecification() = default;
	};

	class TextureArray : public RefCounted
	{
	public:
		TextureArray() = default;
		TextureArray(const TextureArraySpecification& specification);

		void Bind();
		void Unbind();
		//void Bind(int slot);
		static void UnbindAll(int slot = 0);

		const RendererID GetRendererID() const { return m_RendererID; }
		RendererID& GetRendererID() { return m_RendererID; }

		int GetTextureSlot() const { return m_Specification.TextureSlot; }
		const TextureArrayDimension& GetDimension() const { return m_Specification.Dimension; }
		const TextureArraySpecification& GetSpecification() const { return m_Specification; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }

		static std::pair<int, int> ConvertDimensionsToWidthAndHeight(const TextureArrayDimension& dimension)
		{
			switch (dimension)
			{
				case TextureArrayDimension::Dimension_200x200:   return { 200, 200 };
				case TextureArrayDimension::Dimension_400x400:   return { 400, 400 };
				case TextureArrayDimension::Dimension_800x800:   return { 800, 800 };
				case TextureArrayDimension::Dimension_1024x1024: return { 1024, 1024 };
				case TextureArrayDimension::Dimension_2048x2048: return { 2048, 2048 };
				case TextureArrayDimension::Dimension_4096x4096: return { 4096, 4096 };
			}
			LK_CORE_ASSERT(false, "Unknown TextureArrayDimension");
		}

		static TextureArrayDimension DetermineDimension(int width, int height)
		{
			if (width != height)
				height = width;
			switch (width)
			{
				case 200:  return TextureArrayDimension::Dimension_200x200;
				case 400:  return TextureArrayDimension::Dimension_400x400;
				case 800:  return TextureArrayDimension::Dimension_800x800;
				case 1024: return TextureArrayDimension::Dimension_1024x1024;
				case 2048: return TextureArrayDimension::Dimension_2048x2048;
				case 4096: return TextureArrayDimension::Dimension_4096x4096;
			}
			LK_CORE_ASSERT(false, "Unknown dimension arguments, width={}  height={}", width, height);
		}

	public:
		static constexpr int MaxTexturesPerArray = 32;
	public:
		RendererID m_RendererID;
		int m_Width, m_Height;
		TextureArraySpecification m_Specification;
	};


}