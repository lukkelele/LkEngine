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

        TObjectPtr<LMaterial> GetMaterial(std::string_view InMaterialName);

        FORCEINLINE std::unordered_map<std::string, TObjectPtr<LMaterial>>& GetMaterials() 
        { 
            return m_Collection; 
        }

        FORCEINLINE int GetMaterials(std::unordered_map<std::string, TObjectPtr<LMaterial>>& InMaterialMap) const
        { 
            InMaterialMap.clear();
            InMaterialMap = m_Collection;

            return static_cast<int>(InMaterialMap.size());
        }

        FORCEINLINE std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>>& GetLoadedMaterialAssets() 
        { 
            return m_LoadedMaterialAssets; 
        }

        /// TODO
        /** Return the base (NULL) material. */
        TObjectPtr<LMaterial> GetBaseMaterial();

    private:
        /** Initialization for basic materials. */
        void CreateBasicMaterials();

    private:
        /// @TODO: Why on earth is the key a string? FIX !!!
        inline static std::unordered_map<std::string, TObjectPtr<LMaterial>> m_Collection;
		inline static std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>> m_LoadedMaterialAssets;

        LCLASS(LMaterialLibrary)
    };

}