#pragma once

#include "Material.h"


namespace LkEngine {

    class MaterialAsset;

    class MaterialLibrary : public RefCounted
    {
    public:
        MaterialLibrary();
        ~MaterialLibrary() = default;

        void Init();
        void Add(const Ref<Material> material);

        Ref<Material> GetMaterial(std::string_view materialName);
        std::unordered_map<std::string, Ref<Material>>& GetMaterials() { return m_Collection; }
        std::unordered_map<AssetHandle, Ref<MaterialAsset>>& GetLoadedMaterialAssets() { return m_LoadedMaterialAssets; }

        Ref<Material> GetBaseMaterial();

    private:
        void CreateBasicMaterials();

    private:
        std::unordered_map<std::string, Ref<Material>> m_Collection;
		std::unordered_map<AssetHandle, Ref<MaterialAsset>> m_LoadedMaterialAssets;
    };

}