#pragma once

#include "Material.h"

///
/// UPDATE / REFACTOR
///

namespace LkEngine {

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
        inline static std::unordered_map<std::string, TObjectPtr<LMaterial>> m_Collection;
		inline static std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>> m_LoadedMaterialAssets;

        LCLASS(LMaterialLibrary)
    };

}