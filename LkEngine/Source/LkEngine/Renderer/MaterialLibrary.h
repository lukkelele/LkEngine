#pragma once

#include "Material.h"

///
/// UPDATE / REFACTOR
///

namespace LkEngine {

    static constexpr const char* BASE_MATERIAL = "BaseMaterial";

    class LMaterialAsset;
	/// FIXME: MOVE THIS TO ASSET MANAGER ENTIRELY

    class LMaterialLibrary : public LObject
    {
    public:
        LMaterialLibrary();

        static LMaterialLibrary& Get();

        void Initialize();
        TObjectPtr<LMaterialAsset> Add(const TObjectPtr<LMaterial> InMaterial);

        TObjectPtr<LMaterialAsset> GetMaterial(const FAssetHandle Handle);
        TObjectPtr<LMaterialAsset> GetMaterial(const std::string& InMaterialName);

        FORCEINLINE int GetMaterials(std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>>& InMaterialMap) const
        { 
            InMaterialMap.clear();
            InMaterialMap = LoadedMaterialAssets;

            return static_cast<int>(InMaterialMap.size());
        }

        std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>>& GetLoadedMaterialAssets() 
        { 
            return LoadedMaterialAssets; 
        }

		std::string DumpLoadedAssets();

    private:
        /** Initialization for basic materials. */
        void CreateBasicMaterials();

	public:
		static TObjectPtr<LMaterialAsset> BaseMaterial;
    private:
		inline static std::unordered_map<FAssetHandle, TObjectPtr<LMaterialAsset>> LoadedMaterialAssets;
        inline static std::unordered_map<std::string, FAssetHandle> AssetNameMap;

        LCLASS(LMaterialLibrary)
    };

}
