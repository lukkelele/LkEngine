#pragma once

#include "Material.h"

///
/// UPDATE / REFACTOR
///

namespace LkEngine {

    static constexpr const char* BASE_MATERIAL = "BaseMaterial";

    class LMaterialAsset;

    class LMaterialLibrary : public LObject
    {
    public:
        LMaterialLibrary();

        static LMaterialLibrary& Get();

        void Initialize();
        void Add(const TObjectPtr<LMaterial> InMaterial);

        TObjectPtr<LMaterialAsset> GetMaterial(const FAssetHandle Handle);
        TObjectPtr<LMaterialAsset> GetMaterial(std::string_view InMaterialName);

        FORCEINLINE int GetMaterials(std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>>& InMaterialMap) const
        { 
            InMaterialMap.clear();
            InMaterialMap = m_LoadedMaterialAssets;

            return static_cast<int>(InMaterialMap.size());
        }

        FORCEINLINE std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>>& GetLoadedMaterialAssets() 
        { 
            return m_LoadedMaterialAssets; 
        }

		std::string DumpLoadedAssets();

    private:
        /** Initialization for basic materials. */
        void CreateBasicMaterials();

    private:
		inline static std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>> m_LoadedMaterialAssets;
        inline static std::unordered_map<std::string, FAssetHandle> m_Collection;

        LCLASS(LMaterialLibrary)
    };

}
