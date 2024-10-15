#pragma once

#include "LkEngine/Renderer/TextureArray.h"

#include "OpenGLTexture.h"


namespace LkEngine {

	class OpenGLTextureArray : public TextureArray
	{
	public:
		OpenGLTextureArray(const TextureArraySpecification& specification);
		~OpenGLTextureArray();

		void Bind() override;
		void Unbind() override;

		void AddTextureToArray(TObjectPtr<LTexture> texture) override;
		bool RemoveTextureFromArray(RendererID& rendererID) override;

		const RendererID GetRendererID() const override { return m_RendererID; }
		RendererID& GetRendererID() override { return m_RendererID; }

		int GetTextureSlot() const override { return m_Specification.TextureSlot; }
		const ETextureArrayDimension& GetDimension() const override { return m_Specification.TextureArrayDimension; }
		const TextureArraySpecification& GetSpecification() const override { return m_Specification; }

		int GetWidth() const override { return m_Width; }
		int GetHeight() const override { return m_Height; }
		void SetWidth(int width) override { m_Width = width; }
		void SetHeight(int height) override { m_Height = height; }

		bool HasTexture(const TObjectPtr<LTexture>& texture) override
		{
			const RendererID textureRendererID = texture->GetRendererID();
			for (auto& t : m_Textures)
			{
				if (t->GetRendererID() == textureRendererID)
					return true;
			}
			return false;
		}

		TObjectPtr<LTexture> GetTextureWithID(RendererID id) override
		{
			for (auto& t : m_Textures)
			{
				if (t->GetRendererID() == id)
					return t;
			}
			return nullptr;
		}

		int GetIndexOfTexture(const TObjectPtr<LTexture>& texture) override
		{
			for (int i = 0; i < m_Textures.size(); i++)
			{
				if (m_Textures[i]->GetRendererID() == texture->GetRendererID())
					return i;
			}
			return 0; 
		}

	public:
		static constexpr int MaxTexturesPerArray = 32;
	public:
		RendererID m_RendererID;
		int m_Width, m_Height;
		TextureArraySpecification m_Specification;

		std::deque<TObjectPtr<LTexture>> m_Textures{};

		friend class OpenGLRenderer;
		friend class OpenGLRenderer2D;
	};


	namespace GLUtils {

		static std::pair<int, int> ConvertDimensionsToWidthAndHeight(const ETextureArrayDimension& TextureArrayDimension)
		{
			switch (TextureArrayDimension)
			{
				case ETextureArrayDimension::Dimension_200x200:   return { 200, 200 };
				case ETextureArrayDimension::Dimension_512x512:   return { 512, 512 };
				case ETextureArrayDimension::Dimension_1024x1024: return { 1024, 1024 };
				case ETextureArrayDimension::Dimension_2048x2048: return { 2048, 2048 };
				case ETextureArrayDimension::Dimension_4096x4096: return { 4096, 4096 };
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLTextureArrayDimension");
			return {};
		}

		static ETextureArrayDimension DetermineDimension(int width, int height)
		{
			if (width != height)
			{
				height = width;
			}

			switch (width)
			{
				case 200:  return ETextureArrayDimension::Dimension_200x200;
				case 512:  return ETextureArrayDimension::Dimension_512x512;
				case 1024: return ETextureArrayDimension::Dimension_1024x1024;
				case 2048: return ETextureArrayDimension::Dimension_2048x2048;
				case 4096: return ETextureArrayDimension::Dimension_4096x4096;
			}

			LK_CORE_ASSERT(false, "Unknown dimension arguments, width={}  height={}", width, height);
			return {};
		}

		static void GenerateTextureArrayImage(RendererID& rendererID, const TextureArraySpecification& Specification)
		{
			auto [Width, Height] = GLUtils::ConvertDimensionsToWidthAndHeight(Specification.TextureArrayDimension);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GLUtils::ImageFormatToGLDataFormat(specification.Format), width, height, specification.Layers, 0, GLUtils::ImageFormatToGLDataFormat(specification.Format), GL_UNSIGNED_BYTE, nullptr);
			//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GLUtils::ImageFormatToGLDataFormat(specification.Format), width, height, specification.Layers, 0, GLUtils::ImageFormatToGLDataFormat(specification.Format), GL_UNSIGNED_BYTE, nullptr);
			glTextureStorage3D(rendererID, Specification.Layers, ImageFormatToGLDataFormat(Specification.Format), Width, Height, 0);
		}

	}

}