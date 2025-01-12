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

        TObjectPtr<LMaterialAsset> GetMaterial(const LUUID Handle);
        TObjectPtr<LMaterialAsset> GetMaterial(std::string_view InMaterialName);

        FORCEINLINE int GetMaterials(std::unordered_map<LUUID, TObjectPtr<LMaterialAsset>>& InMaterialMap) const
        { 
            InMaterialMap.clear();
            InMaterialMap = m_LoadedMaterialAssets;

            return static_cast<int>(InMaterialMap.size());
        }

        FORCEINLINE std::unordered_map<LUUID, TObjectPtr<LMaterialAsset>>& GetLoadedMaterialAssets() 
        { 
            return m_LoadedMaterialAssets; 
        }

		std::string DumpLoadedAssets();

    private:
        /** Initialization for basic materials. */
        void CreateBasicMaterials();

	public:
		static TObjectPtr<LMaterialAsset> BaseMaterial;
    private:
		inline static std::unordered_map<LUUID, TObjectPtr<LMaterialAsset>> m_LoadedMaterialAssets;
        inline static std::unordered_map<std::string, LUUID> m_Collection;

        LCLASS(LMaterialLibrary)
    };

}
