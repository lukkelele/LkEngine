#pragma once

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/ArrayTextureSpecification.h"


namespace LkEngine {

	class LArrayTexture : public LObject
	{
	public:
		virtual ~LArrayTexture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void AddTextureToArray(const TObjectPtr<LTexture> InTexture) = 0;
		virtual bool RemoveTextureFromArray(const LRendererID InTextureID) = 0;

		virtual const LRendererID GetRendererID() const = 0;
		virtual LRendererID& GetRendererID() = 0;

		virtual int GetTextureSlot() const = 0;
		virtual const EArrayTextureDimension& GetDimension() const = 0;
		virtual const FArrayTextureSpecification& GetSpecification() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual void SetWidth(const int InWidth) = 0;
		virtual void SetHeight(const int InHeight) = 0;

		virtual bool HasTexture(const TObjectPtr<LTexture>& Texture) = 0;
		virtual TObjectPtr<LTexture> GetTextureWithID(LRendererID id) = 0;
		virtual int GetIndexOfTexture(const TObjectPtr<LTexture>& Texture) = 0;

		static TObjectPtr<LArrayTexture> Create(const FArrayTextureSpecification& Specification);

	private:
		LCLASS(LArrayTexture)
	};

}
