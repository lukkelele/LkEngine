#pragma once

#include "LkEngine/Renderer/TextureArray.h"

#include "OpenGLTexture.h"


namespace LkEngine {

	class LOpenGLTextureArray : public LTextureArray
	{
	public:
		LOpenGLTextureArray(const FTextureArraySpecification& InSpecification);
		LOpenGLTextureArray() = delete;
		~LOpenGLTextureArray();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void AddTextureToArray(const TObjectPtr<LTexture> Texture) override;
		virtual bool RemoveTextureFromArray(const LRendererID TextureID) override;

		FORCEINLINE virtual const LRendererID GetRendererID() const override 
		{ 
			return m_RendererID; 
		}

		FORCEINLINE virtual LRendererID& GetRendererID() override 
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

		FORCEINLINE virtual TObjectPtr<LTexture> GetTextureWithID(LRendererID TextureID) override
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
		LRendererID m_RendererID = 0;
		FTextureArraySpecification Specification{};

		int m_Width = 0;
		int m_Height = 0;

		std::deque<TObjectPtr<LTexture>> Textures{};
		std::unordered_map<LRendererID, int> IndexCache{};
		std::unordered_set<LRendererID> TextureIDSet{};

	private:
		LCLASS(LOpenGLTextureArray);
	};


	namespace LOpenGL {

		/* TODO: MOVE ELSEWHERE. */
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

		static ETextureArrayDimension DetermineDimension(const int InWidth, const int InHeight)
		{
			switch (InWidth)
			{
				case 200:  return ETextureArrayDimension::Dimension_200x200;
				case 512:  return ETextureArrayDimension::Dimension_512x512;
				case 1024: return ETextureArrayDimension::Dimension_1024x1024;
				case 2048: return ETextureArrayDimension::Dimension_2048x2048;
				case 4096: return ETextureArrayDimension::Dimension_4096x4096;
			}

			LK_CORE_ASSERT(false, "Failed to determine dimension");
			return {};
		}

		static void GenerateTextureArrayImage(LRendererID& ID, const FTextureArraySpecification& Specification)
		{
			auto [Width, Height] = LOpenGL::ConvertDimensionsToWidthAndHeight(Specification.TextureArrayDimension);
			LK_OpenGL(glTextureStorage3D(ID, 
					                     Specification.Layers, 
							             ImageFormatToGLDataFormat(Specification.ImageFormat), 
							             Width, 
							             Height, 
							             0));
		}

	}

}