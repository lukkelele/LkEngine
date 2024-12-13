#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"

#include "Renderer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

    LMaterialLibrary::LMaterialLibrary()
    {
		LOBJECT_REGISTER();
    }

    LMaterialLibrary& LMaterialLibrary::Get()
    {
        static LMaterialLibrary MaterialLibrary;
        return MaterialLibrary;
    }

    void LMaterialLibrary::Initialize()
    {
		LK_CORE_INFO_TAG("MaterialLibrary", "Creating base material");
        TObjectPtr<LMaterial> BaseMaterial = LMaterial::Create(
			LRenderer::GetShaderLibrary()->Get("Renderer_Model"), /*  Shader.         */
            BASE_MATERIAL                                         /*  Material Name.  */
        );
        //m_Collection.emplace(BaseMaterial->GetName(), BaseMaterial);

		Add(BaseMaterial);
		//LMaterialAsset BaseMatAsset(BaseMaterial);
		//m_LoadedMaterialAssets.emplace(BaseMatAsset.Handle, BaseMatAsset);

        LK_CORE_DEBUG_TAG("MaterialLibrary", "Creating basic materials");
        CreateBasicMaterials();
    }

    void LMaterialLibrary::Add(const TObjectPtr<LMaterial> Material)
    {
		LK_VERIFY(Material);
	#if 0
		LMaterialAsset MatAsset(Material);
		m_LoadedMaterialAssets.emplace(MatAsset.Handle, MatAsset);
		m_Collection.emplace(Material->GetName(), MatAsset.Handle);
	#endif

		LK_CORE_INFO("Adding {} to material library", Material->GetName());
		TObjectPtr<LMaterialAsset> MatAsset = TObjectPtr<LMaterialAsset>::Create(Material);
		//m_LoadedMaterialAssets.insert({ MatAsset->Handle, MatAsset });
		m_LoadedMaterialAssets.emplace(MatAsset->Handle, MatAsset);
		m_Collection.emplace(Material->GetName(), MatAsset->Handle);
    }

    TObjectPtr<LMaterialAsset> LMaterialLibrary::GetMaterial(const FAssetHandle Handle)
	{
        if (auto Iter = m_LoadedMaterialAssets.find(Handle); Iter != m_LoadedMaterialAssets.end())
        {
            return Iter->second;
        }

        LK_CORE_ERROR_TAG("MaterialLibrary", "Failed to get material with handle \"{}\"", Handle);
        return nullptr;
	}

	TObjectPtr<LMaterialAsset> LMaterialLibrary::GetMaterial(std::string_view InMaterialName)
    {
        if (auto Iter = m_Collection.find(std::string(InMaterialName)); Iter != m_Collection.end())
        {
			return m_LoadedMaterialAssets.at(Iter->second);
        }

        return nullptr;
    }

    std::string LMaterialLibrary::DumpLoadedAssets()
	{
		std::string String;
		for (const auto& [MatAssetHandle, MatAsset] : m_LoadedMaterialAssets)
		{
			String += LK_FORMAT_STRING("Material Asset {}: {}\n", MatAssetHandle, MatAsset->GetMaterial()->GetName());
		}

		return String;
	}

	void LMaterialLibrary::CreateBasicMaterials()
    {
        // Asphalt
        {
            TObjectPtr<LMaterial> Asphalt = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Asphalt"
            );
            TObjectPtr<LMaterialAsset> MaterialAsset(Asphalt);
            m_Collection.emplace(Asphalt->GetName(), Asphalt);
            m_LoadedMaterialAssets.emplace(MaterialAsset->Handle, MaterialAsset);

            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Asphalt'");
        }

        // Rubber
        {
            TObjectPtr<LMaterial> Rubber = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Rubber"
            );
            TObjectPtr<LMaterialAsset> MaterialAsset(Rubber);
            m_Collection.emplace(Rubber->GetName(), Rubber);
            m_LoadedMaterialAssets.emplace(MaterialAsset->Handle, MaterialAsset);
            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Rubber'");
        }
    }

}
