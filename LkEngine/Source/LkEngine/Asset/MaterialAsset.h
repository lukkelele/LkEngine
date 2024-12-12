#pragma once

#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class LMaterialAsset : public LAsset
    {
    public:
		explicit LMaterialAsset(const TObjectPtr<LMaterial>& Material);
        LMaterialAsset(const bool bIsTransparent = false);
		LMaterialAsset() = delete;
        ~LMaterialAsset() = default;

		static EAssetType GetStaticType() { return EAssetType::Material; }

		glm::vec3& GetAlbedoColor();
		void SetAlbedoColor(const glm::vec3& color);

		float& GetMetalness();
		void SetMetalness(float value);

		float& GetRoughness();
		void SetRoughness(float value);

		float& GetEmission();
		void SetEmission(float value);

		TObjectPtr<LMaterial> GetMaterial() const { return Material; }

		void SetMaterial(TObjectPtr<LMaterial> Material) 
		{ 
			LK_CORE_VERIFY(Material);
			Material = Material; 
		}

        void SetTransparency(float transparency);
        float& GetTransparency();
		bool IsTransparent() const { return m_Transparent; }

        TObjectPtr<LTexture2D> GetAlbedoMap();
        void SetAlbedoMap(TObjectPtr<LTexture2D> texture);
        void ClearAlbedoMap();

        TObjectPtr<LTexture2D> GetNormalMap();
        void SetNormalMap(TObjectPtr<LTexture2D> texture);

        TObjectPtr<LTexture2D> GetMetalnessMap();
        void SetMetalnessMap(TObjectPtr<LTexture2D> texture);
        void ClearMetalnessMap();

        TObjectPtr<LTexture2D> GetRoughnessMap();
        void SetRoughnessMap(TObjectPtr<LTexture2D> texture);
        void ClearRoughnessMap();

	private:
		void SetDefaults();

	private:
		TObjectPtr<LMaterial> Material = nullptr;
		bool m_Transparent = false;

		friend class MaterialEditor;

        LASSET(LMaterialAsset)
    };


    class LMaterialTable : public LObject
    {
    public:
        LMaterialTable(const uint32_t MaterialCount = 1);
        LMaterialTable(TObjectPtr<LMaterialTable> Other);
        ~LMaterialTable() = default;

        void SetMaterial(const uint32_t Index, const FAssetHandle AssetHandle);

		FORCEINLINE FAssetHandle GetMaterial(const uint32_t Index) const
		{
			LK_CORE_ASSERT(HasMaterial(Index), "GetMaterial failed for index {}", Index);
			return Materials.at(Index);
		}

        void ClearMaterial(const uint32_t Index);
        void InsertLast(const FAssetHandle Material);

        FORCEINLINE bool HasMaterial(const uint32_t Index) const 
        { 
            return (Materials.find(Index) != Materials.end());
        }

        FORCEINLINE FAssetHandle GetMaterialHandle(const uint32_t Index) const
        {
            LK_CORE_ASSERT(HasMaterial(Index));
            return Materials.at(Index);
        }

        FORCEINLINE uint32_t GetMaterialCount() const 
        { 
            return MaterialCount; 
        }

        FORCEINLINE void SetMaterialCount(const uint32_t InMaterialCount) 
        { 
            MaterialCount = InMaterialCount;
        }

        /**
         * @brief
         */
        void Clear();

        std::map<uint32_t, FAssetHandle>& GetMaterials() 
		{ 
			return Materials; 
		}

        const std::map<uint32_t, FAssetHandle>& GetMaterials() const 
		{ 
			return Materials; 
		}

    private:
		std::map<uint32_t, FAssetHandle> Materials{};
        uint32_t MaterialCount = 0;

        LCLASS(LMaterialTable)
    };



}
