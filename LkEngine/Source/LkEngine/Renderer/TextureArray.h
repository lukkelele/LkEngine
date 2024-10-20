#pragma once

#include "Texture.h"


namespace LkEngine {

	enum ETextureArrayDimension : uint8_t
	{
		Dimension_200x200 = 0,
		Dimension_512x512,
		Dimension_1024x1024,
		Dimension_2048x2048,
		Dimension_4096x4096,
	};

	struct FTextureArraySpecification
	{
		ETextureArrayDimension TextureArrayDimension = ETextureArrayDimension::Dimension_1024x1024;
		EImageFormat ImageFormat = EImageFormat::RGBA;
		int TextureSlot = 0;
		int Layers = 10;
		int Mipmaps = 5;
		std::string DebugName{};
	};

	class TextureArray : public LObject
	{
	public:
		virtual ~TextureArray() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void AddTextureToArray(const TObjectPtr<LTexture> Texture) = 0;
		virtual bool RemoveTextureFromArray(const RendererID TextureID) = 0;

		virtual const RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual int GetTextureSlot() const = 0;
		virtual const ETextureArrayDimension& GetDimension() const = 0;
		virtual const FTextureArraySpecification& GetSpecification() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual void SetWidth(int width) = 0;
		virtual void SetHeight(int height) = 0;

		virtual bool HasTexture(const TObjectPtr<LTexture>& Texture) = 0;
		virtual TObjectPtr<LTexture> GetTextureWithID(RendererID id) = 0;
		virtual int GetIndexOfTexture(const TObjectPtr<LTexture>& Texture) = 0;

		static TObjectPtr<TextureArray> Create(const FTextureArraySpecification& Specification);

	private:
		LCLASS(TextureArray)
	};

	namespace Utils {

		static std::string TextureArrayDimensionToString(ETextureArrayDimension TextureArrayDimension)
		{
			switch (TextureArrayDimension)
			{
				case ETextureArrayDimension::Dimension_512x512:   return "512x512";
				case ETextureArrayDimension::Dimension_1024x1024: return "1024x1024";
				case ETextureArrayDimension::Dimension_2048x2048: return "2048x2048";
				case ETextureArrayDimension::Dimension_4096x4096: return "4096x4096";
			}

			LK_CORE_ASSERT(false, "Unknown enum value of ETextureArrayDimension: {}", static_cast<int>(TextureArrayDimension));
			return {};
		}

	}

}