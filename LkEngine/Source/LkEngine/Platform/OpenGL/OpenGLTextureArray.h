#pragma once

#include "LkEngine/Renderer/TextureArray.h"

#include "OpenGLTexture.h"


namespace LkEngine {

	class OpenGLTextureArray : public TextureArray
	{
	public:
		OpenGLTextureArray(const FTextureArraySpecification& InSpecification);
		~OpenGLTextureArray();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void AddTextureToArray(const TObjectPtr<LTexture> Texture) override;
		virtual bool RemoveTextureFromArray(const RendererID TextureID) override;

		FORCEINLINE virtual const RendererID GetRendererID() const override 
		{ 
			return m_RendererID; 
		}

		FORCEINLINE virtual RendererID& GetRendererID() override 
		{ 
			return m_RendererID; 
		}

		FORCEINLINE virtual int GetTextureSlot() const override 
		{ 
			return Specification.TextureSlot; 
		}

		FORCEINLINE virtual const FTextureArraySpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

		FORCEINLINE virtual const ETextureArrayDimension& GetDimension() const override 
		{ 
			return Specification.TextureArrayDimension; 
		}

		FORCEINLINE virtual int GetWidth() const override { return m_Width; }
		FORCEINLINE virtual int GetHeight() const override { return m_Height; }
		FORCEINLINE virtual void SetWidth(const int NewWidth) override { m_Width = NewWidth; }
		FORCEINLINE virtual void SetHeight(const int NewHeight) override { m_Height = NewHeight; }

		FORCEINLINE virtual bool HasTexture(const TObjectPtr<LTexture>& Texture) override
		{
			return (TextureIDSet.find(Texture->GetRendererID()) != TextureIDSet.end());
		}

		FORCEINLINE virtual TObjectPtr<LTexture> GetTextureWithID(RendererID TextureID) override
		{
			if (auto Iter = IndexCache.find(TextureID); Iter != IndexCache.end())
			{
				return (Textures.at((*Iter).second));
			}

			LK_ASSERT(false, "GetTextureWithID failed with ID: {}", TextureID);
			return nullptr;
		}

		FORCEINLINE virtual int GetIndexOfTexture(const TObjectPtr<LTexture>& Texture) override
		{
			for (int i = 0; i < Textures.size(); i++)
			{
				if (Textures[i]->GetRendererID() == Texture->GetRendererID())
				{
					return i;
				}
			}

			return 0; 
		}

	public:
		static constexpr int MaxTexturesPerArray = 32;
	public:
		RendererID m_RendererID = 0;
		FTextureArraySpecification Specification{};

		int m_Width = 0;
		int m_Height = 0;

		std::deque<TObjectPtr<LTexture>> Textures{};
		std::unordered_map<RendererID, int> IndexCache{};
		std::unordered_set<RendererID> TextureIDSet{};
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

			LK_CORE_ASSERT(false, "Invalid dimension arguments,  width={}  height={}", width, height);
			return {};
		}

		static void GenerateTextureArrayImage(RendererID& ID, const FTextureArraySpecification& Specification)
		{
			auto [Width, Height] = GLUtils::ConvertDimensionsToWidthAndHeight(Specification.TextureArrayDimension);
			LK_OpenGL(glTextureStorage3D(ID, 
					                     Specification.Layers, 
							             ImageFormatToGLDataFormat(Specification.ImageFormat), 
							             Width, 
							             Height, 
							             0));
		}

	}

}