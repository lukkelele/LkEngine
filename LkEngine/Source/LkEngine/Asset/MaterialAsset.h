#pragma once

#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class LMaterialAsset : public LAsset
    {
    public:
		LMaterialAsset(const TObjectPtr<LMaterial>& material);
        LMaterialAsset(const bool bIsTransparent = false);
        ~LMaterialAsset() = default;

		static EAssetType GetStaticType() { return EAssetType::Material; }
		//virtual EAssetType GetAssetType() const override { return GetStaticType(); }

		glm::vec3& GetAlbedoColor();
		void SetAlbedoColor(const glm::vec3& color);

		float& GetMetalness();
		void SetMetalness(float value);

		float& GetRoughness();
		void SetRoughness(float value);

		float& GetEmission();
		void SetEmission(float value);

		TObjectPtr<LMaterial> GetMaterial() const { return m_Material; }
		void SetMaterial(TObjectPtr<LMaterial> material) { m_Material = material; }

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
		TObjectPtr<LMaterial> m_Material = nullptr;
		bool m_Transparent = false;

		friend class MaterialEditor;

        LASSET(LMaterialAsset)
    };


    class LMaterialTable : public LObject
    {
    public:
        LMaterialTable(uint32_t materialCount = 1);
        LMaterialTable(TObjectPtr<LMaterialTable> Other);
        ~LMaterialTable() = default;

        void SetMaterial(uint32_t index, FAssetHandle material);
        void ClearMaterial(uint32_t index);

        void InsertLast(FAssetHandle material);

        bool HasMaterial(uint32_t materialIndex) const 
        { 
            return m_Materials.find(materialIndex) != m_Materials.end(); 
        }

        FAssetHandle GetMaterialHandle(uint32_t materialIndex) const
        {
            LK_CORE_ASSERT(HasMaterial(materialIndex));
            return m_Materials.at(materialIndex);
        }

        std::map<uint32_t, FAssetHandle>& GetMaterials() { return m_Materials; }
        const std::map<uint32_t, FAssetHandle>& GetMaterials() const { return m_Materials; }

        FORCEINLINE uint32_t GetMaterialCount() const 
        { 
            return m_MaterialCount; 
        }

        FORCEINLINE void SetMaterialCount(uint32_t materialCount) 
        { 
            m_MaterialCount = materialCount;
        }

        void Clear();

    private:
        std::map<uint32_t, FAssetHandle> m_Materials;
        uint32_t m_MaterialCount;

        LCLASS(LMaterialTable)
    };



}