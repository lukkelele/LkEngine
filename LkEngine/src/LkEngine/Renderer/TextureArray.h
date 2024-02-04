#pragma once

#include "Texture.h"


namespace LkEngine {

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
		int Layers = 10;
		std::string DebugName;
	};

	class TextureArray : public RefCounted
	{
	public:
		virtual ~TextureArray() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void AddTextureToArray(Ref<Texture> texture) = 0;
		virtual bool RemoveTextureFromArray(RendererID& rendererID) = 0;

		virtual const RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual int GetTextureSlot() const = 0;
		virtual const TextureArrayDimension& GetDimension() const = 0;
		virtual const TextureArraySpecification& GetSpecification() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual void SetWidth(int width) = 0;
		virtual void SetHeight(int height) = 0;

		virtual bool HasTexture(const Ref<Texture>& texture) = 0;
		virtual Ref<Texture> GetTextureWithID(RendererID id) = 0;
		virtual int GetIndexOfTexture(const Ref<Texture>& texture) = 0;

		static Ref<TextureArray> Create(const TextureArraySpecification& specification);

	};

}