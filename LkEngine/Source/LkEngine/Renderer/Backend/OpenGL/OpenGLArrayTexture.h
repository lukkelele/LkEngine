#pragma once

#include "LkEngine/Renderer/ArrayTexture.h"

#include "OpenGLTexture.h"


namespace LkEngine {

	class LOpenGLArrayTexture : public LArrayTexture
	{
	public:
		LOpenGLArrayTexture(const FArrayTextureSpecification& InSpecification);
		LOpenGLArrayTexture() = delete;
		~LOpenGLArrayTexture();

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

		FORCEINLINE virtual const FArrayTextureSpecification& GetSpecification() const override 
		{ 
			return Specification; 
		}

		FORCEINLINE virtual const EArrayTextureDimension GetDimension() const override 
		{ 
			return Specification.Dimension; 
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
		FArrayTextureSpecification Specification{};

		int m_Width = 0;
		int m_Height = 0;

		std::deque<TObjectPtr<LTexture>> Textures{};
		std::unordered_map<LRendererID, int> IndexCache{};
		std::unordered_set<LRendererID> TextureIDSet{};

	private:
		LCLASS(LOpenGLArrayTexture);
	};

}
