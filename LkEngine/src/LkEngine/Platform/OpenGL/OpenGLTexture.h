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

		RendererID GetRendererID() const override;
		RendererID& GetRendererID();
		std::string GetName() const override { return m_Specification.Name; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		FilePath GetPath() const { return m_FilePath; }
		FilePath& GetPath() { return m_FilePath; }

		uint8_t GetTextureArrayIndex() const { return m_TextureArrayIndex; }

	protected:
		Ref<Image> m_Image;
		uint32_t m_Width, m_Height;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		float m_ScalerX = 1.0f, m_ScalerY = 1.0f;
		bool m_Loaded = false;
		bool m_Locked = false;

		uint8_t m_TextureArrayIndex = 0;
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

		uint8_t GetTextureArrayIndex() const { return m_TextureArrayIndex; }

	private:
		Ref<Image2D> m_Image = nullptr;
		TextureSpecification m_Specification;
		FilePath m_FilePath;

		bool m_Loaded = false;
		bool m_Locked = false;

		uint8_t m_TextureArrayIndex = 0;
	};


	enum TextureArrayDimension : uint8_t
	{
		Dimension_200x200 = 0,
		Dimension_512x512,
		Dimension_1024x1024,
		Dimension_2048x2048,
		Dimension_4096x4096,
	};

	struct TextureArraySpecification
	{
		TextureArrayDimension Dimension = Dimension_1024x1024;
		ImageFormat Format = ImageFormat::RGBA;
		int TextureSlot = 0;
		std::string DebugName;
	};

	class TextureArray : public RefCounted
	{
	public:
		TextureArray(const TextureArraySpecification& specification);

		void Bind();
		void Unbind();
		static void UnbindAll(int slot = 0);

		void AddTextureToArray(Ref<Texture> texture);

		bool RemoveTextureFromArray(RendererID& rendererID)
		{
			//m_TextureIDs.erase(rendererID);
		}

		const RendererID GetRendererID() const { return m_RendererID; }
		RendererID& GetRendererID() { return m_RendererID; }

		int GetTextureSlot() const { return m_Specification.TextureSlot; }
		const TextureArrayDimension& GetDimension() const { return m_Specification.Dimension; }
		const TextureArraySpecification& GetSpecification() const { return m_Specification; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }

		bool HasTexture(const Ref<Texture>& texture)
		{
			const RendererID textureRendererID = texture->GetRendererID();
			for (auto& t : m_Textures)
			{
				if (t->GetRendererID() == textureRendererID)
					return true;
			}
			return false;
		}

		Ref<Texture> GetTextureWithID(RendererID id)
		{
			for (auto& t : m_Textures)
			{
				if (t->GetRendererID() == id)
					return t;
			}
			return nullptr;
		}

		float GetIndexOfTexture(const Ref<Texture>& texture)
		{
			for (int i = 0; i < m_Textures.size(); i++)
			{
				LK_CORE_TRACE("{} GetIndexOfTexture - Current texture: {}", i, m_Textures[i]->GetName());
				if (m_Textures[i]->GetRendererID() == texture->GetRendererID())
					return (float)(i); // 
			}
			return 0.0f;
		}

	public:
		static constexpr int MaxTexturesPerArray = 32;
	public:
		RendererID m_RendererID;
		int m_Width, m_Height;
		TextureArraySpecification m_Specification;

		//std::vector<Ref<Texture>> m_Textures{};
		std::deque<Ref<Texture>> m_Textures{};
		std::vector<RendererID> m_TextureIDs{};

		friend class OpenGLRenderer;
		friend class OpenGLRenderer2D;
	};


	namespace GLUtils {

		static std::pair<int, int> ConvertDimensionsToWidthAndHeight(const TextureArrayDimension& dimension)
		{
			switch (dimension)
			{
				case TextureArrayDimension::Dimension_200x200:   return { 200, 200 };
				case TextureArrayDimension::Dimension_512x512:   return { 512, 512 };
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
				case 512:  return TextureArrayDimension::Dimension_512x512;
				case 1024: return TextureArrayDimension::Dimension_1024x1024;
				case 2048: return TextureArrayDimension::Dimension_2048x2048;
				case 4096: return TextureArrayDimension::Dimension_4096x4096;
			}
			LK_CORE_ASSERT(false, "Unknown dimension arguments, width={}  height={}", width, height);
		}

	}

}